cmake_minimum_required(VERSION 3.10)
project(NetLib)

set(CMAKE_CXX_STANDARD 14)

#添加子目录
add_subdirectory(src)

add_executable(NetLib main.cpp)

#添加链接库
target_link_libraries(NetLib base)
