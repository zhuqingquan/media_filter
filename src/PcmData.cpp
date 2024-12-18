#include "PcmData.h"
#include <assert.h>
#include <algorithm>
#include <cstring>

using namespace zMedia;
PcmData::PcmData( uint32_t channels, uint32_t sampleRate, AudioSampelTypeSize sampleType)
    : m_PerSampleByteCount(sampleType)
    , m_nSampleRate(sampleRate)
    , m_nChannels(channels)
    , m_nTimeStamp(0), m_nTimeCount(0)
    , m_capacity(0)
{
}

PcmData::~PcmData()
{
    this->free();
}

size_t PcmData::malloc_timecount(uint32_t timecount, MemoryAllocator allocator /*= MemoryAllocator()*/)
{
    size_t capacity = m_nSampleRate * m_nChannels * m_PerSampleByteCount * ((float)timecount / 1000);
    if(capacity<=0)
    {
        assert(false);
    }
    if(capacity>m_buf.malloc(capacity, allocator))
    {
        m_buf.free();
        return 0;
    }
    m_nTimeCount = timecount;
    m_capacity = capacity;
    return capacity;
}

size_t PcmData::malloc_samplecount(uint32_t sampleCountPerChannel, MemoryAllocator allocator /*= MemoryAllocator()*/)
{
    size_t capacity = sampleCountPerChannel * m_nChannels * m_PerSampleByteCount;
    if(capacity<=0)
    {
        assert(false);
    }
    if(capacity>m_buf.malloc(capacity, allocator))
    {
        m_buf.free();
        return 0;
    }
    m_nTimeCount = ((float)sampleCountPerChannel / m_nSampleRate) * 1000;
    m_nTimeCount = m_nTimeCount > 0 ? m_nTimeCount : 1;
    m_capacity = capacity;
    return capacity;
}

size_t PcmData::free( )
{
    size_t ret = m_buf.free();
    m_nTimeCount = 0;
    m_capacity = 0;
    return ret;
}

size_t PcmData::appendData( const uint8_t* data, size_t bytesCount )
{
    assert(bytesCount % m_PerSampleByteCount == 0);
    if(m_nTimeCount==0 || m_capacity==0)
        return 0;
    size_t bytesAligned = bytesCount - (bytesCount % m_PerSampleByteCount);
    if(bytesAligned==0 || data==NULL)
        return 0;

    size_t writeSize = std::min(bytesAligned, freeSize());

    if (writeSize > 0)
    {
        uint8_t* dst = m_buf.data() + m_buf.getPayloadOffset() + m_buf.getPayloadSize();
        memcpy( dst, data, writeSize);
        m_buf.setPayloadSize(m_buf.getPayloadSize()+writeSize);
    }
    return writeSize;
}

bool PcmData::isSilence() {
    if (size() == 0)
        return true;
    uint8_t *ptr = (uint8_t *)data();
    if (*ptr == 0 && memcmp(ptr, ptr + 1, size() - 1) == 0)
      return true;
    return false;
}

PcmData::SPtr zMedia::copyPcmFrame(const PcmData::SPtr& src)
{
    PcmData::SPtr result = std::make_shared<zMedia::PcmData>(src->channels(), src->sampleRate(), src->sampleSize());
    result->malloc_samplecount(src->sampleCountPerChannel());
    result->appendData(src->data(), src->capacity());
    return result;
}


