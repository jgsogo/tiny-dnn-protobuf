
cmake_minimum_required(VERSION 3.1)

add_library(tiny_dnn_c c_api.cpp c_api.h protobuf_serialization.hpp)
target_link_libraries(tiny_dnn_c PUBLIC messages CONAN_PKG::tiny-dnn)
generate_export_header(tiny_dnn_c EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/tiny_dnn_c_api.h)
target_include_directories(tiny_dnn_c
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>/src
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

