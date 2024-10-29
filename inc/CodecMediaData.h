#ifndef _MEDIA_FILTER_CODEC_MEDIA_DATA_H_
#define _MEDIA_FILTER_CODEC_MEDIA_DATA_H_

#include "PictureInfo.h"
#include "MediaBuffer.h"
#include <memory.h>

namespace zMedia
{
    /**
     * @brief 编码后的视频数据帧
     */
    class PictureCodec
    {
    public:
        typedef PictureCodec SelfType;
        typedef std::shared_ptr<PictureCodec> SPtr;

		PictureCodec() : m_subtype(0), m_nPts(0), m_nDts(0) {}

        PictureCodec(uint32_t fourcc, uint32_t subtype) : m_frameType(fourcc), m_subtype(subtype), m_nPts(0), m_nDts(0) {}

        PictureCodec(uint32_t fourcc, uint32_t subtype, size_t length, const MemoryAllocator& allocator = MemoryAllocator())
			: m_frameType(fourcc), m_subtype(subtype) 
			, m_nPts(0), m_nDts(0)
        {
            allocData(length, allocator);
        }
        
        PictureCodec(uint32_t fourcc, uint32_t subtype, BYTE* data, size_t length)
			: m_frameType(fourcc), m_subtype(subtype)
			, m_nPts(0), m_nDts(0)
        {
            attachData(data, length);
        }

        virtual ~PictureCodec(){
            free();
        }

        void setFrameType(uint32_t fourcc) { m_frameType = fourcc; }
        uint32_t getFrameType() const { return m_frameType; }

        void setSubtype(uint32_t subtype) { m_subtype = subtype; }
        uint32_t getSubtype() const { return m_subtype; }
        
		void setPts(int64_t nPts) { m_nPts = nPts; }
		int64_t getPts() const { return m_nPts; }

		void setDts(int64_t nDts) { m_nDts = nDts; }
		int64_t getDts() const { return m_nDts; }

        size_t allocData(size_t length, const MemoryAllocator& allocator = MemoryAllocator())
        {
            return m_buf.malloc(length, allocator);
        }

        /**
         * @brief   malloc内存空间，并将srcBuf中的数据的length字节拷贝到内存空间中
         */
        size_t allocData(uint8_t* srcBuf, size_t length, const MemoryAllocator& allocator = MemoryAllocator())
        {
            size_t ret = m_buf.malloc(length, allocator);
            if (ret < length)
                return ret;
            memcpy(m_buf.data(), srcBuf, length);
            m_buf.setPayloadSize(length);
            return ret;
        }

        bool attachData(BYTE* data, size_t length, const MemoryAllocator& allocator = MemoryAllocator())
        {
            return m_buf.attachData(data, length, allocator);
        }

        size_t free()
        {
            return m_buf.free();
        }

        BYTE* data() { return m_buf.data(); }
        const BYTE* data() const { return m_buf.data(); }
        const MediaBuffer& buffer() const { return m_buf; }
        size_t size() const { return m_buf.getPayloadSize(); }
    private:
        PictureCodec(const PictureCodec& robj);
        PictureCodec& operator=(const PictureCodec& robj);
    private:
        uint32_t m_frameType = FOURCC_Unknown;    // 编码后的视频帧类型，比如H264，H265, AV1等
        uint32_t m_subtype = 0;                   // 子类型，比如I帧、P帧。如果是H264则可以对应EncVideoFrameType
        MediaBuffer m_buf;
		int64_t m_nPts = 0;
		int64_t m_nDts = 0;
    };//class PictureCodec

    /**
     * @brief 编码后的音频数据
     */
    class AudioCodec
    {
    public:
        typedef AudioCodec SelfType;
        typedef std::shared_ptr<AudioCodec> SPtr;

        AudioCodec() : m_forcc(0), m_subtype(0), m_timestamp(0) {}
        
        AudioCodec(uint32_t fourcc) : m_forcc(fourcc), m_subtype(0), m_timestamp(0) {}

        AudioCodec(uint32_t fourcc, size_t length, const MemoryAllocator& allocator = MemoryAllocator())
            : m_forcc(fourcc), m_subtype(0), m_timestamp(0)
        {
            allocData(length, allocator);
        }
        
        AudioCodec(uint32_t fourcc, BYTE* data, size_t length)
            : m_forcc(fourcc), m_subtype(0), m_timestamp(0)
        {
            attachData(data, length);
        }

        virtual ~AudioCodec() {
            free();
        }

        void setTimestamp(uint64_t ts) { m_timestamp = ts; }
        uint64_t getTimestamp() const { return m_timestamp; }
        
        void setFORCC(uint32_t forcc) { m_forcc = forcc; }
        uint32_t getFORCC() const { return m_forcc; }

        void setSubtype(uint32_t subtype) { m_subtype = subtype; }
        uint32_t getSubtype() const { return m_subtype; }

        size_t allocData(size_t length, const MemoryAllocator& allocator = MemoryAllocator())
        {
            return m_buf.malloc(length, allocator);
        }

        /**
         * @brief   malloc内存空间，并将srcBuf中的数据的length字节拷贝到内存空间中
         */
        size_t allocData(uint8_t* srcBuf, size_t length, const MemoryAllocator& allocator = MemoryAllocator())
        {
            size_t ret = m_buf.malloc(length, allocator);
            if (ret < length)
                return ret;
            memcpy(m_buf.data(), srcBuf, length);
            m_buf.setPayloadSize(length);
            return ret;
        }

        bool attachData(BYTE* data, size_t length, const MemoryAllocator& allocator = MemoryAllocator())
        {
            return m_buf.attachData(data, length, allocator);
        }

        size_t free()
        {
            return m_buf.free();
        }

        BYTE* data() { return m_buf.data(); }
        const BYTE* data() const { return m_buf.data(); }
        const MediaBuffer& buffer() const { return m_buf; }
        size_t size() const { return m_buf.getPayloadSize(); }
    private:
        AudioCodec(const AudioCodec& robj);
        AudioCodec& operator=(const AudioCodec& robj);
    private:
        uint32_t m_forcc = FOURCC_Unknown;
        uint32_t m_subtype = 0;
        uint64_t m_timestamp = 0;
        MediaBuffer m_buf;
    };//class AudioCodec
}//namespace zMedia

#endif //_MEDIA_FILTER_CODEC_MEDIA_DATA_H_
