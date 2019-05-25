cmake_minimum_required(VERSION 3.9)
project(egypt_opengl_scene)

set(CMAKE_CXX_STANDARD 11)

add_executable(egypt_opengl_scene Main.cpp)

target_link_libraries(egypt_opengl_scene libopengl32.a libglu32.a libfreeglut.a)
