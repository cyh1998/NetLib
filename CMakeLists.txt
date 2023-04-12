cmake_minimum_required(VERSION 3.10)
project(NetLib)

set(CMAKE_CXX_STANDARD 14)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# set(Protobuf_PREFIX_PATH "/usr/local/protobuf" )
# list(APPEND CMAKE_PREFIX_PATH "${Protobuf_PREFIX_PATH}")
find_package(Protobuf REQUIRED)
find_package(Threads)

#添加子目录
add_subdirectory(src)

add_executable(NetLib main.cpp)

#添加链接库
target_link_libraries(NetLib base)
