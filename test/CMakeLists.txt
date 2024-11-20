cmake_minimum_required(VERSION 3.0)

project(media_filter_unittest)
enable_testing()

set(SRC_FILE_TESTED 
    ../inc/AudioData.h 
    ../src/PcmData.cpp
    ../src/PcmDataHelper.cpp
    ../src/MediaBuffer.cpp
    ../src/MemoryAllocator.cpp
    ../src/RawPicture.cpp
    ../src/VideoFrameQueue.cpp
    )

include_directories(../inc)
add_executable(mediafilter_unittest 
    ${SRC_FILE_TESTED}
    AudioData_unittest.cpp 
    VideoFrameQueu_unittest.cpp
)

target_link_libraries(mediafilter_unittest gtest_main)

include(GoogleTest)
gtest_discover_tests(mediafilter_unittest)
