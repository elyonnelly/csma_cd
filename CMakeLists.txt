cmake_minimum_required(VERSION 3.14)
project(csmacd)

set(CMAKE_CXX_STANDARD 14)

add_executable(csmacd main.cpp CSMACD.cpp CSMACD.h)