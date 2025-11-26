#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <cmath>
#include <cstdint>

namespace py = pybind11;

struct Color{
    uint8_t r,g,b;
    int original_index;
};

struct Box{
    int start_index;
    int end_index;
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

py::array_t<uint8_t> medianCut
(
py::array_t<uint8_t> input_image, 
int num_colors
){
    auto buff = input_image.request();

    if(buff.ndim != 3) throw std::runtime_error("Image must have three dimensions for RGB channels");

    int height = buff.shape[0];
    int width = buff.shape[1];
    int channels = buff.shape[2];

    int imageSize = height*width;

    uint8_t* ptr = static_cast<uint8_t*>(buff.ptr);

    std::vector<Color> pixels;
    pixels.reserve(imageSize); 

    for(int i = 0; i < (imageSize); i++){
        pixels.push_back({
            ptr[i * channels], // red
            ptr[i * channels + 1], // green
            ptr[i * channels + 2], // blue
            i // original index of pixel in image
        });
    }

    std::vector<Box> boxes;
    boxes.push_back({0, static_cast<int>(pixels.size())}); // full box

    while(boxes.size() < num_colors){
        int split_index = -1;
        int max_range = -1;
        int channel_to_sort = -1; // 0:R , 1:G, 2:B

        for(int i = 0; i < boxes.size(); i++){
            int start = boxes[i].start_index;
            int end = boxes[i].end_index;

            if(end-start <= 1) continue;

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

            int current_max = std::max({r_range, g_range, b_range});

            if(current_max > max_range){
                max_range = current_max;
                split_index = i;

                channel_to_sort = (current_max == r_range) ? 0 
                : (current_max == g_range) ?  1
                : 2;
            }
        }

        // no more boxes to split
        if( split_index == -1) break;

    }



    
}




PYBIND11_MODULE(img_proc, m){
    m.def("medianCut", &medianCut);
}