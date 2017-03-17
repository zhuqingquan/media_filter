#include "gtest/gtest.h"
#include "PcmData.h"

TEST(SampleSize, enum_value)
{
    EXPECT_EQ(2, zMedia::AudioSampleSize_SHORT);
    EXPECT_EQ(0, zMedia::AudioSampleSize_NONE);
    EXPECT_EQ(1, zMedia::AudioSampleSize_BYTE);
    EXPECT_EQ(4, zMedia::AudioSampleSize_FLOAT);
}

TEST(PcmData, Constructor)
{
    //zMedia::PcmData pd;//default constructor is invalid
    zMedia::PcmData pd(2, 44100, zMedia::AudioSampleSize_SHORT);
    EXPECT_EQ(2, pd.channels());
    EXPECT_EQ(44100, pd.sampleRate());
    EXPECT_EQ(zMedia::AudioSampleSize_SHORT, pd.sampleSize());
    EXPECT_EQ(2*zMedia::AudioSampleSize_SHORT, pd.sampleSizeAllChannels());
}

TEST(PcmData, malloc_free)
{
    zMedia::PcmData pd(2, 44100, zMedia::AudioSampleSize_SHORT);
    size_t timecount = 40;
    size_t capacity = ((float)timecount / 1000) * 44100 * 2 * zMedia::AudioSampleSize_SHORT;
    EXPECT_GE(capacity, pd.malloc_timecount(timecount));
}
