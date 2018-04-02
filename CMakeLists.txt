cmake_minimum_required(VERSION 3.9)
project(cosc363_assignment)

set(CMAKE_CXX_STANDARD 98)

add_executable(cosc363_assignment Main.cpp)

target_link_libraries(cosc363_assignment libopengl32.a libglu32.a libfreeglut.a)
