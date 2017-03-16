#CMakeLists for media_filter unittest
cmake_minimum_required(VERSION 2.6)
PROJECt(media_filter_unittest)

set(SRC_LIST 
    ./MediaData_test.cpp 
    ./PictureInfo_test.cpp)

set(LIB_PATH_LIST 
    ../googletest/lib 
    ../build)

FIND_LIBRARY(GTEST_MAIN_LIB NAMES gtest_main HINTS ${LIB_PATH_LIST})
MESSAGE(STATUS ${GTEST_MAIN_LIB})
IF (NOT GTEST_MAIN_LIB)
    MESSAGE(FATAL_ERROR "gtest_main lib not found")
ENDIF(NOT GTEST_MAIN_LIB)

#set(MEDIA_FILTER_LIBS ${MEDIA_FILTER_LIBS} ${GTEST_MAIN_LIB})
#MESSAGE(STATUS ${MEDIA_FILTER_LIBS})

FIND_LIBRARY(GTEST_LIB NAMES gtest HINTS ../googletest/lib)
MESSAGE(STATUS ${GTEST_LIB})
IF (NOT GTEST_LIB)
    MESSAGE(FATAL_ERROR "gtest lib not found")
ENDIF(NOT GTEST_LIB)
#set(MEDIA_FILTER_LIBS ${MEDIA_FILTER_LIBS} ${GTEST_LIB})
#MESSAGE(STATUS ${MEDIA_FILTER_LIBS})

FIND_LIBRARY(MEDIA_FILTER_LIB NAMES media_filter HINTS ${LIB_PATH_LIST})
MESSAGE(STATUS ${MEDIA_FILTER_LIB})
IF (NOT MEDIA_FILTER_LIB)
    MESSAGE(FATAL_ERROR "media_filter lib not found")
ENDIF(NOT MEDIA_FILTER_LIB)

set(LINKED_LIBS 
    ${MEDIA_FILTER_LIBS} 
    ${GTEST_LIB} 
    ${GTEST_MAIN_LIB} 
    ${MEDIA_FILTER_LIB}
    boost_system 
    boost_thread)

include_directories("../googletest/include" 
    "../inc")

add_executable(media_filter_unittest ${SRC_LIST})

TARGET_LINK_LIBRARIES(media_filter_unittest ${LINKED_LIBS} pthread)
