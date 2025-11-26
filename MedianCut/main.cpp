#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace py = pybind11;

struct Color{
    uint8_t r,g,b;
    int original_index;
};

struct Box{
    int start_index, end_index;
};


bool compareRed(const Color &a, const Color &b){
    return a.r < b.r;
}

bool compareGreen(const Color &a, const Color &b){
    return a.g < b.g;
}

bool compareBlue(const Color &a, const Color &b){
    return a.b < b.b;
}

py::array_t<uint8_t> medianCut(
    py::array_t<uint8_t> input_image,
    int num_colors
){

    auto buff = input_image.request();
    if(buff.ndim != 3) throw std::runtime_error("Image must be RGB");

    ssize_t H = buff.shape[0];
    ssize_t W = buff.shape[1];
    ssize_t channels = buff.shape[2];

    ssize_t imageSize = H*W;

    uint8_t* ptr = static_cast<uint8_t*>(buff.ptr);

    std::vector<Color> pixels;
    pixels.reserve(imageSize);

    // Put all pixels on vector
    for(int i = 0; i < imageSize; i++){
        pixels.push_back({
            ptr[i*channels], // R
            ptr[i*channels + 1], //G
            ptr[i*channels + 2], //B
            i
        });
    }

    std::vector<Box> boxes;
    boxes.push_back({0, static_cast<int>(pixels.size())});

    while(boxes.size() < num_colors){
        int split_index = -1;
        int max_range = -1;
        int channel_to_sort = -1; //0:R, 1:G, 2:B

        for(int i = 0; i < boxes.size(); i++){
            int start = boxes[i].start_index;
            int end = boxes[i].end_index;

            if(end - start <= 1) continue;

            uint8_t min_r = 255;
            uint8_t max_r = 0;
            uint8_t min_g = 255;
            uint8_t max_g = 0;
            uint8_t min_b = 255;
            uint8_t max_b = 0;

            for(int k = start; k < end; k++){
                min_r = std::min(min_r, pixels[k].r);
                max_r = std::max(max_r, pixels[k].r);
                min_g = std::min(min_g, pixels[k].g);
                max_g = std::max(max_g, pixels[k].g);
                min_b = std::min(min_b, pixels[k].b);
                max_b = std::max(max_b, pixels[k].b);
            }

            int r_range = max_r - min_r;
            int g_range = max_g - min_g;
            int b_range = max_b - min_b;

            int current_max = std::max({
                r_range, 
                g_range, 
                b_range
            });

            if(current_max > max_range){
                max_range = current_max;
                split_index = i;

                channel_to_sort = (current_max == r_range) ? 0
                : (current_max == g_range) ? 1
                : 2;
            }
        }

        if(split_index == -1) break;

        Box &box_to_split = boxes[split_index];
        auto start = pixels.begin() + box_to_split.start_index;
        auto end = pixels.begin() + box_to_split.end_index;

        if(channel_to_sort == 0){
            std::sort(start, end, compareRed);
        }else if (channel_to_sort == 1){
            std::sort(start, end, compareGreen);
        }else{
            std::sort(start, end, compareBlue);
        }

        int mid = box_to_split.start_index + (box_to_split.end_index - box_to_split.start_index)/2;
        Box new_box = {mid, box_to_split.end_index};
        box_to_split.end_index = mid;
        boxes.push_back(new_box);
    }

    
    py::array_t<uint8_t> result ({H, W, 3});
    auto rbuff = result.request();
    uint8_t* out_ptr = static_cast<uint8_t*>(rbuff.ptr);

    for(const auto &box: boxes){
        uint64_t sum_r = 0;
        uint64_t sum_g = 0;
        uint64_t sum_b = 0;

        int box_size = box.end_index - box.start_index;  
        if(box_size == 0) continue;

        for(int i = box.start_index; i < box.end_index; i++){
            sum_r += pixels[i].r;
            sum_g += pixels[i].g;
            sum_b += pixels[i].b;
        }

        uint8_t avg_r = static_cast<uint8_t>(sum_r / box_size);
        uint8_t avg_g = static_cast<uint8_t>(sum_g / box_size);
        uint8_t avg_b = static_cast<uint8_t>(sum_b / box_size);
        
        for(int i = box.start_index; i < box.end_index; i++){
            int original_index = pixels[i].original_index;

            out_ptr[original_index * 3] = avg_r;
            out_ptr[original_index * 3 + 1] = avg_g;
            out_ptr[original_index * 3 + 2] = avg_b;
        }
    }

    return result;
}

PYBIND11_MODULE(imageproc, m){
    m.def("medianCut", &medianCut);
}