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

TEST(PcmData, malloc_timecount_free)
{
    zMedia::PcmData pd(2, 44100, zMedia::AudioSampleSize_SHORT);
    size_t timecount = 40;
    size_t capacity = ((float)timecount / 1000) * 44100 * 2 * zMedia::AudioSampleSize_SHORT;
    EXPECT_GE(capacity, pd.malloc_timecount(timecount));
    EXPECT_GE(pd.capacity(), capacity);
    EXPECT_GE(pd.freeSize(), capacity);
    EXPECT_TRUE(NULL!=pd.data());
    EXPECT_EQ(40, pd.getTimeCount());
    EXPECT_EQ(0, pd.size());
    EXPECT_TRUE(pd.empty());
    EXPECT_EQ(0, pd.sampleCount());
    EXPECT_EQ(0, pd.sampleCountPerChannel());

    //free
    EXPECT_TRUE(pd.free());
    EXPECT_EQ(0, pd.capacity());
    EXPECT_EQ(0, pd.freeSize());
    EXPECT_TRUE(NULL==pd.data());
}

TEST(PcmData, malloc_framecount_free)
{
    zMedia::PcmData pd(2, 44100, zMedia::AudioSampleSize_SHORT);
    size_t framecount = 1024;
    size_t capacity = framecount * 2 * zMedia::AudioSampleSize_SHORT;
    EXPECT_GE(capacity, pd.malloc_samplecount(framecount));
    EXPECT_GE(pd.capacity(), capacity);
    EXPECT_GE(pd.freeSize(), capacity);
    EXPECT_TRUE(NULL!=pd.data());
    EXPECT_EQ(0, pd.size());
    EXPECT_TRUE(pd.empty());
    EXPECT_EQ(0, pd.sampleCount());
    EXPECT_EQ(0, pd.sampleCountPerChannel());

    //free
    EXPECT_TRUE(pd.free());
    EXPECT_EQ(0, pd.capacity());
    EXPECT_EQ(0, pd.freeSize());
    EXPECT_TRUE(NULL==pd.data());
}

TEST(PcmData, appendData)
{
    zMedia::PcmData pd(2, 44100, zMedia::AudioSampleSize_SHORT);
    size_t framecount = 1024;
    size_t capacity = framecount * 2 * zMedia::AudioSampleSize_SHORT;
    EXPECT_GE(capacity, pd.malloc_samplecount(framecount));
    size_t twoSampleBytesCount = 2 * 2 * zMedia::AudioSampleSize_SHORT;
    BYTE* pTwoSample = (BYTE*)malloc(twoSampleBytesCount);
    for (size_t i = 0; i < framecount/2; i++) {
        EXPECT_EQ(twoSampleBytesCount, pd.appendData(pTwoSample, twoSampleBytesCount));
        EXPECT_FALSE(pd.empty());
        EXPECT_EQ((i+1)*twoSampleBytesCount, pd.size());
        EXPECT_EQ(capacity-(i+1)*twoSampleBytesCount, pd.freeSize());
    }
    delete pTwoSample;
    EXPECT_EQ(1024*2, pd.sampleCount());
    EXPECT_EQ(1024, pd.sampleCountPerChannel());
    EXPECT_TRUE(pd.full());
    //clear
    pd.clear();
    EXPECT_TRUE(pd.empty());
    EXPECT_FALSE(pd.full());
    EXPECT_EQ(capacity, pd.freeSize());
    EXPECT_EQ(0, pd.sampleCount());
    EXPECT_EQ(0, pd.sampleCountPerChannel());
    EXPECT_FALSE(pd.full());

    //append data more
    size_t dataSize = capacity * 2;
    BYTE* bigBuf = (BYTE*)malloc(dataSize);
    EXPECT_EQ(capacity, pd.appendData(bigBuf, dataSize));
    EXPECT_EQ(1024*2, pd.sampleCount());
    EXPECT_EQ(1024, pd.sampleCountPerChannel());
    EXPECT_TRUE(pd.full());
    //free
    EXPECT_TRUE(pd.free());
    EXPECT_EQ(0, pd.capacity());
    EXPECT_EQ(0, pd.freeSize());
    EXPECT_TRUE(NULL==pd.data());
}
