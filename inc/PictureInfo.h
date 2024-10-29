//#pragma once
#ifndef _MEDIA_FILTER_PICTURE_RAW_H_
#define _MEDIA_FILTER_PICTURE_RAW_H_

#include <assert.h>
#include <limits.h>
#include "MediaBuffer.h"
#include "fourcc.h"
#include "mediafilter.h"


namespace zMedia
{
	// YUV的颜色空间标准
	enum VideoColorSpace {
		VIDEO_CS_DEFAULT,
		VIDEO_CS_601,
		VIDEO_CS_709,
	};

	// YUV取值区间
	enum video_range_type {
		VIDEO_RANGE_DEFAULT,
		VIDEO_RANGE_PARTIAL,
		VIDEO_RANGE_FULL
	};

		//色彩空间 define
		enum E_PIXFMT
		{
			PIXFMT_E_NONE = 0,
			PIXFMT_E_YV12,  // yvu yvu
			PIXFMT_E_I420,	//等同FOURCC_IYUV  //yuv yuv
			PIXFMT_E_NV12,	// NV12, Y UV
			PIXFMT_E_YUY2,	//等同FOURCC_YUYV
			PIXFMT_E_UYVY,
			PIXFMT_E_RGB565,   
			PIXFMT_E_RGB24,  //BGR
			PIXFMT_E_RGB32,  //BGRA BGRA
			PIXFMT_E_RGBA,   //RGBA
            PIXFMT_E_BGRA,   //BGRA
            PIXFMT_E_R8,        //  R 8bit for uint8_t data
			PIXFMT_E_MAX,
			PIXFMT_E_I420A = 101,	//等同I420, 并附加了透明通道, 是一种自定义的非标准格式，这种格式还未在PICTURE_FORMAT中支持计算器图片内存大小
		};

	extern int32_t PixfmtToFOURCC(E_PIXFMT pixfmt);
	extern E_PIXFMT FOURCCToPixfmt(int32_t fourcc);

		//单个像素的字节个数
		//需配合E_PIXFMT使用，如 PixelBytesCount[PIXFMT_E_RGB32], 若E_PIXFMT的修改导致PixelBytesCount[n]出错，则应同步修改PixelBytesCount
		static const float PixelBytesCount[] = {0, 1.5, 1.5, 1.5, 2, 2, 2, 3, 4, 4, 4, 1, 4};

		struct PICTURE_FORMAT
		{
			PICTURE_FORMAT()
                : w(0),h(0),ePixfmt(PIXFMT_E_NONE)
                , stride(0),u_stride(0), v_stride(0), a_stride(0)
            {}

			PICTURE_FORMAT(int width, int height, E_PIXFMT pixfmt)
				: w(width), h(height), ePixfmt(pixfmt)
				, stride(0), u_stride(0), v_stride(0), a_stride(0)
			{
				switch(ePixfmt)
				{
				case PIXFMT_E_YUY2:
				case PIXFMT_E_UYVY:
				case PIXFMT_E_RGB565:
				case PIXFMT_E_RGB24:
				case PIXFMT_E_RGB32:
				case PIXFMT_E_RGBA:
                case PIXFMT_E_BGRA:
                case PIXFMT_E_R8:
				case PIXFMT_E_MAX:
					y_stride =Align16Bytes((int) (w * PixelBytesCount[ePixfmt]));
					break;
				case PIXFMT_E_YV12:
				case PIXFMT_E_I420:
					y_stride = Align16Bytes(w);
					u_stride = Align16Bytes(w>>1);//w/2
					v_stride = Align16Bytes(w>>1);//w/2
					break;
				case PIXFMT_E_NV12:
					// NV12数据时uv planar的stride使用u_stride保存，v_stride无效
					y_stride = Align16Bytes(w);
					u_stride = Align16Bytes(w);
					v_stride = 0;
					break;
				default:
					break;
				}
			}

			PICTURE_FORMAT(int width, int height, E_PIXFMT pixfmt,
                            int _stride, int _uStride, int _vStride, int _aStride)
				: w(width), h(height), ePixfmt(pixfmt)
				, stride(_stride), u_stride(_uStride), v_stride(_vStride), a_stride(_aStride)
			{
			}

			//PICTURE_FORMAT(int width, int height, E_PIXFMT pixfmt, 
            //                int _stride, int _uStride, int _vStride, int _aStride,
            //                unsigned int _pts)
			//	: w(width), h(height), ePixfmt(pixfmt)
			//	, stride(_stride), u_stride(_uStride), v_stride(_vStride), a_stride(_aStride)
			//{
			//}

			bool isValid() const 
			{ 
				if(w<=0 || h<=0)	return false;
				switch(ePixfmt)
				{
				case PIXFMT_E_NONE:
					return false;
				case PIXFMT_E_YUY2:
				case PIXFMT_E_UYVY:
				case PIXFMT_E_RGB565:
				case PIXFMT_E_RGB24:
				case PIXFMT_E_RGB32:
				case PIXFMT_E_RGBA:
                case PIXFMT_E_BGRA:
                case PIXFMT_E_R8:
				case PIXFMT_E_MAX:
					return y_stride>0;
				case PIXFMT_E_YV12:
				case PIXFMT_E_I420:
					return y_stride>0 && u_stride>0 && v_stride>0;
				case PIXFMT_E_NV12:
					return y_stride > 0 && u_stride > 0;
				default:
					return false;
				}
			}

			/**
			 *	判断每行数据的后面是否存在padding数据
			 **/
			bool hasPadding() const 
			{ 
				switch (ePixfmt)
				{
				case PIXFMT_E_YUY2:
				case PIXFMT_E_UYVY:
				case PIXFMT_E_RGB565:
				case PIXFMT_E_RGB24:
				case PIXFMT_E_RGB32:
				case PIXFMT_E_RGBA:
                case PIXFMT_E_BGRA:
                case PIXFMT_E_R8:
				case PIXFMT_E_MAX:
					return stride != w * PixelBytesCount[ePixfmt];		// stride不等于w * BytesPerPixel
				case PIXFMT_E_YV12:
				case PIXFMT_E_I420:
					return y_stride != w || u_stride != (w >> 1) || v_stride != (w >> 1);
				case PIXFMT_E_NV12:
					// NV12数据时uv planar的stride使用u_stride保存，v_stride无效
					return y_stride != w || u_stride != (w >> 1);
				default:
					break;
				}
				return false;
			}

            bool operator==(const PICTURE_FORMAT& fmt) const
            {
                if(w!=fmt.w || h!=fmt.h 
                    || stride!=fmt.stride || u_stride!=fmt.u_stride || v_stride!=fmt.v_stride
                    || ePixfmt!=fmt.ePixfmt)
                    return false;
                return true;
            }

			int w;
			int h;
			E_PIXFMT ePixfmt;
            VideoColorSpace colorspace = VIDEO_CS_DEFAULT;
			union {
				int stride;
				int y_stride;
			};
			int u_stride;//仅仅平面格式用到，否则不用
			int v_stride;//仅仅平面格式用到，否则不用
			int a_stride;//仅仅平面格式用到，否则不用,when pixel format need A planer, we need this.
            //uint64_t pts = 0;//after decoded we get pts.Not need dts in PICTURE after decoded.
			
			bool isDirectionDownToUp = false;  //图片存在内存中的方向 true: down to up, //false: up to down.!
		};//struct PICTURE_FORMAT

		inline int GetBitCount(E_PIXFMT ePixfmt)
		{
			int nBitCount = 0;
			switch (ePixfmt)
			{
			case PIXFMT_E_YV12:
			case PIXFMT_E_I420:
				nBitCount = 12;
				break;
			case PIXFMT_E_YUY2:
			case PIXFMT_E_UYVY:
			case PIXFMT_E_RGB565:
				nBitCount = 16;
				break;	
			case PIXFMT_E_RGB24:
				nBitCount = 24;
				break;		
			case PIXFMT_E_RGB32:
				nBitCount = 32;
				break;
			case PIXFMT_E_RGBA:
			case PIXFMT_E_BGRA:
				nBitCount = 32;
				break;
            case PIXFMT_E_R8:
                nBitCount = 8;
                break;
			default:
				assert(! "GetBitCount not support this pixel format");
			}

			return nBitCount;
		}

		inline int GetPictureSize(const PICTURE_FORMAT& pH)
		{
			int nByteSize = 0;
			switch (pH.ePixfmt)
			{
			case PIXFMT_E_YV12:
			case PIXFMT_E_I420:
            {
                int heightHalf = (pH.h + 1) >> 1;
                nByteSize = pH.y_stride * pH.h + pH.u_stride * heightHalf + pH.v_stride * heightHalf;
                //nByteSize = Align16Bytes(nByteSize);
				break;
            }
			case PIXFMT_E_YUY2:
			case PIXFMT_E_UYVY:
			case PIXFMT_E_RGB565:
			case PIXFMT_E_RGB24:
			case PIXFMT_E_RGB32:
			case PIXFMT_E_RGBA:
            case PIXFMT_E_BGRA:
            case PIXFMT_E_R8:
				nByteSize = pH.y_stride * pH.h;
                //nByteSize = Align16Bytes(nByteSize);
				break;
			default:
				assert(! "GetBitCount不支持的格式");
			}

			return nByteSize;
		}

	/**
	 * @brief 编码后的视频帧的类型
	 */
	enum EncVideoFrameType
    {
        Frame_UNKNOW      = 0xFF,
        Frame_I     = 0,
        Frame_P     = 1,
        Frame_B     = 2,
        Frame_PSEI  = 3,        // 0 - 3 is same with YY video packet's frame type.
        Frame_IDR   = 4,
        Frame_SPS   = 32,       // SPS，此处使用32与前面的枚举值在数值上隔离开，用户可以通过或操作判断数据是否同时包含SPS与PPS，比如 val | Frame_SPS 为真，则包含SPS
        Frame_PPS   = 64,       // PPS，此处使用32与前面的枚举值在数值上隔离开，用户可以通过或操作判断数据是否同时包含SPS与PPS，比如 val | Frame_SPS 为真，则包含PPS
        Frame_HEVCHeader= 7,
	};
}//namespace zMedia

#endif //_MEDIA_FILTER_PICTURE_INFO_H_
