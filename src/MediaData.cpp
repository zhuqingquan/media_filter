#include "MediaData.h"
#include "mediafilter.h"

using namespace zMedia;

MediaBuffer::MediaBuffer()
    : m_data(NULL), m_bufLen(0)
    , m_type(0)
    , m_isNeedFree(false)
{

}

MediaBuffer::MediaBuffer(size_t length, const MemoryAllocator& allocator /*= MemoryAllocator()*/)
    : m_data(NULL), m_bufLen(0)
    , m_type(0)
    , m_isNeedFree(false)
{
    malloc(length, allocator);
}

MediaBuffer::MediaBuffer(BYTE* pData, size_t len)
    : m_data(NULL), m_bufLen(0)
    , m_type(0)
    , m_isNeedFree(false)
{
    attachData(pData, len);
}

MediaBuffer::~MediaBuffer()
{
    free();
}

/*virtual*/ size_t MediaBuffer::malloc(size_t length, const MemoryAllocator& allocator/* = MemoryAllocator()*/)
{
	if(m_data && m_bufLen>0 && m_isNeedFree)
	{
		free();
	}
	if(length<=0)
	{
		return 0;
	}
	size_t realLen = Align32Bytes(length);
	malloc_func mf = allocator.malloc_function() ? allocator.malloc_function() : (::malloc);
	BYTE* pdata = (BYTE*)mf(realLen);
	assert(pdata);
	if(!pdata)	return 0;
	m_data = pdata;
	m_bufLen = length;
	m_isNeedFree = true;
	m_allocator = allocator;
	return m_bufLen;
}

/*virtual*/ size_t MediaBuffer::free()
{
	if(!m_isNeedFree)
		return 0;
	if(m_data==0 || m_bufLen<=0)
		return 0;
	free_func ff = m_allocator.free_function() ? m_allocator.free_function() : (::free);
	ff(m_data);
	m_data = NULL;
	m_bufLen = 0;
	m_isNeedFree = false;
	m_allocator = MemoryAllocator();
	return m_bufLen;
}

/*virtual*/bool MediaBuffer::attachData(BYTE* pData, size_t len)
{
	if(pData==NULL || len<=0)
		return false;
	this->free();
	m_data = pData;
	m_bufLen = len;
	m_isNeedFree = false;
	m_allocator = MemoryAllocator();
	return true;
}
