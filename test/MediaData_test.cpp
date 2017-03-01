//#pragma once
#include "gtest/gtest.h"
#include "MediaData.h"

TEST(MediaData_test, constructor)
{
    zMedia::MediaBuffer buf;
    EXPECT_EQ(NULL, buf.data());
}

