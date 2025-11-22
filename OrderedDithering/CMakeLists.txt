cmake_minimum_required(VERSION 3.10)
project(dither4x4_module LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)

pybind11_add_module(ordered_dithering main.cpp)
