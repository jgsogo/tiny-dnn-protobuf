cmake_minimum_required(VERSION 3.1)

# Generate protobuffers
file(GLOB PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

source_group("Include Files" FILES ${PROTO_HDRS})
add_library(messages ${PROTO_SRCS} ${PROTO_HDRS} ${PROTO_FILES})
target_include_directories(messages
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
        ${Protobuf_INCLUDE_DIRS})

target_link_libraries(messages PUBLIC protobuf::libprotobuf)
