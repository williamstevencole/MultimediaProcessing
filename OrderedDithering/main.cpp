#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace py = pybind11;

std::vector<std::vector<int>> make_bayer_matrix(int n){
  if (n < 2){
    throw std::runtime_error("Matrix must be of at least size 2");
  }

  if((n & (n-1)) != 0){
    throw std::runtime_error("Matrix size must be power of 2");
  }

  if(n == 2){
    return std::vector<std::vector<int>> {{0,2},{1,3}};
  }

  int half = n/2;
  auto bayerHalf = make_bayer_matrix(half);

  std::vector<std::vector<int>> newBayer(n, std::vector<int>(n));

  for(int row = 0; row < half; row++){
    for(int col = 0; col < half; col++){
      int val = bayerHalf[row][col];
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
    throw std::runtime_error("Array must be of size 2");
  }

  if(bayersize < 2){
    throw std::runtime_error("Bayer matrix size must be at least 2");
  }

  if((bayersize & (bayersize -1)) != 0){
    throw std::runtime_error("Bayer matrix size must be of power of 2");
  }

  auto buff = img.unchecked<2>();

  ssize_t H = buff.shape(0);
  ssize_t W = buff.shape(1);

  py::array_t<uint8_t> newImg ({H,W});
  auto nbuff = newImg.mutable_unchecked<2>();

  int scale_factor = 256/(bayersize * bayersize);
  auto bayer = make_bayer_matrix(bayersize);

  for(ssize_t row = 0; row < H; row++){
    for(ssize_t col = 0; col< W; col++){
      uint8_t pixel = buff(row, col);

      int bayerval = bayer[row%bayersize][col%bayersize];
      int threshold = bayerval * scale_factor;

      uint8_t newPixel = (pixel > threshold) ? 1 : 0;

      nbuff(row, col) = newPixel;

    }
  }
  return newImg;
}

py::array_t<uint8_t> binary_to_grayscale(py::array_t<uint8_t> img){
  if(img.ndim() != 2){
    throw std::runtime_error("Array must be of size 2");
  }

  auto buff = img.unchecked<2>();

  ssize_t H = buff.shape(0);
  ssize_t W = buff.shape(1);

  py::array_t<uint8_t> newImg ({H,W});
  auto nbuff = newImg.mutable_unchecked<2>();

  for(ssize_t row = 0; row < H; row++){
    for(ssize_t col = 0; col < W; col++){
      nbuff(row, col) = (buff(row,col) == 0) ? 0 : 255;
    }
  }

  return newImg;
}


PYBIND11_MODULE(ordered_dithering, m){
  m.def("make_bayer_matrix", &make_bayer_matrix, py::arg("n"));
  m.def("order_dither", &order_dither, py::arg("img"), py::arg("bayersize"));
  m.def("binary_to_grayscale", &binary_to_grayscale, py::arg("img"));
}