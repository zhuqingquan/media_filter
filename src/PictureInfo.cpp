#include "PictureInfo.h"

using namespace zMedia;

	PictureRaw::PictureRaw()
	{
	}

	PictureRaw::~PictureRaw(void)
	{
	}

	bool PictureRaw::allocData( const PICTURE_HEADER& _header, const MemoryAllocator& allocator /*= MemoryAllocator()*/ )
	{
		if(!_header.isValid())	return false;
		int picRealSize = GetPictureSize(_header);
		size_t totalMallocSize = picRealSize;//*/(size_t)m_maxPixels;
		if(totalMallocSize!=m_buf.malloc(totalMallocSize, allocator))
			return false;

		m_header = _header;
		return m_buf.data()!=NULL;			
	}

	bool PictureRaw::attachData( BYTE* pData, size_t len, const PICTURE_HEADER& _header )
	{
		if(!_header.isValid())	return false;
		if(!m_buf.attachData(pData, len))
			return false;

		m_header = _header;
        return true;
	}

 	void PictureRaw::freeData()
 	{
        m_buf.free();
 	}

