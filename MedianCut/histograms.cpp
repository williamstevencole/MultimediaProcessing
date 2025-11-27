#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <map>

namespace py = pybind11;

struct ColorBin{
    uint8_t r, g, b;
    int count;

    uint8_t getChannel(int channel_index) const{
        if(channel_index == 0) return r;
        if(channel_index == 1) return g;
        return b;
    }
};

bool compareRed(const ColorBin &a, const ColorBin &b){
    return a.r < b.r;
}

bool compareGreen(const ColorBin &a, const ColorBin &b){
    return a.g < b.g;
}

bool compareBlue(const ColorBin &a, const ColorBin &b){
    return a.b < b.b;
}


class Bucket{
    public:
        std::vector<ColorBin> colors;
        uint64_t total_pixel_count = 0;

        void addColor(const ColorBin &bin){
            colors.push_back(bin);
            total_pixel_count += bin.count;
        }

        bool canSplit() const{
            return colors.size() > 1;
        }

        ColorBin getWeightedAverage(){
            uint64_t sum_r = 0, sum_g = 0, sum_b = 0;

            for(const auto &c : colors){
                sum_r += c.r * c.count;
                sum_g += c.g * c.count;
                sum_b += c.b * c.count;
            }

            if(total_pixel_count == 0) return {0,0,0};

            return {
                static_cast<uint8_t>(sum_r/total_pixel_count),
                static_cast<uint8_t>(sum_g/total_pixel_count),
                static_cast<uint8_t>(sum_b/total_pixel_count),
                0
            };
        }

        int getLargestChannelRange(){
            int max_r = 0, max_g = 0, max_b = 0;
            int min_r = 255, min_g = 255, min_b = 255;

            for(const auto &c : colors){
                if(max_r < c.r) max_r = c.r;
                if(max_g < c.g) max_g = c.g;
                if(max_b < c.b) max_b = c.b;

                if(min_r > c.r) min_r = c.r;
                if(min_g > c.g) min_g = c.g;
                if(min_b > c.b) min_b = c.b;
            }

            int r_range = max_r - min_r;
            int g_range = max_g - min_g;
            int b_range = max_b - min_b;

            if(r_range > b_range && r_range > g_range) return 0;
            if(b_range > r_range && b_range > g_range) return 2;
            return 1;
        }

        Bucket split(){
            int channel = getLargestChannelRange();

            if(channel== 0){
                std::sort(colors.begin(), colors.end(), compareRed);
            } else if(channel== 1){
                std::sort(colors.begin(), colors.end(), compareGreen);
            } else {
                std::sort(colors.begin(), colors.end(), compareBlue);
            }

            uint64_t target_count = total_pixel_count / 2;
            uint64_t current_count = 0;
            int split_index = 0;

            for(int i = 0; i < colors.size(); i++){
                current_count += colors[i].count;
                if(current_count > target_count){
                    split_index = i + 1;

                    if(split_index >= colors.size() && colors.size() > 1){
                        split_index = colors.size() -1;
                    }
                    break;
                }
            }

            Bucket new_bucket;
            for(int i = split_index; i < colors.size(); i++){
                new_bucket.addColor(colors[i]);
            }

            colors.erase(colors.begin() + split_index, colors.end());

            this->total_pixel_count = 0;
            for(const auto &c : colors){
                this->total_pixel_count += c.count;
            }

            return new_bucket;
        }
};

py::array_t<uint8_t> medianCutHistogram(py::array_t<uint8_t> img, int num_colors){
    auto buff = img.request();
    if(buff.ndim != 3) throw std::runtime_error("Image must be RGB");

    uint8_t *ptr = static_cast<uint8_t*>(buff.ptr);
    ssize_t total_pixels = buff.shape[0] * buff.shape[1];
    ssize_t channels = buff.shape[2];

    std::map<uint32_t, int> histogram;
    for(int i = 0; i < total_pixels; i++){
        uint8_t r = ptr[i*channels];
        uint8_t g = ptr[i*channels + 1];
        uint8_t b = ptr[i*channels + 2];
        uint32_t key = (r << 16) | (g << 8) | b;
        histogram[key]++;
    }

    Bucket initial_bucket;
    for(auto const& [key, count] : histogram){
        uint8_t r = (key >> 16) & 0xFF;
        uint8_t g = (key >> 8) & 0xFF;
        uint8_t b = key & 0xFF;
        initial_bucket.addColor({r,g,b,count});
    }

    std::vector<Bucket> buckets;
    buckets.push_back(initial_bucket);

    while(buckets.size() < num_colors){
        int best_bucket_idx = -1;

        for(int i = 0; i < buckets.size(); i++){
            if(buckets[i].canSplit()){
                best_bucket_idx = i;
                break;
            }
        }

        if(best_bucket_idx == -1) break;

        Bucket &to_split = buckets[best_bucket_idx];
        Bucket new_bucket = to_split.split();
        buckets.push_back(new_bucket);
    }

    ssize_t final_colors == buckets.size();
    py::array_t<result> ({final_colors,3 });
    auto rbuff = result.request();
    uint8_t* rptr = static_cast<uint8_t*>(rbuff.ptr);

    for(int i = 0; i < final_colors; i++){
        ColorBin avg = buckets[i].getWeightedAverage();

        rptr[i*3] = avg.r;
        rptr[i*3 + 1] = avg.g;
        rptr[i*3 + 2] = avg.b;
    }

    return result;
}

