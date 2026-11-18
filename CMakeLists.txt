cmake_minimum_required(VERSION 3.20)
project(sphere_project VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sphere
sphere.cpp geometry.cpp)

set_property(TARGET sphere PROPERTY CXX_STANDARD 23)