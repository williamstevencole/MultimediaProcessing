cmake_minimum_required(VERSION 3.20)
project(dither4x4_module LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)

set(MODULE_NAME ordered_dithering)
#pybind11_add_module(${MODULE_NAME} main.cpp)
pybind11_add_module(${MODULE_NAME} ptrmain.cpp)
