# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目信息
project (chftek)
# 指定生成目标
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./output/)

#aux_source_directory(./src DIR_SRCS)
add_executable(main ./src/main.cpp)
add_executable(object_buffer_ut ./src/thread/object_buffer_ut.cpp)
add_definitions(-std=c++11)
target_link_libraries(object_buffer_ut /usr/local/lib/libgtest.a)

