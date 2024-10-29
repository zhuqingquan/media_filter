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

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INC_DIR ${PROJECT_SOURCE_DIR}/inc)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${INC_DIR}")
include_directories(../zMemoryPool/include)

file(GLOB_RECURSE INC_LIST ${INC_DIR}/*.h)

set(SRC_LIST 
    ${SRC_DIR}/MediaBuffer.cpp 
    ${SRC_DIR}/MemoryAllocator.cpp
    ${SRC_DIR}/RawPicture.cpp
    ${SRC_DIR}/PictureInfo.cpp
    ${SRC_DIR}/PcmData.cpp
    ${SRC_DIR}/PcmDataHelper.cpp
)

add_library(media_filter STATIC ${SRC_LIST} ${INC_LIST})

source_group("Inc" FILES ${INC_LIST})
source_group("Src" FILES ${SRC_LIST})

install (TARGETS media_filter DESTINATION lib)
install (FILES 
    ${INC_DIR}/MediaBuffer.h 
    ${INC_DIR}/TextureHolder.h
    ${INC_DIR}/MemoryAllocator.h 
    ${INC_DIR}/AudioData.h 
    ${INC_DIR}/CodecMediaData.h 
    ${INC_DIR}/fourcc.h 
    ${INC_DIR}/mediadata_tpl.h 
    ${INC_DIR}/PcmData.h 
    ${INC_DIR}/PictureInfo.h 
    ${INC_DIR}/VideoData.h 
    DESTINATION include)
