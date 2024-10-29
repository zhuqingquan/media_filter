#include "libyuv.h"
#include "libyuv/scale_rgb.h"
#include "libyuv/scale_argb.h"
#include "RawPicture.h"
#include <algorithm>
#include <string.h>

using namespace zMedia;

int32_t zMedia::PixfmtToFOURCC(E_PIXFMT pixfmt)
{
	switch (pixfmt)
	{
	case PIXFMT_E_YUY2:
		return FOURCC_YUY2;
	case PIXFMT_E_UYVY:
		return FOURCC_UYVY;
	case PIXFMT_E_RGB24:
		return FOURCC_RGB24;
	case PIXFMT_E_RGB32:
		return FOURCC_RGB32;
	case PIXFMT_E_RGBA:
		return FOURCC_RGBA;
	case PIXFMT_E_BGRA:
		return FOURCC_BGRA;
	case PIXFMT_E_YV12:
		return FOURCC_YV12;
	case PIXFMT_E_I420:
		return FOURCC_I420;
	case PIXFMT_E_NV12:
		return FOURCC_NV12;
	case PIXFMT_E_MAX:
	case PIXFMT_E_RGB565:
	default:
		break;
	}
	return FOURCC_Unknown;
}

E_PIXFMT zMedia::FOURCCToPixfmt(int32_t fourcc)
{
	switch (fourcc)
	{
	case FOURCC_YUY2:
		return PIXFMT_E_YUY2;
	case FOURCC_UYVY:
		return PIXFMT_E_UYVY;
	case FOURCC_RGB24:
		return PIXFMT_E_RGB24;
	case FOURCC_RGB32:
		return PIXFMT_E_RGB32;
	case FOURCC_RGBA:
		return PIXFMT_E_RGBA;
	case FOURCC_BGRA:
		return PIXFMT_E_BGRA;
	case FOURCC_YV12:
		return PIXFMT_E_YV12;
	case FOURCC_I420:
		return PIXFMT_E_I420;
	case FOURCC_NV12:
		return PIXFMT_E_NV12;
	case PIXFMT_E_MAX:
	case PIXFMT_E_RGB565:
	default:
		break;
	}
	return PIXFMT_E_NONE;
}

PictureRaw::PictureRaw()
{
}

PictureRaw::~PictureRaw(void)
{
}

bool PictureRaw::allocData( const PICTURE_FORMAT& _format, const MemoryAllocator& allocator /*= MemoryAllocator()*/ )
{
    if(!_format.isValid())	return false;
    int picRealSize = GetPictureSize(_format);
    size_t totalMallocSize = Align16Bytes(picRealSize);
    if(totalMallocSize!=m_buf.malloc(totalMallocSize, allocator))
        return false;

    m_format = _format;
    return m_buf.data()!=NULL;			
}

bool PictureRaw::attachData( uint8_t* pData, size_t len, const PICTURE_FORMAT& _format, const MemoryAllocator& allocator/* = MemoryAllocator()*/)
{
    if(!_format.isValid())	return false;
    if(!m_buf.attachData(pData, len, allocator))
        return false;

    m_format = _format;
    return true;
}

bool PictureRaw::attachData(const PICTURE_FORMAT& _format, const TextureHolder::SPtr texture)
{
    if(!_format.isValid())	return false;
    m_texture = texture;
    m_format = _format;
    return true;
}

void PictureRaw::freeData()
{
    m_buf.free();
    m_texture.reset();
    m_format = zMedia::PICTURE_FORMAT();
}

bool PictureRaw::copyData(uint8_t** src, int* src_stride, int x, int y, int w, int h)
{
    if(data()==nullptr) return false;
    #ifdef _WIN32
    int real_w = min(w, m_format.w);
    int real_h = min(h, m_format.h);
#else
    int real_w = std::min(w, m_format.w);
    int real_h = std::min(h, m_format.h);
#endif
    switch (m_format.ePixfmt)
    {
    case PIXFMT_E_I420:
    {
        const uint8_t* psrc = nullptr;
        uint8_t* pdst = nullptr;
        // copy Y
        psrc = src[0] + (y * src_stride[0] + x);
        pdst = (uint8_t*)this->y();
        libyuv::CopyPlane(psrc, src_stride[0], pdst, m_format.y_stride, real_w, real_h);
        //for(int i=0; i<real_h; i++)
        //{
        //    memcpy(pdst, psrc, real_w);
        //    psrc += src_stride[0];
        //    pdst += this->m_format.y_stride;
        //}
        // copy U
        psrc = src[1] + ((y >> 1) * src_stride[1] + (x >> 1));
        pdst = (uint8_t*)this->u();
        int h_half = real_h / 2;
        int w_half = real_w / 2;
        libyuv::CopyPlane(psrc, src_stride[1], pdst, m_format.u_stride, w_half, h_half);
        //for(int i=0; i<h_half; i++)
        //{
        //    memcpy(pdst, psrc, w_half);
        //    psrc += src_stride[1];
        //    pdst += this->m_format.u_stride;
        //}
        // copy V
        psrc = src[2] + ((y >> 1) * src_stride[2] + (x >> 1));
        pdst = (uint8_t*)this->v();
        libyuv::CopyPlane(psrc, src_stride[2], pdst, m_format.v_stride, w_half, h_half);
        //for(int i=0; i<h_half; i++)
        //{
        //    memcpy(pdst, psrc, w_half);
        //    psrc += src_stride[2];
        //    pdst += this->m_format.v_stride;
        //}
        return true;
    }
    case PIXFMT_E_YV12:
    case PIXFMT_E_YUY2:
    case PIXFMT_E_UYVY:
    case PIXFMT_E_NV12:
        return false;
    case PIXFMT_E_RGB565:
    case PIXFMT_E_RGB24:
    case PIXFMT_E_RGB32:
    case PIXFMT_E_RGBA:
    case PIXFMT_E_BGRA:
    case PIXFMT_E_R8:
        return true;
    default:
        assert(! "不支持的像素格式");
        return false;
    }
}

bool PictureRaw::maskAlpha()
{
    if(m_alphaMask==nullptr || m_alphaMask->data()==nullptr)
        return false;
    const PICTURE_FORMAT& fmt = m_alphaMask->format();
    if(fmt.ePixfmt != PIXFMT_E_RGBA && fmt.ePixfmt!=PIXFMT_E_BGRA)
        return false;
    if(m_format.ePixfmt != PIXFMT_E_RGBA && m_format.ePixfmt!=PIXFMT_E_BGRA)
        return false;
    if(fmt.w!=m_format.w || fmt.h!=m_format.h)
        return false;
    uint8_t* psrcLine = this->data();
    uint8_t* pAlphaLine = m_alphaMask->data();
    for(int y=0; y<fmt.h; y++)
    {
        uint8_t* pixSrc = psrcLine;
        uint8_t* pixAlpha = pAlphaLine;
        for(int i=0; i<fmt.w; i++)
        {
            pixSrc[3] = pixAlpha[1];
            pixSrc +=4;
            pixAlpha +=4;
        }

        psrcLine += m_format.stride;
        pAlphaLine += fmt.stride;
    }
    return true;
} 

bool PictureRaw::addOverlay(const PictureRaw::Overlay& overlay)
{
    if(overlay.overlay==nullptr)
        return false;
    if ((overlay.left + overlay.overlay->format().w) < 0 || overlay.left > m_format.w
        || (overlay.top + overlay.overlay->format().h) < 0 || overlay.top > m_format.h)
        return false;
    m_overlays.push_back(overlay);
    return true;
}

bool PictureRaw::removeOverlay(const PictureRaw::Overlay& overlay)
{
    auto iter = m_overlays.begin();
    for ( ; iter!=m_overlays.end(); iter++)
    {
        if (overlay.left == iter->left && overlay.top == iter->top && overlay.overlay == iter->overlay)
        {
            m_overlays.erase(iter);
            return true;
        }
    }
    return false;
}

PictureRaw::SPtr zMedia::copySubImage(const PictureRaw::SPtr& src, int x, int y, int w, int h)
{
    if(src==nullptr || !src->format().isValid() || src->data()==nullptr)
        return nullptr;
    if(x<0 || y<0 || x >= src->format().w || y >= src->format().h)
        return nullptr;
    if( x + w >= src->format().w || y + h >= src->format().h )
        return nullptr;
    zMedia::PictureRaw::SPtr result = std::make_shared<zMedia::PictureRaw>();
    result->allocData(zMedia::PICTURE_FORMAT(w, h, src->format().ePixfmt));
    if(result->data()==nullptr)
        return nullptr;
    switch(src->format().ePixfmt)
    {
    case PIXFMT_E_I420:
    {
        const uint8_t* psrcY = src->y() + (y * src->format().y_stride + x);
        const uint8_t* psrcU = src->u() + (((y + 1) >> 1) * src->format().u_stride + ((x+1)>>1));
        const uint8_t* psrcV = src->v() + (((y + 1) >> 1) * src->format().v_stride + ((x+1)>>1));
        uint8_t* dst_y = (uint8_t*)result->y();
        uint8_t* dst_u = (uint8_t*)result->u();
        uint8_t* dst_v = (uint8_t*)result->v();
        libyuv::I420Copy(psrcY, src->format().y_stride, psrcU, src->format().u_stride, psrcV, src->format().v_stride,
                            dst_y, result->format().y_stride, dst_u, result->format().u_stride, dst_v, result->format().v_stride, w, h);
        break;
    }
    case PIXFMT_E_BGRA:
    case PIXFMT_E_RGBA:
    case PIXFMT_E_RGB32:
    {
        const uint8_t* src_rgb = src->rgb() + (y * src->format().stride + (int)(x * zMedia::PixelBytesCount[src->format().ePixfmt]));
        uint8_t* dst_rgb = (uint8_t*)result->rgb();
        libyuv::ARGBCopy(src_rgb, src->format().stride, dst_rgb, result->format().stride, w, h);
        break;
    }
    default:
        return nullptr;
    }
    return result;
}

PictureRaw::SPtr zMedia::scaleImage(const PictureRaw::SPtr& src, int w, int h)
{
    if(src==nullptr || !src->format().isValid() || src->data()==nullptr)
        return nullptr;
    if(w <=0 || h <= 0)
        return nullptr;
    zMedia::PictureRaw::SPtr result = std::make_shared<zMedia::PictureRaw>();
    result->allocData(zMedia::PICTURE_FORMAT(w, h, src->format().ePixfmt));
    if(result->data()==nullptr)
        return nullptr;
    switch(src->format().ePixfmt)
    {
    case PIXFMT_E_I420:
    {
        libyuv::I420Scale(src->y(), src->format().y_stride, src->u(), src->format().u_stride, src->v(), src->format().v_stride, src->format().w, src->format().h,
                    (uint8_t*)result->y(), result->format().y_stride, (uint8_t*)result->u(), result->format().u_stride, (uint8_t*)result->v(), result->format().v_stride,
                    w, h, libyuv::FilterMode::kFilterNone);
        break;
    }
    case PIXFMT_E_RGB24:
        libyuv::RGBScale(src->rgb(), src->format().stride, src->format().w, src->format().h,
            (uint8_t*)result->rgb(), result->format().stride, result->format().w, result->format().h, libyuv::kFilterNone);
        break;
    case PIXFMT_E_RGBA:
    case PIXFMT_E_BGRA:
    case PIXFMT_E_RGB32:
        libyuv::ARGBScale(src->rgb(), src->format().stride, src->format().w, src->format().h,
            (uint8_t*)result->rgb(), result->format().stride, result->format().w, result->format().h, libyuv::kFilterNone);
        break;
    default:
        return nullptr;
    }
    return result;
}

PictureRaw::SPtr convertToRGB24(const PictureRaw::SPtr& src, bool needPadding = true)
{
    if(src==nullptr || src->data()==nullptr || !src->format().isValid())
        return nullptr;
    zMedia::PictureRaw::SPtr result = std::make_shared<zMedia::PictureRaw>();
    if(needPadding)
        result->allocData(zMedia::PICTURE_FORMAT(src->format().w, src->format().h, PIXFMT_E_RGB24));
    else
        result->allocData(zMedia::PICTURE_FORMAT(src->format().w, src->format().h, PIXFMT_E_RGB24, src->format().w * 3, 0, 0, 0));
    if(result->data()==nullptr)
        return nullptr;
    switch (src->format().ePixfmt)
    {
    case PIXFMT_E_I420:
        libyuv::I420ToRAW(src->y(), src->format().y_stride, src->u(), src->format().u_stride, src->v(), src->format().v_stride,
            (uint8_t*)result->rgb(), result->format().stride, src->format().w, src->format().h);
        break;
    
    default:
        return nullptr;
    }
    return result;
}

PictureRaw::SPtr convertToI420P(const PictureRaw::SPtr& src, bool needPadding = true)
{
    if(src==nullptr || src->data()==nullptr || !src->format().isValid())
        return nullptr;
    zMedia::PictureRaw::SPtr result = std::make_shared<zMedia::PictureRaw>();
    result->allocData(zMedia::PICTURE_FORMAT(src->format().w, src->format().h, PIXFMT_E_I420));
    if(result->data()==nullptr)
        return nullptr;
    switch (src->format().ePixfmt)
    {
    case PIXFMT_E_RGB24:
        libyuv::RAWToI420(src->rgb(), src->format().stride, 
                            (uint8_t*)result->y(), result->format().y_stride, (uint8_t*)result->u(), result->format().u_stride, (uint8_t*)result->v(), result->format().v_stride, 
                            src->format().w, src->format().h);
        break;
    default:
        return nullptr;
    }
    return result;
}

PictureRaw::SPtr zMedia::convertImage(const PictureRaw::SPtr& src, zMedia::E_PIXFMT pixfmt, bool needPadding /*= true*/)
{
    switch (pixfmt)
    {
    case PIXFMT_E_RGB24:
        return convertToRGB24(src, needPadding);
    case PIXFMT_E_I420:
        return convertToI420P(src, needPadding);
    default:
        return nullptr;
    }
    return nullptr;
}

bool zMedia::dumpToFile_i420(FILE* file, const PictureRaw::SPtr& pic)
{
    if(pic==nullptr)
        return false;
    if(pic->y())
    {
        const uint8_t* pdata = pic->y();
        for(int i=0; i<pic->format().h; i++)
        {
            fwrite(pdata, 1, pic->format().w, file);
            pdata += pic->format().y_stride;
        }
    }
    if(pic->u())
    {
        const uint8_t* pdata = pic->u();
        for(int i=0; i<(pic->format().h+1) / 2; i++)
        {
            fwrite(pdata, 1, (pic->format().w + 1) / 2, file);
            pdata += pic->format().u_stride;
        }
    }
    if(pic->v())
    {
        const uint8_t* pdata = pic->v();
        for(int i=0; i<(pic->format().h+1) / 2; i++)
        {
            fwrite(pdata, 1, (pic->format().w + 1) / 2, file);
            pdata += pic->format().v_stride;
        }
    }
    return true;
}