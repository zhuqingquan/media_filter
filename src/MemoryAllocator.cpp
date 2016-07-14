
#include "MemoryAllocator.h"

bool zMedia::operator==(const zMedia::MemoryAllocator& lobj, const zMedia::MemoryAllocator& robj) 
{
    return lobj.m_MallocFunc==robj.m_MallocFunc && lobj.m_FreeFunc==robj.m_FreeFunc;
}
