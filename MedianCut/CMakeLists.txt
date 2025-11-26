cmake_minimum_required(VERSION 3.20)
project(MedianCut)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 3 REQUIRED)

set(MODULE_NAME img_proc)
pybind11_add_module(${MODULE_NAME} main.cpp)