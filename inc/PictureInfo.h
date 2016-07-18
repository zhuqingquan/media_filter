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
		//ɫ�ʿռ� define
		enum E_COLORSPACE
		{
			COLOR_E_NONE = 0,
			COLOR_E_YV12,  // yvu yvu
			COLOR_E_I420,	//��ͬFOURCC_IYUV  //yuv yuv
			COLOR_E_YUY2,	//��ͬFOURCC_YUYV
			COLOR_E_UYVY,
			COLOR_E_RGB565,   
			COLOR_E_RGB24,
			COLOR_E_RGB32,  //BGRA BGRA
			COLOR_E_RGBA,   //BGRA BGRA
			COLOR_E_MAX,
		};

		//�������ص��ֽڸ���
		//�����E_COLORSPACEʹ�ã��� PixelBytesCount[COLOR_E_RGB32], ��E_COLORSPACE���޸ĵ���PixelBytesCount[n]������Ӧͬ���޸�PixelBytesCount
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
				//���֧��4K
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
			int uv_stride;//����ƽ���ʽ�õ���������
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
				assert(! "GetBitCount��֧�ֵĸ�ʽ");
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
				assert(! "GetBitCount��֧�ֵĸ�ʽ");
			}

			return nByteSize;
		}

		// ��Ƶ���ݶ���
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
			 *	@brief			���ݲ���_header�������ڱ���ͼƬ���ݵ��ڴ�ռ�
			 *					�û����ṩallocatorָ���ڴ������Լ��ͷ�ʹ�õĺ��������δָ������Ĭ��ʹ��BigBufferManager���ṩ�Ĺ����ڴ������
			 *	@param[in]		const PICTURE_HEADER & _header ͼƬ������
			 *	@param[in]		const MemoryAllocator & allocator �ڴ������ͷŶ���
			 *	@return			bool true--�ɹ�  false--ʧ�ܣ�����_headerָ����ͼƬ���Բ���ȷ�����������ڴ�ʧ��
			 **/
			bool allocData(const PICTURE_HEADER& _header, const MemoryAllocator& allocator = MemoryAllocator());
			/**
			 *	@name			attachData
			 *	@brief			����ǰ��������������ڴ��
			 *					��ǰ�����ͷ�ʱ�������ͷŰ󶨵��ڴ棬�û���Ҫ�Լ������ͷ��ڴ�
			 *					�˴����ǲ���������Ƿ�Ϊ���Լ����ݵĳ����Ƿ����_headerָ����ͼƬ��������Ҫ�����ݳ���
			 *					����ǰ�����������ݣ�����Ļ��߰󶨵ģ���֮ǰ�����ݽ��������ã�����Ľ����ͷţ��󶨵Ľ����ٰ�
			 *	@param[in]		BYTE * pData �û��ڴ���׵�ַ
			 *	@param[in]		size_t len �ڴ���С
			 *	@param[in]		const PICTURE_HEADER & _header ͼƬ����
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
				assert(! "��֧�ֵ����ظ�ʽ");
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
				assert(! "��֧�ֵ����ظ�ʽ");
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
				assert(! "��֧�ֵ����ظ�ʽ");
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
				assert(! "��֧�ֵ����ظ�ʽ");
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
				assert(! "��֧�ֵ����ظ�ʽ");
				return NULL;
			}
		}

}//namespace zMedia

#endif //_MEDIA_FILTER_PICTURE_INFO_H_
