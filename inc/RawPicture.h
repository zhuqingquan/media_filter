#pragma once
#ifndef _Z_MEDIA_RAW_PICTURE_H_
#define _Z_MEDIA_RAW_PICTURE_H_

#include "PictureInfo.h"
#include "TextureHolder.h"
#include "FrameMetaData.h"
#include <vector>

namespace zMedia
{
    // 视频数据对象
    // composite with PICTURE_FORMAT and MediaBuffer
    class PictureRaw : public FrameMetaDataValid
    {
    public:
        typedef PictureRaw SelfType;
        typedef std::shared_ptr<PictureRaw> SPtr;

        PictureRaw();
        ~PictureRaw();

        //functions for get the information about this picture
        inline const PICTURE_FORMAT& format() const { return m_format; }
        inline const MediaBuffer& buffer() const { return m_buf; }
        inline const uint8_t* data() const { return m_buf.data(); }
        inline uint8_t* data() { return m_buf.data(); }
        size_t size() const	{ return m_format.isValid() ? GetPictureSize(m_format) : 0; }
        inline const uint8_t* rgb() const;
        inline const uint8_t* yuv() const;
        inline const uint8_t* y() const;
        inline const uint8_t* u() const;
        inline const uint8_t* v() const;
        // for NV12 YV12, the UV or VU planar
        inline const uint8_t* uv() const;
        inline const uint8_t* a() const;
        inline TextureHolder::SPtr texture() const { return m_texture; }

        /**
         *	@name			allocData
         *	@brief			根据参数_format申请用于保存图片数据的内存空间
         *					用户可提供allocator指定内存申请以及释放使用的函数，如果未指定，则默认使用BigBufferManager中提供的共享内存管理方法
         *	@param[in]		const PICTURE_FORMAT & _format 图片的属性
         *	@param[in]		const MemoryAllocator & allocator 内存申请释放对象
         *	@return			bool true--成功  false--失败，可能_format指定的图片属性不正确，或者申请内存失败
         **/
        bool allocData(const PICTURE_FORMAT& _format, const MemoryAllocator& allocator = MemoryAllocator());
        /**
         *	@name			attachData
         *	@brief			将当前对象与已申请的内存绑定
         *					当前对象被释放时，不会释放绑定的内存，用户需要自己负责释放内存
         *					此处绑定是不坚持数据是否为空以及数据的长度是否符合_format指定的图片属性所需要的数据长度
         *					若当前对象已有数据（申请的或者绑定的）则之前的数据将不再引用，申请的将被释放，绑定的将不再绑定
         *	@param[in]		uint8_t * pData 用户内存块首地址
         *	@param[in]		size_t len 内存块大小
         *	@param[in]		const PICTURE_FORMAT & _format 图片属性
         **/
        bool attachData(uint8_t* pData, size_t len, const PICTURE_FORMAT& _format, const MemoryAllocator& allocator = MemoryAllocator());

        /**
         * @name			attachData
         * @brief		Attach a opengl texture to this object
         *				After call this func success, call texture() function to get TextureHodler setted in hear
         * @param]in]	texture TextureHolder的智能指针
         * @return		bool true--success false--failed
         **/
        bool attachData(const PICTURE_FORMAT& _format, const TextureHolder::SPtr texture);

        /**
         * 释放数据
         * 如果数据是本对象申请的则会进行释放，如果是attached的则解除attach。
         **/
        void freeData();

        /**
         *  从src中拷贝图像数据到当前对象中
         *  @param      src 图像数据的内存指针数组。
         *  @param      src_stride 图像数据内存数据相对应的每个的stride
         *  @param      x x轴坐标，表明从src的x处开始拷贝数据
         *  @param      y y轴坐标，表明从src的y处开始拷贝数据
         *  @param      w 宽，拷贝的数据的像素个数
         *  @param      h 高，拷贝的数据的像素行数
         **/
        bool copyData(uint8_t** src, int* src_stride, int x, int y, int w, int h);

        /**
         * 时间戳
         **/
        void setTimestamp(int64_t pts) { m_pts = pts; }

        /**
         * 获取时间戳
         **/
        int64_t getTimestamp() const { return m_pts; }

        /**
         * 设置透明通道遮盖层
         **/
        void setAlphaMask(PictureRaw::SPtr alphaMask) { m_alphaMask = alphaMask; }

        /**
         * 获取设置的透明通道遮盖层
         **/
        PictureRaw::SPtr getAlphaMask() const { return m_alphaMask; }

        bool maskAlpha();

        /**
         *  当前Picture的遮盖图片
         **/
        struct Overlay
        {
            int left;                           // 显示位置，左
            int top;                            // 显示位置，上
            PictureRaw::SPtr overlay;           // 遮盖层的图片

            Overlay(int l, int t, const PictureRaw::SPtr& pic)
                : left(l), top(t), overlay(pic)
            {

            }
        };
        /**
         *  叠加的遮盖层。此遮盖层直接叠加在此画面之上
         * @param   rect 叠加的位置
         * @param   overlay 遮盖层的图片
         **/
        bool addOverlay(const Overlay& overlay);

        /**
         *  删除遮盖层
         * @param   rect 要删除的遮盖层的位置
         * @param   overlay 要删除的遮盖层的图片
         **/
        bool removeOverlay(const Overlay& overlay);

        /**
         *  获取当前图片已经添加的Overlay列表
         **/
        std::vector<Overlay> getOverlays() const 
        {
            return m_overlays;
        }

        /**
         * 设置当前帧是否透明遮罩层的数据内嵌到当前帧的右边
         * 此标记将可能影响对帧的渲染方式
         **/
        void setAlphaEmbedded(bool isEmbedded) { m_isAlphaEmbedded = isEmbedded; }
        bool isAlphaEmbedded() const { return m_isAlphaEmbedded; }
    private:
        PictureRaw(const PictureRaw& robj);
        PictureRaw& operator=(const PictureRaw& robj);

        PICTURE_FORMAT m_format;
        MediaBuffer m_buf;
        TextureHolder::SPtr m_texture;
        uint64_t m_pts = 0;//after decoded we get pts.Not need dts in PICTURE after decoded.
        PictureRaw::SPtr m_alphaMask;       // 透明遮盖，如果设置了这个额外的透明层，将使用这一层中的数据作为透明通道进行显示
        std::vector<Overlay> m_overlays;    // 当前图片的遮盖层
        bool m_isAlphaEmbedded = false;     // 是否将透明遮罩层的数据内嵌到当前帧的右边
    };//class PictureRaw

    const uint8_t* PictureRaw::rgb() const
    {
        if(!m_format.isValid()) return NULL;
        switch (m_format.ePixfmt)
        {
        case PIXFMT_E_YV12:
        case PIXFMT_E_I420:
        case PIXFMT_E_YUY2:
        case PIXFMT_E_UYVY:
        case PIXFMT_E_NV12:
            return NULL;
        case PIXFMT_E_RGB565:
        case PIXFMT_E_RGB24:
        case PIXFMT_E_RGB32:
        case PIXFMT_E_RGBA:
        case PIXFMT_E_BGRA:
        case PIXFMT_E_R8:
            return m_buf.data();
        default:
            assert(! "不支持的像素格式");
            return NULL;
        }
    }

    const uint8_t* PictureRaw::yuv() const
    {
        if(!m_format.isValid()) return NULL;
        switch (m_format.ePixfmt)
        {
        case PIXFMT_E_YV12:
        case PIXFMT_E_I420:
        case PIXFMT_E_YUY2:
        case PIXFMT_E_UYVY:
        case PIXFMT_E_NV12:
            return m_buf.data();
        case PIXFMT_E_RGB565:
        case PIXFMT_E_RGB24:
        case PIXFMT_E_RGB32:
        case PIXFMT_E_RGBA:
        case PIXFMT_E_BGRA:
        case PIXFMT_E_R8:
            return NULL;
        default:
            assert(! "不支持的像素格式");
            return NULL;
        }
    }

    const uint8_t* PictureRaw::y() const
    {
        if(!m_format.isValid()) return NULL;
        switch (m_format.ePixfmt)
        {
        case PIXFMT_E_YV12:
        case PIXFMT_E_I420:
        case PIXFMT_E_NV12:
            return m_buf.data();
        case PIXFMT_E_YUY2:
        case PIXFMT_E_UYVY:
            return NULL;
        case PIXFMT_E_RGB565:
        case PIXFMT_E_RGB24:
        case PIXFMT_E_RGB32:
        case PIXFMT_E_RGBA:
        case PIXFMT_E_BGRA:
        case PIXFMT_E_R8:
            return NULL;
        default:
            assert(! "不支持的像素格式");
            return NULL;
        }
    }

    const uint8_t* PictureRaw::u() const
    {
        if(!m_format.isValid()) return NULL;
        switch (m_format.ePixfmt)
        {
        case PIXFMT_E_YV12:
            //yyyy vv uu
            return m_buf.data() + m_format.y_stride*m_format.h + m_format.v_stride * ((m_format.h + 1) >> 1);
        case PIXFMT_E_I420:
            //yyyy uu vv
            return m_buf.data() + m_format.y_stride*m_format.h;
        case PIXFMT_E_YUY2:
        case PIXFMT_E_UYVY:
        case PIXFMT_E_NV12:
            return NULL;
        case PIXFMT_E_RGB565:
        case PIXFMT_E_RGB24:
        case PIXFMT_E_RGB32:
        case PIXFMT_E_RGBA:
        case PIXFMT_E_BGRA:
        case PIXFMT_E_R8:
            return NULL;
        default:
            assert(! "不支持的像素格式");
            return NULL;
        }
    }

    const uint8_t* PictureRaw::v() const
    {
        if(!m_format.isValid()) return NULL;
        switch (m_format.ePixfmt)
        {
        case PIXFMT_E_YV12:
            //yyyy vv uu
            return m_buf.data() + m_format.y_stride*m_format.h;
        case PIXFMT_E_I420:
            return m_buf.data() + m_format.y_stride*m_format.h + m_format.u_stride * ((m_format.h + 1) >> 1);
        case PIXFMT_E_YUY2:
        case PIXFMT_E_UYVY:
        case PIXFMT_E_NV12:
            return NULL;
        case PIXFMT_E_RGB565:
        case PIXFMT_E_RGB24:
        case PIXFMT_E_RGB32:
        case PIXFMT_E_RGBA:
        case PIXFMT_E_BGRA:
        case PIXFMT_E_R8:
            return NULL;
        default:
            assert(! "不支持的像素格式");
            return NULL;
        }
    }

    const uint8_t* PictureRaw::uv() const
    {
        if (!m_format.isValid()) return NULL;
        switch (m_format.ePixfmt)
        {
        case PIXFMT_E_NV12:
            //yyyy vv uu
            return m_buf.data() + m_format.y_stride*m_format.h;
        case PIXFMT_E_I420:
        case PIXFMT_E_YUY2:
        case PIXFMT_E_UYVY:
        case PIXFMT_E_YV12:
            return NULL;
        case PIXFMT_E_RGB565:
        case PIXFMT_E_RGB24:
        case PIXFMT_E_RGB32:
        case PIXFMT_E_RGBA:
        case PIXFMT_E_BGRA:
        case PIXFMT_E_R8:
            return NULL;
        default:
            assert(!"不支持的像素格式");
            return NULL;
        }
    }

    const uint8_t* PictureRaw::a() const
    {
        if(!m_format.isValid()) return NULL;
        switch (m_format.ePixfmt)
        {
        case PIXFMT_E_YV12:
        case PIXFMT_E_I420:
        case PIXFMT_E_NV12:
        case PIXFMT_E_YUY2:
        case PIXFMT_E_UYVY:
        case PIXFMT_E_RGB565:
        case PIXFMT_E_RGB24:
        case PIXFMT_E_RGB32:
        case PIXFMT_E_RGBA:
        case PIXFMT_E_BGRA:
        case PIXFMT_E_R8:
            return NULL;
        default:
            assert(! "不支持的像素格式");
            return NULL;
        }
    }

    /**
     * @brief 拷贝src的局部区域数据到一个新的PictureRaw对象中
     * @param src 源
     * @param x 拷贝的区域，像素位置，x。取值范围 [0, src.width)
     * @param y 拷贝的区域，像素位置，y。取值范围 [0, src.height)
     * @param w 拷贝的区域，像素宽，w。取值范围 x + w < src.width
     * @param h 拷贝的区域，像素高，h。取值范围 y + h < src.height
     * @return PictureRaw::SPtr 拷贝的结果
     */
    PictureRaw::SPtr copySubImage(const PictureRaw::SPtr& src, int x, int y, int w, int h);

    /**
     * @brief 缩放src图片，返回缩放后的新图片，如果失败则返回nullptr
     * 
     * @param src 源图片
     * @param w 缩放后的宽，像素
     * @param h 缩放后的高，像素
     * @return PictureRaw::SPtr 缩放后的新图片，如果失败则返回nullptr
     */
    PictureRaw::SPtr scaleImage(const PictureRaw::SPtr& src, int w, int h);

    /**
     * @brief 转换图片的像素格式
     * 
     * @param src 源图片
     * @param pixfmt 转换后的像素格式
     * @param needPadding 是否需要在每行像素数据进行对其
     * @return PictureRaw::SPtr 转换后的图片，转换失败则返回nullptr
     */
    PictureRaw::SPtr convertImage(const PictureRaw::SPtr& src, zMedia::E_PIXFMT pixfmt, bool needPadding = true);

    bool dumpToFile_i420(FILE* file, const PictureRaw::SPtr& pic);
}//namespace zMedia

#endif//_Z_MEDIA_RAW_PICTURE_H_
