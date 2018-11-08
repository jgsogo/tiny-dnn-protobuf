cmake_minimum_required(VERSION 3.1)

add_executable(example_cpp main.cpp)
target_link_libraries(example_cpp tiny_dnn_c)
set_property(TARGET example_cpp PROPERTY CXX_STANDARD 17)
