cmake_minimum_required(VERSION 3.17)
project(backend)

set(CMAKE_CXX_STANDARD 20)
add_executable(backend_test main.cpp DatabaseHelper.cpp DatabaseHelper.h)
add_library(backend library.cpp library.h DatabaseHelper.cpp DatabaseHelper.h)