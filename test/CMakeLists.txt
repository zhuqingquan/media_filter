cmake_minimum_required(VERSION 3.0)

project(media_filter_unittest)

# 先fetch googletest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
