#CMakeList to make media_filter lib

cmake_minimum_required(VERSION 2.6)
project(media_filter)
#The version number
set(media_filter_VERSION_MAJOR 1)
set(media_filter_VERSION_MINOR 0)

#configure a header file to pass some of the CMake settings
#to the source code
configure_file(
    "${PROJECT_SOURCE_DIR}/media_filter_cfg.h.in"
    "${PROJECT_BINARY_DIR}/media_filter_cfg.h"
    )

set(SRC_DIR ../src)
set(INC_DIR ../inc)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${INC_DIR}")

set(SRC_LIST ${SRC_DIR}/MediaData.cpp ${SRC_DIR}/MemoryAllocator.cpp)

add_library(media_filter STATIC ${SRC_LIST})