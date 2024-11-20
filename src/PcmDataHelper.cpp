#include "PcmDataHelper.h"
#include <cstring>
#include <vector>
#include <math.h>

#ifndef DWORD
typedef uint32_t DWORD;
#endif

#define AudioSample float 

using namespace zMedia;
using namespace std;

	inline bool MachineIsBigEndian()
	{
		return false;
	}

	inline bool MachineIsLittleEndian()
	{
		return !MachineIsBigEndian();
	}

	inline void SwapOrder(void* p, unsigned nBytes)
	{
		uint8_t* ptr = (uint8_t*)p;
		uint8_t t;
		unsigned n;
		for(n = 0; n < nBytes>>1; n++)
		{
			t = ptr[n];
			ptr[n] = ptr[nBytes - n - 1];
			ptr[nBytes - n - 1] = t;
		}
	}


	template<class T, bool b_swap, bool b_signed, bool b_pad> class sucks_v2 
	{
	public:
		inline static void DoFixedpointConvert(const void* source, unsigned bps, unsigned count, AudioSample* buffer)
		{
			const char * src = (const char *) source;
			unsigned bytes = bps>>3;
			unsigned n;
			T max = ((T)1)<<(bps-1);

			T negmask = - max;

			double div = 1.0 / (double)(1<<(bps-1));
			for(n = 0; n < count; n++)
			{
				T temp;
				if (b_pad)
				{
					temp = 0;
					memcpy(&temp, src, bytes);
				}
				else
				{
					temp = *reinterpret_cast<const T*>(src);
				}

				if (b_swap) 
					SwapOrder(&temp,bytes);

				if (!b_signed) temp ^= max;

				if (b_pad)
				{
					if (temp & max) temp |= negmask;
				}

				if (b_pad)
					src += bytes;
				else
					src += sizeof(T);

				buffer[n] = (AudioSample)((double)temp * div);
			}
		}
	};

	template <class T,bool b_pad> class sucks 
	{ 
	public:
		inline static void DoFixedpointConvert(bool b_swap, bool b_signed, const void* source, unsigned bps,
			unsigned count, AudioSample* buffer)
		{
			if (sizeof(T) == 1)
			{
				if (b_signed)
				{
					sucks_v2<T, false, true, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
				}
				else
				{
					sucks_v2<T, false, false, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
				}
			}
			else if (b_swap)
			{
				if (b_signed)
				{
					sucks_v2<T, true, true, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
				}
				else
				{
					sucks_v2<T, true, false, b_pad>::DoFixedpointConvert(source, bps, count, buffer);
				}
			}
			else
			{
				if (b_signed)
				{
					sucks_v2<T, false, true, b_pad>::DoFixedpointConvert(source,bps,count,buffer);
				}
				else
				{
					sucks_v2<T, false, false, b_pad>::DoFixedpointConvert(source,bps,count,buffer);
				}
			}
		}
	};

    void zMedia::scaleVolume(float* data, int sampleCount, double volFactor)
    {
        if (data == nullptr)
            return;
        for (int i=0; i<sampleCount; i++)
        {
            data[i] = data[i] * volFactor;
        }
    }

    void zMedia::scaleVolume(short* data, int sampleCount, double volFactor)
    {
        if (data == nullptr)
            return;
        for (int i = 0; i < sampleCount; i++)
        {
            int64_t m = (int64_t)(data[i] * volFactor);
            m = m > 32767 ? 32767 : ((m < -32767) ? -32767 : m);
            data[i] = (short)m;
        }
    }


	int zMedia::short2float( const short* shortData, int shortCount, float* floatBuffer, int floatCount )
	{
		if (shortData == nullptr || floatBuffer == nullptr || shortCount <= 0 || floatCount <= 0)
			return 0;
		int nBits = 16;
		bool bSigned = nBits > 8;
		bool bNeedSwap = false;

		//assert(floatCount >= shortCount);
		if (floatCount < shortCount)
			return 0;

		sucks<short, false>::DoFixedpointConvert(bNeedSwap,
			bSigned, shortData, nBits, shortCount, floatBuffer);

		return shortCount;
	}


	int zMedia::float2short(const float* floatData, int floatCount, short* dstShortBuffer, int shortBufferCount)
	{
		assert(shortBufferCount >= floatCount);

		const float* pSrc = floatData;
		short* pDst = dstShortBuffer;
		uint32_t processCount = floatCount;

		union {float f; DWORD i;} u;

		for (uint32_t i = 0; i < processCount; i++)
		{
			u.f = float(*pSrc + 384.0);
			if (u.i > 0x43c07fff) 
			{
				*pDst = 32767;
			}
			else if (u.i < 0x43bf8000) 
			{
				*pDst = -32768;
			}
			else 
			{
				*pDst = short(u.i - 0x43c00000);
			}
			pSrc++;
			pDst++;
		}

		return floatCount;
	}

	PcmData::SPtr zMedia::convertPcmData( const PcmData::SPtr& srcData, uint32_t dstChannels, uint32_t dstSampleRate, AudioSampelTypeSize dstSampleTypeSize )
	{
		if(srcData->empty())
			return PcmData::SPtr();
		//所有属性相同，不需转换
		if(srcData->sampleSize()==dstSampleTypeSize	&& dstChannels==srcData->channels() && dstSampleRate==srcData->sampleRate())
			return srcData;
		
		//只是处理short to float，其他的转换之后再添加
		if(srcData->sampleSize()==AudioSampleSize_SHORT && dstSampleTypeSize==AudioSampleSize_FLOAT
			&& dstChannels==srcData->channels() && dstSampleRate==srcData->sampleRate())
		{
			PcmData::SPtr  dst(new PcmData(dstChannels, dstSampleRate, dstSampleTypeSize));
			assert(dst);
			dst->malloc_timecount(srcData->getTimeCount());
			
			uint32_t dstCapacitySampleCoutn = dst->capacity() / dstSampleTypeSize;
			zMedia::short2float((short*)srcData->data(), srcData->sampleCount(), (float*)dst->data(), dstCapacitySampleCoutn);
			// dst->setWritePos(dst->capacity());
			return dst;
		}
		//其他情况暂时返回失败
		return PcmData::SPtr();
	}

	PcmData::SPtr zMedia::clonePcmData( const PcmData::SPtr& srcData )
	{
		if(srcData==nullptr)
			return PcmData::SPtr();
		PcmData::SPtr clonedPcmData(new PcmData(srcData->channels(), srcData->sampleRate(), srcData->sampleSize()));
		clonedPcmData->malloc_samplecount(srcData->sampleCount(), srcData->memAllocator());
		memcpy(clonedPcmData->data(), srcData->data(), srcData->size());
		// clonedPcmData->allocBuffer(srcData->capacity(), srcData->memAllocator());
		// clonedPcmData->setChannels(srcData->channels());
		// clonedPcmData->setSampleRate(srcData->sampleRate());
		// clonedPcmData->setSampleSize(srcData->sampleSize());
		// clonedPcmData->setVolume(srcData->volume());
		// clonedPcmData->setIsMuted(srcData->isMuted());
		clonedPcmData->setTimeStamp(srcData->getTimeStamp());
		// clonedPcmData->setData(srcData->data(), srcData->size());//copy the pcm data
		return clonedPcmData;
	}

	//bool zMedia::CopyPcmData(PcmData::SPtr dstData, const PcmData::SPtr& srcData, PcmData::SPtr& noCopy)
	//{
	//	if (dstData == nullptr || dstData->freeSize() <= 0 || srcData == nullptr || srcData->size() <= 0)
	//		return false;
	//	size_t freesize = dstData->freeSize();
	//	size_t payloadsize = srcData->size();
	//	int copyed = freesize <= payloadsize ? freesize : payloadsize;
	//	size_t writed = dstData->appendData(srcData->payload(), srcData->size());
	//	if (writed == srcData->size())
	//	{
	//		noCopy
	//		return true;
	//	}
	//	/*PcmData::SPtr */noCopy = std::make_shared<PcmData>(srcData->channels(), srcData->sampleRate(), srcData->sampleSize());
	//	size_t writedSample = writed / (srcData->sampleSizeAllChannels());
	//	noCopy->malloc_samplecount(srcData->sampleCountPerChannel()-writedSample);
	//	noCopy->appendData(srcData->payload() + writed, noCopy->freeSize());
	//	return noCopy;
	//}


	PcmDataConvert::PcmDataConvert()
	//:m_buffer(NULL)
	:m_dataSize(0)
	//, m_bufferSize(0)
	{

	}

	PcmDataConvert::~PcmDataConvert()
	{
		m_buf.free();
		m_dataSize = 0;
// 		if (m_buffer)
// 		{
// 			free(m_buffer);
// 			m_buffer = NULL;
// 			m_dataSize = 0;
// 		}
	}

	bool PcmDataConvert::float2short( const uint8_t* floatData, int bytesCount )
	{
		alloc(bytesCount/2);

		int floatSampleCount = bytesCount / sizeof(float);
		int shortSampleCount = m_buf.length() / sizeof(short);

		int samples = zMedia::float2short((const float*)floatData, floatSampleCount, (short*)m_buf.data(), shortSampleCount);
		m_dataSize = samples * sizeof(short);

		return samples == shortSampleCount;
	}

	bool PcmDataConvert::short2float( const uint8_t* shortData, int bytesCount )
	{
		clear();

		alloc(bytesCount * 2);

		int shortSampleCount = bytesCount / sizeof(short);
		int floatSampleCount = m_buf.length() / sizeof(float);

		int samples = zMedia::short2float((const short*)shortData, shortSampleCount, (float*)m_buf.data(), floatSampleCount);

		m_dataSize = samples * sizeof(float);

		return samples == shortSampleCount;
	}

	void PcmDataConvert::alloc( int size )
	{
        assert(size>0);
		assert(m_dataSize == 0);
		if(m_buf.length()<(size_t)size)
		{
			m_buf.malloc(size);
		}

// 		if (m_bufferSize < size)
// 		{
// 			if (m_buffer)
// 			{
// 				free(m_buffer);
// 			}
// 			m_buffer = (BYTE*)malloc(size);
// 			m_bufferSize = size;
// 		}
	}

	static const double DLT_MIN = -10 / 32767.0f;
	static const double DLT_MAX = 10 / 32767.0f;

	void zMedia::mixPcms(const std::vector<short*>& pcms, int samples, const std::vector<double>& volumeFactors, short* output, double masterVolumeFactor)
	{
#if 0
		// By pass, just use the first one
		memcpy(output, pcms.front(), samples * 2);
#else
		for (int i = 0; i < samples; i++)
		{
			double mixTotal = 0;
			for (size_t j = 0; j < pcms.size(); j++)
			{
				// Simple mix it with volume adjust
				double d = pcms[j][i] / 32767.0f;
				d = d * volumeFactors[j];
				mixTotal += d;

				// Relationship between t(treshold) and a(alpha) are pre-calculated as a mapping table, here we take t=0.6
				// treshold: 0     0.1   0.2   0.3   0.4   0.5   0.6   0.7    0.8    0.9
				// alpha:    2.51  2.84  3.26  3.82  4.59  5.71  7.48  10.63  17.51  41.15
				double out = 0;
				double t = 0.6;
				double a = 7.48;
				double x = mixTotal;
				int n = pcms.size();

				if (pcms.size() == 1 || (x >= -t && x <= t))
				{
					out = x;
				}
				else if (d == mixTotal || (d >= DLT_MIN && d <= DLT_MAX))
				{
					out = x;
				}
				else
				{
					out = (x / fabs(x)) * (t + (1 - t) * log(1 + a * ((fabs(x) - t) / (n - t))) / log(1 + a));
				}

				// Use adjusted value instead, avoid the overfolow
				mixTotal = (double)out;

				// Adjust master volume
				mixTotal = mixTotal * masterVolumeFactor;
			}

			// Convert to short
			int m = (short)(mixTotal * 32767);
			if (m > 32767)
			{
				m = 32767;
			}
			else if (m < -32767)
			{
				m = -32767;
			}

			// Assign to output
			output[i] = m;
		}
#endif
	}

	void zMedia::mixPcms(const vector<float*>& pcms, int samples, const vector<double>& volumeFactors, float* output, double masterVolumeFactor)
	{
#if 0
		// By pass, just use the first one
		memcpy(output, pcms.front(), samples * 2);
#else
		for (int i = 0; i < samples; i++)
		{
			double mixTotal = 0;
			for (size_t j = 0; j < pcms.size(); j++)
			{
				// Simple mix it with volume adjust
				//double d = pcms[j][i] / 32767.0f;
				double d = pcms[j][i];
				d = d * volumeFactors[j];
				mixTotal += d;

				// Relationship between t(treshold) and a(alpha) are pre-calculated as a mapping table, here we take t=0.6
				// treshold: 0     0.1   0.2   0.3   0.4   0.5   0.6   0.7    0.8    0.9
				// alpha:    2.51  2.84  3.26  3.82  4.59  5.71  7.48  10.63  17.51  41.15
				double out = 0;
				double t = 0.6;
				double a = 7.48;
				double x = mixTotal;
				int n = pcms.size();

				if (pcms.size() == 1 || (x >= -t && x <= t))
				{
					out = x;
				}
				else if (d == mixTotal || (d >= DLT_MIN && d <= DLT_MAX))
				{
					out = x;
				}
				else
				{
					out = (x / fabs(x)) * (t + (1 - t) * log(1 + a * ((fabs(x) - t) / (n - t))) / log(1 + a));
				}

				// Use adjusted value instead, avoid the overfolow
				mixTotal = (double)out;

				// Adjust master volume
				//mixTotal = mixTotal * masterVolumeFactor;
			}

			//if (mixTotal > 1.0)
			//	mixTotal = 1.0;
			//if (mixTotal < -1.0)
			//	mixTotal = -1.0;

			// Convert to short
			//int m = (short)(mixTotal * 32767);
			//if (m > 32767)
			//{
			//	m = 32767;
			//}
			//else if (m < -32767)
			//{
			//	m = -32767;
			//}

			// Assign to output
			output[i] = mixTotal;
		}
#endif
	}

    template<typename T>
    int calculatePcmLevelTmp(const T* pdata, int channel, int sampleCountPerChannel)
    {
        int level = 0;
        float sum = 0.0f;
        for(int i=0; i<sampleCountPerChannel; i++)
        {
            for(int ch=0; ch<channel; ch++)
            {
                sum += pdata[i*channel + ch];
            }
        }
        level = sum / sampleCountPerChannel;
        return level;
    }

	int zMedia::calculatePcmLevel(const PcmData::SPtr& srcData)
    {
        if(srcData==nullptr) return 0;
        int level = 0;
        switch(srcData->sampleSize())
        {
        case AudioSampleSize_BYTE:
            {
                return calculatePcmLevelTmp<uint8_t>((uint8_t*)srcData->data(), srcData->channels(), srcData->sampleCountPerChannel());
            }
            break;
        case AudioSampleSize_SHORT:
            {
                return calculatePcmLevelTmp<short>((short*)srcData->data(), srcData->channels(), srcData->sampleCountPerChannel());
            }
            break;
        case AudioSampleSize_FLOAT:
            {
                return calculatePcmLevelTmp<float>((float*)srcData->data(), srcData->channels(), srcData->sampleCountPerChannel());
            }
            break;
        }
        return level;
    }

    int zMedia::PCMPackedDataToPlanar(const float* packedData, int samplecount, int channel, float* planarData[])
    {
        if (packedData == nullptr || samplecount == 0 || channel == 0 || planarData == nullptr)
            return 0; 
        int copyedSampleCount = 0;
        float* src = (float*)packedData;
        for (int i = 0; i < samplecount; i++)
        {
            for (int j = 0; j < channel; j++)
            {
                planarData[j][i] = src[j];
            }
            src += channel;
            ++copyedSampleCount;
        }
        return copyedSampleCount;
    }

