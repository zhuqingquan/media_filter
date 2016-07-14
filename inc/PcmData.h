#ifndef _MEDIA_FILTER_PCM_DATA_H_
#define _MEDIA_FILTER_PCM_DATA_H_
 
#include <assert.h>
#include "BoostInc.h"
#include "mediafilter.h"
#include "MediaData.h"

namespace zMedia
{
	enum AudioSampelTypeSize
	{
        AudioSampleSize_NONE = 0,
		AudioSampleSize_BYTE = sizeof(uint8_t),
		AudioSampleSize_SHORT = sizeof(short),
		AudioSampleSize_FLOAT = sizeof(float)
	};

	class PcmData
	{
	public:
		typedef PcmData SelfType;
		typedef boost::shared_ptr<PcmData> SPtr;

		PcmData();
		/**
		 *	@name			PcmData
		 *	@brief			构造函数
		 *					使用该构造函数如果参数有效，则将直接申请内存，用户无需另外调用allocBuffer申请内存
		 *	@param[in]		uint32_t channels 通道数
		 *	@param[in]		uint32_t sampleRate 采样率
		 *	@param[in]		AudioSampelTypeSize sampleType 每个时间Sample的字节长度
		 *	@param[in]		uint32_t timecount 该对象中保存的PCM数据的播放时长
		 *	@return			 
		 **/
		PcmData(uint32_t channels, uint32_t sampleRate, AudioSampelTypeSize sampleType, uint32_t timecount, const MemoryAllocator& allocator = MemoryAllocator());
		~PcmData();

		inline const BYTE* data() const	{ return (BYTE*)m_buf.data(); }
		inline BYTE* data() { return (BYTE*)m_buf.data(); }
        const MemoryAllocator& memAllocator() const { return m_buf.memAllocator(); }
		inline size_t size() const { return m_nDataSize; }
		inline void setWritePos(size_t writePos) { m_nDataSize = (writePos<=m_buf.length() ? writePos : m_buf.length()); }

		/**
		 *	@name			allocBuffer
		 *	@brief			申请内存
		 *	@param[in]		int capacity 申请内存的总字节数
		 *	@param[in]		malloc_func malloc_f 分配内存的方法
		 *					默认值为NULL,此时将使用malloc作为内存分配函数，用户可不调用freeBuffer，对象析构时将自动使用系统free函数进行内存释放
		 *					若该参数不为NULL，即用户提供申请内存的函数，此时用户在对象析构前必须调用freeBuffer释放内存，并使用正确的free函数
		 *	@return			void 
		 **/
		size_t allocBuffer(size_t capacity, MemoryAllocator allocator = MemoryAllocator());
		/**
		 *	@name			freeBuffer
		 *	@brief			释放申请的内存
		 *	@param[in]		free_func free_f 释放内存的函数
		 *					默认值为NULL，此时使用系统提供的free函数进行释放
		 *	@return			void 
		 **/
		size_t free();

		inline uint32_t sampleRate() const{ return m_nSampleRate; }
		inline void setSampleRate(uint32_t nSampleRate)	{ m_nSampleRate = nSampleRate;	}

		inline uint32_t channels() const { return m_nChannels; }
		inline void setChannels(uint32_t nChannels)	{ m_nChannels = nChannels;}

		inline int volume() const { return m_nVolume; }
		inline void setVolume(int nVolume)	
		{ assert(nVolume >=0 && nVolume <= 100); m_nVolume = nVolume; }

		inline bool isMuted() const { return m_bIsMuted; }
		inline void setIsMuted(bool mute) { m_bIsMuted = mute; }

		void setTimeStamp(long nTimeStamp){m_nTimeStamp = nTimeStamp;}
		inline long getTimeStamp() const {return m_nTimeStamp;}
		/**
		 *	@name			getTimeCount
		 *	@brief			获取当前保存的PCM数据能播放多长时间
		 *	@return			long 数据的时间长度，单位毫秒
		 **/
		inline long getTimeCount() const { return sampleCount_Channel() * 1000 / m_nSampleRate; }

		/**
		 *	@name			sampleCount_Channel
		 *	@brief			获取单个通道的采样个数总数
		 *	@return			uint32_t 单个通道的采样个数总数 
		 **/
		inline uint32_t sampleCount_Channel() const { return sampleCount() / m_nChannels; }
		inline uint32_t sampleCount() const { return m_nDataSize / m_PerSampleByteCount; }
		inline void setSampleCount(uint32_t nSampleCount)
		{
			uint32_t bytesCount = nSampleCount * m_PerSampleByteCount;
			assert(m_buf.length() >= bytesCount);
			m_nDataSize = bytesCount; 
		}
		/**
		 *	@name			sampleSize_Channel
		 *	@brief			获取所有通道的单个采样的字节数
		 *	@return			uint32_t 所有通道的单个采样的字节数
		 **/
		inline uint32_t sampleSize_Channel() { return m_PerSampleByteCount*m_nChannels; }
		inline AudioSampelTypeSize sampleSize() { return m_PerSampleByteCount; }
		inline void setSampleSize(AudioSampelTypeSize type) { m_PerSampleByteCount = type; }

		inline size_t capacity() const { return m_buf.length(); }
		inline bool empty() const { return m_nDataSize == 0; }
		inline bool full() const { return m_nDataSize == m_buf.length(); }
		inline void clear() { m_nDataSize = 0; }

		void setData(const BYTE* data, uint32_t bytesCount);
		uint32_t appendData(const BYTE* data, uint32_t bytesCount);

	private:
		//因复制与赋值都必须拷贝数据，推荐使用智能指针，因此不支持复制与赋值
		PcmData(const PcmData& robj);
		PcmData& operator=(const PcmData& robj);

	private:
		AudioSampelTypeSize m_PerSampleByteCount;
		uint32_t m_nSampleRate; // 采样频率 //默认44HZ
		uint32_t m_nChannels; // 声道  //默认2声道
		int  m_nVolume; // 音量 [0, 100]
		bool m_bIsMuted;

		size_t m_nDataSize; //已有的AudioSample数, 占用的内存为m_sampleSize*m_nSampleCount
        MediaBuffer m_buf;
		long m_nTimeStamp; //当前数据的PTS时间戳
		int m_nTimeCount;	//当前buffer能够保存声音数据的时间跨度，单位为毫秒
	};
}//namespace zMedia

#endif//_MEDIA_FILTER_PCM_DATA_H_
