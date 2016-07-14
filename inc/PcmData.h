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
		 *	@brief			���캯��
		 *					ʹ�øù��캯�����������Ч����ֱ�������ڴ棬�û������������allocBuffer�����ڴ�
		 *	@param[in]		uint32_t channels ͨ����
		 *	@param[in]		uint32_t sampleRate ������
		 *	@param[in]		AudioSampelTypeSize sampleType ÿ��ʱ��Sample���ֽڳ���
		 *	@param[in]		uint32_t timecount �ö����б����PCM���ݵĲ���ʱ��
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
		 *	@brief			�����ڴ�
		 *	@param[in]		int capacity �����ڴ�����ֽ���
		 *	@param[in]		malloc_func malloc_f �����ڴ�ķ���
		 *					Ĭ��ֵΪNULL,��ʱ��ʹ��malloc��Ϊ�ڴ���亯�����û��ɲ�����freeBuffer����������ʱ���Զ�ʹ��ϵͳfree���������ڴ��ͷ�
		 *					���ò�����ΪNULL�����û��ṩ�����ڴ�ĺ�������ʱ�û��ڶ�������ǰ�������freeBuffer�ͷ��ڴ棬��ʹ����ȷ��free����
		 *	@return			void 
		 **/
		size_t allocBuffer(size_t capacity, MemoryAllocator allocator = MemoryAllocator());
		/**
		 *	@name			freeBuffer
		 *	@brief			�ͷ�������ڴ�
		 *	@param[in]		free_func free_f �ͷ��ڴ�ĺ���
		 *					Ĭ��ֵΪNULL����ʱʹ��ϵͳ�ṩ��free���������ͷ�
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
		 *	@brief			��ȡ��ǰ�����PCM�����ܲ��Ŷ೤ʱ��
		 *	@return			long ���ݵ�ʱ�䳤�ȣ���λ����
		 **/
		inline long getTimeCount() const { return sampleCount_Channel() * 1000 / m_nSampleRate; }

		/**
		 *	@name			sampleCount_Channel
		 *	@brief			��ȡ����ͨ���Ĳ�����������
		 *	@return			uint32_t ����ͨ���Ĳ����������� 
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
		 *	@brief			��ȡ����ͨ���ĵ����������ֽ���
		 *	@return			uint32_t ����ͨ���ĵ����������ֽ���
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
		//�����븳ֵ�����뿽�����ݣ��Ƽ�ʹ������ָ�룬��˲�֧�ָ����븳ֵ
		PcmData(const PcmData& robj);
		PcmData& operator=(const PcmData& robj);

	private:
		AudioSampelTypeSize m_PerSampleByteCount;
		uint32_t m_nSampleRate; // ����Ƶ�� //Ĭ��44HZ
		uint32_t m_nChannels; // ����  //Ĭ��2����
		int  m_nVolume; // ���� [0, 100]
		bool m_bIsMuted;

		size_t m_nDataSize; //���е�AudioSample��, ռ�õ��ڴ�Ϊm_sampleSize*m_nSampleCount
        MediaBuffer m_buf;
		long m_nTimeStamp; //��ǰ���ݵ�PTSʱ���
		int m_nTimeCount;	//��ǰbuffer�ܹ������������ݵ�ʱ���ȣ���λΪ����
	};
}//namespace zMedia

#endif//_MEDIA_FILTER_PCM_DATA_H_
