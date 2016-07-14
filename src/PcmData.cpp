#include "PcmData.h"
#include <assert.h>
#include <algorithm>

using namespace zMedia;

	PcmData::PcmData()
		: m_PerSampleByteCount(AudioSampleSize_NONE)
		, m_nSampleRate(0)
		, m_nChannels(0)
		, m_nVolume(0), m_bIsMuted(false)
		, m_nDataSize(0)
		, m_nTimeStamp(0), m_nTimeCount(0)
	{
	}

	PcmData::PcmData( UINT channels, UINT sampleRate, AudioSampelTypeSize sampleType, UINT timecount, 
						const MemoryAllocator& allocator/* = MemoryAllocator()*/)
		: m_PerSampleByteCount(sampleType)
		, m_nSampleRate(sampleRate)
		, m_nChannels(channels)
		, m_nVolume(0), m_bIsMuted(false)
		, m_nDataSize(0)
		, m_nTimeStamp(0), m_nTimeCount(0)
	{
		UINT capacity = m_nSampleRate * m_nChannels * m_PerSampleByteCount * timecount / 1000;
		//assert(capacity>0);
		if(capacity<=0)
		{
			assert(false);
		}
		m_buf.malloc(capacity, allocator);
		m_nTimeCount = timecount;
	}

	PcmData::~PcmData()
	{
        this->free();
	}

    size_t PcmData::allocBuffer(size_t capacity, MemoryAllocator allocator /*= MemoryAllocator()*/)
 	{
        m_nDataSize = capacity;
        return m_buf.malloc(capacity, allocator);
 	}

	size_t PcmData::free( )
	{
        size_t ret = m_buf.free();
		if(ret>0)
		{
			m_nDataSize = 0;
			m_nTimeCount = 0;
		}
        return ret;
	}

	void PcmData::setData( const BYTE* data, UINT bytesCount)
	{
		if(data==NULL || bytesCount<=0)
			return;

		assert(bytesCount <= capacity());
		assert(bytesCount % sampleSize_Channel() == 0);

        BYTE* dst = m_buf.data();
		memcpy(dst, data, bytesCount);
		m_nDataSize = bytesCount;
	}

	UINT PcmData::appendData( const BYTE* data, UINT bytesCount )
	{
		assert(bytesCount % sizeof(float) == 0);
		assert(capacity() >= m_nDataSize);

		UINT emptySize = capacity() - m_nDataSize;
		UINT writeSize = std::min(bytesCount, emptySize);


		if (writeSize > 0)
		{
            BYTE* dst = m_buf.data();
			memcpy( dst + m_nDataSize, data, writeSize);
			m_nDataSize += writeSize;
		}
		return writeSize;
	}
