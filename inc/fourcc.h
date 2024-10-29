#ifndef _MEDIA_FILTER_COMMON_FORCC_H_
#define _MEDIA_FILTER_COMMON_FORCC_H_

#ifdef _WIN32
#include <windows.h>
#include <mmiscapi.h>
#include <tchar.h>
#elif __linux__
#define mmioFOURCC(ch0, ch1, ch2, ch3) \
    (((uint32_t)(uint8_t)(ch0)) | ((uint32_t)(uint8_t)(ch1) << 8) |\
    ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24))
#endif

namespace zMedia
{
#define FOURCC_Unknown 0
		//FOURCC define
		/* yuyu		4:2:2 16bit, y-u-y-v, packed*/
#define FOURCC_YUY2	mmioFOURCC('Y','U','Y','2')
#define FOURCC_YUYV	mmioFOURCC('Y','U','Y','V')
		/* uyvy		4:2:2 16bit, u-y-v-y, packed */
#define FOURCC_UYVY	mmioFOURCC('U','Y','V','Y')
		/* i420		y-u-v, planar */
#define FOURCC_I420	mmioFOURCC('I','4','2','0')
#define FOURCC_IYUV	mmioFOURCC('I','Y','U','V')
		/* yv12		y-v-u, planar */
#define FOURCC_YV12	mmioFOURCC('Y','V','1','2')
	/* yv12		Y planer + u-v planer */
#define FOURCC_NV12	mmioFOURCC('N','V','1','2')

#define FOURCC_HDYC mmioFOURCC('H','D','Y','C') //等同于FOURCC_UYVY，但颜色范围不同，先忽略不计
#define FOURCC_BGRA	mmioFOURCC('B','G','R','A')
#define FOURCC_RGBA	mmioFOURCC('R','G','B','A')
#define	FOURCC_RGB24 mmioFOURCC('R', 'G', 'B', '3')
#define	FOURCC_RGB32 mmioFOURCC('R', 'G', 'B', '4')
#define	FOURCC_H264 mmioFOURCC('H', '2', '6', '4')              // H264
#define	FOURCC_AAC_LC mmioFOURCC('A', 'A', 'C', 'L')            // AAC LC
#define	FOURCC_MP3 mmioFOURCC('M', 'P', '3', 'A')            // MP3
#define	FOURCC_MP2 mmioFOURCC('M', 'P', '2', 'A')            // MP3

	inline const char* FourCCToString(int vf)
	{
		switch (vf)
		{
		//case FOURCC_ANY:
		//	return "Any";
		//	break;
		case FOURCC_Unknown:
			return "Unkown";
		//case FOURCC_24BG:
		case FOURCC_RGB24:
			return "RGB24";
		case FOURCC_RGB32:
			return "RGB32";
		//case FOURCC_ARGB:
		//	return "ARGB";
		//	break;
		case FOURCC_I420:
			return "I420";
		case FOURCC_NV12:
			return "NV12";
		case FOURCC_YV12:
			return "YV12";
		//case FOURCC_Y800:
		//	return "Y800";
		//	break;
		case FOURCC_YUY2:
			return "YUY2";
		case FOURCC_UYVY:
			return "UYVY";
		case FOURCC_HDYC:
			return "HDYC";
		//case FOURCC_MJPG:
		//	return "MJPEG";
		//	break;
        case FOURCC_H264:
            return "H264";
        case FOURCC_AAC_LC:
            return "AAC-LC";
		default:
			break;
		}
		return "Unkown";
	}
}

#endif// _MEDIA_FILTER_COMMON_FORCC_H_
