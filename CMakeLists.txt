cmake_minimum_required(VERSION 3.23)
project(QEC)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)
include_directories("/tools/Xilinx/Vitis_HLS/2023.2/include")
include_directories("F:/Xilinx/Vitis_HLS/2023.1/include")


add_executable(QEC testBench/TestBench.cpp src/ProjectC/FRATC.cpp src/ProjectC/FRATC.h)
