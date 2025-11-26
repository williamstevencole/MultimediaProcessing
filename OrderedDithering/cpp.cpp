#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <vector>
#include <cstdint>
#include <stdexcept>

namespace py = pybind11;

std::vector<std::vector<int>> makeBayerMatrix(int size){
    if(size < 2) throw std::out_of_range("size must be at least 2");
    

    if ((size & (size - 1)) != 0 ) throw std::runtime_error("Matrix size must be of power of two");

    if(size == 2){
        return {{0,2}, {1,3}};
    }

    int half = size / 2;
    auto halfBayer = makeBayerMatrix(half);

    std::vector<std::vector<int>> newBayer(size, std::vector<int>(size));

    for(int row = 0; row < half; row++){
        for(int col = 0; col < half; col++){
            int val = halfBayer[row][col];
            newBayer[row][col] = 4 * val;
            newBayer[row][col + half] = 4 * val + 2;
            newBayer[row + half][col] = 4 * val + 1;
            newBayer[row + half][col + half] = 4 * val + 3;
        }
    }

    return newBayer;
}

py::array_t<uint8_t> ordered_dithering(py::array_t<uint8_t> img, int bayerSize){
    if(img.ndim() != 2) throw std::runtime_error("Img must be of 2 dimensions");

    auto buff = img.request();
    uint8_t* ptr = static_cast<uint8_t*>(buff.ptr);

    ssize_t H = buff.shape[0];
    ssize_t W = buff.shape[1];

    py::array_t<uint8_t> newImg ({H, W});
    auto nbuff = newImg.request();
    uint8_t *nptr = static_cast<uint8_t*>(nbuff.ptr);

    auto bayerMatrix = makeBayerMatrix(bayerSize);
    int scaleSize = 256 / (bayerSize * bayerSize);

    for(ssize_t row = 0; row < H; row++){
        for(ssize_t col = 0; col < W; col++){
            int bayerVal = bayerMatrix[row % bayerSize][col % bayerSize];
            uint8_t pixel = ptr[row*W + col];

            nptr[row*W + col] = (pixel > bayerVal*scaleSize) ? 1 : 0;
        }
    }

    return newImg;

}

PYBIND11_MODULE(multimediaprocessing, m){
    m.def("ordered_dithering", &ordered_dithering);

    m.def("makeBayerMatrix", &makeBayerMatrix);
}