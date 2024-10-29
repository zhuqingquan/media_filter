#ifndef _MEDIA_FILTER_PCM_DATA_H_
#define _MEDIA_FILTER_PCM_DATA_H_
 
#include <assert.h>
#include "MediaBuffer.h"
#include "FrameMetaData.h"

namespace zMedia
{
	enum AudioSampelTypeSize
	{
        AudioSampleSize_NONE = 0,
		AudioSampleSize_BYTE = sizeof(uint8_t),
		AudioSampleSize_SHORT = sizeof(short),
		AudioSampleSize_FLOAT = sizeof(float)
	};

	class PcmData : public FrameMetaDataValid
	{
	public:
        typedef PcmData SelfType;
        typedef std::shared_ptr<PcmData> SPtr;

		/**
		 *	@name			PcmData
		 *	@brief			Constructor
		 *	@param[in]		uint32_t channels
		 *	@param[in]		uint32_t sampleRate Sample rate of the audio data
		 *	@param[in]		AudioSampelTypeSize sampleType Bytes of one sample.See AudioSampelTypeSize
		 **/
		PcmData(uint32_t channels, uint32_t sampleRate, AudioSampelTypeSize sampleType);

		~PcmData();

		/**
		 *	@name			data
		 *	@brief	        Get the beginning pointer to this PcmData buffer.
		 *	@return			uint8_t* The beginning pointer to this PcmData buffer
		 **/
		inline const uint8_t* data() const	{ return (uint8_t*)m_buf.data(); }
		inline uint8_t* data() { return (uint8_t*)m_buf.data(); }
		inline const uint8_t* payload() const { return (uint8_t*)m_buf.payload(); }
		inline uint8_t* payload() { return (uint8_t*)m_buf.payload(); }
        inline const MediaBuffer& buffer() const { return m_buf; }
        const MemoryAllocator& memAllocator() const { return m_buf.memAllocator(); }

		/**
		 *	@name			allocBuffer
		 *	@brief			Malloc memory for save PCM data, buffer size is based on the param in constructor
		 *	@param[in]		malloc_func malloc_f 分配内存的方法
		 *	@param[in]		uint32_t timecount The total time spended to play the data in the obj.In millsec
         *	                Capacity = sampleRate * channels * BytesPerSample * timecount / 1000
		 *	@return			void 
		 **/
		size_t malloc_timecount(uint32_t timecount, MemoryAllocator allocator = MemoryAllocator());
		/**
		 *	@name			allocBuffer
		 *	@brief			Malloc memory for save PCM data, buffer size is based on the param in constructor
		 *	@param[in]		malloc_func malloc_f 分配内存的方法
		 *	@param[in]		uint32_t sampleCountPerChannel The total sample count in the obj.
         *	                This is the sample count of every channel.
         *	                Capacity = sampleCountPerChannel * channels * BytesPerSample
		 *	@return			void 
		 **/
		size_t malloc_samplecount(uint32_t sampleCountPerChannel, MemoryAllocator allocator = MemoryAllocator());
		/**
		 *	@name			free
		 *	@brief			free the memory allocate by malloc func.
		 *	@return			void 
		 **/
		size_t free();

		inline uint32_t sampleRate() const{ return m_nSampleRate; }
		inline uint32_t channels() const { return m_nChannels; }

		void setTimeStamp(uint64_t nTimeStamp){m_nTimeStamp = nTimeStamp;}
		inline uint64_t getTimeStamp() const {return m_nTimeStamp;}
		/**
		 *	@name			getTimeCount
		 *	@brief			Get the total time count in this obj.
         *	                This value is const after malloc_xxx called.
		 *	@return			long 数据的时间长度，单位毫秒
		 **/
		inline long getTimeCount() const { return m_nTimeCount; }

		/**
		 *	@name			sampleCount_Channel
		 *	@brief			获取单个通道的采样个数总数
		 *	@return			uint32_t 单个通道的采样个数总数 
		 **/
		inline uint32_t sampleCountPerChannel() const { return m_nChannels==0 ? 0 : sampleCount() / m_nChannels; }
		inline uint32_t sampleCount() const { return 0== m_PerSampleByteCount ? 0 : (uint32_t)size() / m_PerSampleByteCount; }
		/**
		 *	@name			sampleSize_Channel
		 *	@brief			获取所有通道的单个采样的字节数
		 *	@return			uint32_t 所有通道的单个采样的字节数
		 **/
		inline uint32_t sampleSizeAllChannels() { return m_PerSampleByteCount*m_nChannels; }
		inline AudioSampelTypeSize sampleSize() { return m_PerSampleByteCount; }

		/**
		 *	@name			capacity
		 *	@brief			Get the total size in this obj.
         *	                This func is valid after call malloc
		 *	@return			size_t Total size malloced in the obj.
		 **/
		inline size_t capacity() const { return m_capacity; }
		/**
		 *	@name			size
		 *	@brief			Get the valid data size in this obj.
         *	                The valid data size is changed by appendData func
		 *	@return			size_t Valid data size in the obj.
		 **/
        inline size_t size() const { return m_buf.getPayloadSize(); }
        inline size_t freeSize() const { return capacity() - size(); }
		inline bool empty() const { return freeSize()==capacity(); }
		inline bool full() const { return size()==capacity(); }
		inline void clear() { m_buf.setPayloadOffset(0); m_buf.setPayloadSize(0); }

		/**
		 *	@brief 当使用者是直接通过data()获取到内部指针并将数据拷贝进去时需要使用此接口正确设置有效数据的大小
		 **/
		inline void setPayloadSize(uint32_t payloadSize) { m_buf.setPayloadSize(payloadSize); }
		inline void setPayloadOffset(uint32_t offset) { m_buf.setPayloadOffset(offset); }

		/**
		 *	@name			appendData
		 *	@brief	        copy and append data to the tail of the buffer.	
		 *	@return			size_t Bytes of data that copyed.
		 **/
		size_t appendData(const uint8_t* data, size_t bytesCount);
        bool isSilence();

	private:
		//因复制与赋值都必须拷贝数据，推荐使用智能指针，因此不支持复制与赋值
		PcmData(const PcmData& robj);
		PcmData& operator=(const PcmData& robj);

	private:
		AudioSampelTypeSize m_PerSampleByteCount;
		uint32_t m_nSampleRate; // 采样频率 //默认44HZ
		uint32_t m_nChannels; // 声道  //默认2声道
		int m_nTimeCount;	//当前buffer能够保存声音数据的时间跨度，单位为毫秒
		uint64_t m_nTimeStamp; //当前数据的PTS时间戳

        size_t m_capacity;
        MediaBuffer m_buf;
	};

    /**
     *  复制一帧音频数据的PCM
     **/
    PcmData::SPtr copyPcmFrame(const PcmData::SPtr& src);
}//namespace zMedia

#endif//_MEDIA_FILTER_PCM_DATA_H_
