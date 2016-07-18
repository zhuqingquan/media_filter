//#pragma once
#ifndef _MEDIA_FILTER_PICTURE_RAW_H_
#define _MEDIA_FILTER_PICTURE_RAW_H_

#include <assert.h>
#include <limits.h>
#include "MediaData.h"
#include "forcc.h"
#include "mediafilter.h"


namespace zMedia
{
		//色彩空间 define
		enum E_COLORSPACE
		{
			COLOR_E_NONE = 0,
			COLOR_E_YV12,  // yvu yvu
			COLOR_E_I420,	//等同FOURCC_IYUV  //yuv yuv
			COLOR_E_YUY2,	//等同FOURCC_YUYV
			COLOR_E_UYVY,
			COLOR_E_RGB565,   
			COLOR_E_RGB24,
			COLOR_E_RGB32,  //BGRA BGRA
			COLOR_E_RGBA,   //BGRA BGRA
			COLOR_E_MAX,
		};

		//单个像素的字节个数
		//需配合E_COLORSPACE使用，如 PixelBytesCount[COLOR_E_RGB32], 若E_COLORSPACE的修改导致PixelBytesCount[n]出错，则应同步修改PixelBytesCount
		static const float PixelBytesCount[] = {0, 1.5, 1.5, 2, 2, 2, 3, 4, 4, 4};

		struct PICTURE_HEADER
		{
			PICTURE_HEADER():w(0),h(0),eColor(COLOR_E_NONE),stride(0),uv_stride(0) {}

			PICTURE_HEADER(int width, int height, E_COLORSPACE colorspace)
				: w(width), h(height), eColor(colorspace)
				, stride(0), uv_stride(0)
			{
				switch(eColor)
				{
				case COLOR_E_YUY2:
				case COLOR_E_UYVY:
				case COLOR_E_RGB565:
				case COLOR_E_RGB24:
				case COLOR_E_RGB32:
				case COLOR_E_RGBA:
				case COLOR_E_MAX:
					y_stride =Align16Bytes((int) (w * PixelBytesCount[eColor]));
					break;
				case COLOR_E_YV12:
				case COLOR_E_I420:
					y_stride = Align16Bytes(w);
					uv_stride = Align16Bytes(w>>1);//w/2
					break;
				default:
					break;
				}
			}

			PICTURE_HEADER(int width, int height, E_COLORSPACE colorspace, int _stride, int uvStride)
				: w(width), h(height), eColor(colorspace)
				, stride(_stride), uv_stride(uvStride)
			{
			}

			bool isValid() const 
			{ 
				//最大支持4K
				if(w<=0 || h<=0 || w>3840 || h>2160)	return false;
				switch(eColor)
				{
				case COLOR_E_NONE:
					return false;
				case COLOR_E_YUY2:
				case COLOR_E_UYVY:
				case COLOR_E_RGB565:
				case COLOR_E_RGB24:
				case COLOR_E_RGB32:
				case COLOR_E_RGBA:
				case COLOR_E_MAX:
					return y_stride>0;
				case COLOR_E_YV12:
				case COLOR_E_I420:
					return y_stride>0 && uv_stride>0;
				default:
					return false;
				}
			}
			int w;
			int h;
			E_COLORSPACE eColor;
			union {
				int stride;
				int y_stride;
			};
			int uv_stride;//仅仅平面格式用到，否则不用
		};

		inline int GetBitCount(E_COLORSPACE eColor)
		{
			int nBitCount = 0;
			switch (eColor)
			{
			case COLOR_E_YV12:
			case COLOR_E_I420:
				nBitCount = 12;
				break;
			case COLOR_E_YUY2:
			case COLOR_E_UYVY:
			case COLOR_E_RGB565:
				nBitCount = 16;
				break;	
			case COLOR_E_RGB24:
				nBitCount = 24;
				break;		
			case COLOR_E_RGB32:
				nBitCount = 32;
				break;
			case COLOR_E_RGBA:
				nBitCount = 32;
				break;
			default:
				assert(! "GetBitCount不支持的格式");
			}

			return nBitCount;
		}

		inline int GetPictureSize(const PICTURE_HEADER& pH)
		{
			int nByteSize = 0;
			switch (pH.eColor)
			{
			case COLOR_E_YV12:
			case COLOR_E_I420:
				nByteSize = pH.y_stride * pH.h + pH.uv_stride * pH.h / 2 + pH.uv_stride * pH.h / 2;
                nByteSize = Align16Bytes(nByteSize);
				break;
			case COLOR_E_YUY2:
			case COLOR_E_UYVY:
			case COLOR_E_RGB565:
			case COLOR_E_RGB24:
			case COLOR_E_RGB32:
			case COLOR_E_RGBA:
				nByteSize = pH.y_stride * pH.h;
				break;
			default:
				assert(! "GetBitCount不支持的格式");
			}

			return nByteSize;
		}

		// 视频数据对象
		class PictureRaw
		{
		public:
			typedef PictureRaw SelfType;
			typedef boost::shared_ptr<PictureRaw> SPtr;

			PictureRaw();
			~PictureRaw();

			inline const PICTURE_HEADER& header() const { return m_header; }
			inline const BYTE* data() const { return m_buf.data(); }
			inline BYTE* data() { return m_buf.data(); }
			size_t size() const	{ return m_header.isValid() ? GetPictureSize(m_header) : 0; }
			inline const BYTE* rgb() const;
			inline const BYTE* yuv() const;
			inline const BYTE* y() const;
			inline const BYTE* u() const;
			inline const BYTE* v() const;

			/**
			 *	@name			allocData
			 *	@brief			根据参数_header申请用于保存图片数据的内存空间
			 *					用户可提供allocator指定内存申请以及释放使用的函数，如果未指定，则默认使用BigBufferManager中提供的共享内存管理方法
			 *	@param[in]		const PICTURE_HEADER & _header 图片的属性
			 *	@param[in]		const MemoryAllocator & allocator 内存申请释放对象
			 *	@return			bool true--成功  false--失败，可能_header指定的图片属性不正确，或者申请内存失败
			 **/
			bool allocData(const PICTURE_HEADER& _header, const MemoryAllocator& allocator = MemoryAllocator());
			/**
			 *	@name			attachData
			 *	@brief			将当前对象与已申请的内存绑定
			 *					当前对象被释放时，不会释放绑定的内存，用户需要自己负责释放内存
			 *					此处绑定是不坚持数据是否为空以及数据的长度是否符合_header指定的图片属性所需要的数据长度
			 *					若当前对象已有数据（申请的或者绑定的）则之前的数据将不再引用，申请的将被释放，绑定的将不再绑定
			 *	@param[in]		BYTE * pData 用户内存块首地址
			 *	@param[in]		size_t len 内存块大小
			 *	@param[in]		const PICTURE_HEADER & _header 图片属性
			 **/
			bool attachData(BYTE* pData, size_t len, const PICTURE_HEADER& _header);

			void freeData();

			void setTimestamp(int64_t ts) { m_timestamp = ts; }
			int64_t getTimestamp() const { return m_timestamp; }

		private:
			PictureRaw(const PictureRaw& robj);
			PictureRaw& operator=(const PictureRaw& robj);

			PICTURE_HEADER m_header;
            MediaBuffer m_buf;
            uint32_t m_timestamp;
		};

        const BYTE* PictureRaw::rgb() const
		{
			if(!m_header.isValid()) return NULL;
			switch (m_header.eColor)
			{
			case COLOR_E_YV12:
			case COLOR_E_I420:
			case COLOR_E_YUY2:
			case COLOR_E_UYVY:
				return NULL;
			case COLOR_E_RGB565:
			case COLOR_E_RGB24:
			case COLOR_E_RGB32:
			case COLOR_E_RGBA:
				return m_buf.data();
			default:
				assert(! "不支持的像素格式");
				return NULL;
			}
		}

		const BYTE* PictureRaw::yuv() const
		{
			if(!m_header.isValid()) return NULL;
			switch (m_header.eColor)
			{
			case COLOR_E_YV12:
			case COLOR_E_I420:
			case COLOR_E_YUY2:
			case COLOR_E_UYVY:
				return m_buf.data();
			case COLOR_E_RGB565:
			case COLOR_E_RGB24:
			case COLOR_E_RGB32:
			case COLOR_E_RGBA:
				return NULL;
			default:
				assert(! "不支持的像素格式");
				return NULL;
			}
		}

		const BYTE* PictureRaw::y() const
		{
			if(!m_header.isValid()) return NULL;
			switch (m_header.eColor)
			{
			case COLOR_E_YV12:
			case COLOR_E_I420:
				return m_buf.data();
			case COLOR_E_YUY2:
			case COLOR_E_UYVY:
				return NULL;
			case COLOR_E_RGB565:
			case COLOR_E_RGB24:
			case COLOR_E_RGB32:
			case COLOR_E_RGBA:
				return NULL;
			default:
				assert(! "不支持的像素格式");
				return NULL;
			}
		}

		const BYTE* PictureRaw::u() const
		{
			if(!m_header.isValid()) return NULL;
			switch (m_header.eColor)
			{
			case COLOR_E_YV12:
				//yyyy vv uu
				return m_buf.data() + m_header.y_stride*m_header.h + m_header.uv_stride * (m_header.h >> 1);
			case COLOR_E_I420:
				//yyyy uu vv
				return m_buf.data() + m_header.y_stride*m_header.h;
			case COLOR_E_YUY2:
			case COLOR_E_UYVY:
				return NULL;
			case COLOR_E_RGB565:
			case COLOR_E_RGB24:
			case COLOR_E_RGB32:
			case COLOR_E_RGBA:
				return NULL;
			default:
				assert(! "不支持的像素格式");
				return NULL;
			}
		}

		const BYTE* PictureRaw::v() const
		{
			if(!m_header.isValid()) return NULL;
			switch (m_header.eColor)
			{
			case COLOR_E_YV12:
				//yyyy vv uu
				return m_buf.data() + m_header.y_stride*m_header.h;
			case COLOR_E_I420:
				return m_buf.data() + m_header.y_stride*m_header.h + m_header.uv_stride * (m_header.h >> 1);
			case COLOR_E_YUY2:
			case COLOR_E_UYVY:
				return NULL;
			case COLOR_E_RGB565:
			case COLOR_E_RGB24:
			case COLOR_E_RGB32:
			case COLOR_E_RGBA:
				return NULL;
			default:
				assert(! "不支持的像素格式");
				return NULL;
			}
		}

}//namespace zMedia

#endif //_MEDIA_FILTER_PICTURE_INFO_H_
