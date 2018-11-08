cmake_minimum_required(VERSION 3.1)

include(GenerateExportHeader)

project(TinyDNNProtobuf CXX)
set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Protobuf 3.6.1 REQUIRED)

add_subdirectory(messages)
add_subdirectory(src)
add_subdirectory(examples)
enable_testing()
