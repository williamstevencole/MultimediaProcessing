#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace py = pybind11;

std::vector<std::vector<uint8_t>> make_bayer_matrix(int size){
    if (size<=1){
        throw std::runtime_error("Size must be at least 2");
    }

    if((size & (size -1))!=0){
        throw std::runtime_error("Size must be a power of 2");
    }


    if(size ==2){
        return {{0,1}, {2,3}};
    }

    int half = size/2;
    auto halfBayer = make_bayer_matrix(half);

    std::vector<std::vector<uint8_t>> newBayer(size, std::vector<uint8_t>(size));

    for(int row = 0; row < half; row++){
        for(int col = 0; col < half; col++){    
            int val = halfBayer[row][col];
            newBayer[row][col] = 4*val;
            newBayer[row][col+half] = 4*val + 2;
            newBayer[row+half][col] = 4*val + 1;
            newBayer[row+half][col+half] = 4*val + 3;
        }
    }

    return newBayer;
}

py::array_t<uint8_t> order_dither(py::array_t<uint8_t> img, int bayersize){
    if(img.ndim() != 2){
        throw std::runtime_error("Image must be of 2 dimensions");
    }

    if(bayersize < 2 || (bayersize & (bayersize -1)) != 0){
        throw std::runtime_error("Bayer size must be at least 2 and a power of 2");
    }

    auto buff = img.request();
    uint8_t* ptr = static_cast<uint8_t*>(buff.ptr);

    ssize_t H = buff.shape[0];
    ssize_t W = buff.shape[1];

    py::array_t<uint8_t> newImg ({H,W});
    auto nbuff = newImg.request();
    uint8_t* nptr = static_cast<uint8_t*>(nbuff.ptr);

    int scale_factor = 256/(bayersize * bayersize);
    auto bayer = make_bayer_matrix(bayersize);

    for(ssize_t row = 0; row < H; row++){
        for(ssize_t col = 0; col < W; col++){
            uint8_t pixel = ptr[row*W + col];

            int bayerval = bayer[row%bayersize][col%bayersize];
            int threshold = bayerval * scale_factor;

            uint8_t newPixel = (pixel > threshold) ? 1 : 0;

            nptr[row*W + col] = newPixel;
        }
    }

    return newImg;
}

py::array_t<uint8_t> binary_to_grayscale(py::array_t<uint8_t> img){
    if(img.ndim() != 2){
        throw std::runtime_error("Image must be of 2 dimensions");
    }

    auto buff = img.request();
    uint8_t* ptr = static_cast<uint8_t*>(buff.ptr);

    ssize_t H = buff.shape[0];
    ssize_t W = buff.shape[1];

    py::array_t<uint8_t> newImg ({H,W});
    auto nbuff = newImg.request();
    uint8_t* nptr = static_cast<uint8_t*>(nbuff.ptr);

    for(ssize_t row = 0; row < H; row++){
        for(ssize_t col = 0; col < W; col++){
            nptr[row*W + col] = (ptr[row*W + col] == 0) ? 0 : 255;
        }
    }

    return newImg;
}

PYBIND11_MODULE(ordered_dithering, m){
    m.def("make_bayer_matrix", &make_bayer_matrix, py::arg("n"));
    m.def("order_dither", &order_dither, py::arg("img"), py::arg("bayersize"));
    m.def("binary_to_grayscale", &binary_to_grayscale, py::arg("img"));
}