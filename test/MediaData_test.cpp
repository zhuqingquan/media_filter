//#pragma once
#include "gtest/gtest.h"
#include "MediaData.h"

TEST(MediaData, constructor_default)
{
    zMedia::MediaBuffer buf;
    EXPECT_EQ(NULL, buf.data());
    EXPECT_EQ(0, buf.length());
    const zMedia::MemoryAllocator& memAllocator = buf.memAllocator();
    EXPECT_EQ(NULL, memAllocator.malloc_function());
    EXPECT_EQ(NULL, memAllocator.free_function());
    EXPECT_EQ(0, buf.getPayloadOffset());
    EXPECT_EQ(0, buf.getPayloadSize());
}

TEST(MediaData, constructor_length)
{
    //if length<=0 allocate failed, the members in MediaBuffer is not inited
    zMedia::MediaBuffer buf_len_invalid(0);
    EXPECT_EQ(NULL, buf_len_invalid.data());
    EXPECT_EQ(0, buf_len_invalid.length());
    const zMedia::MemoryAllocator& memAllocator_1 = buf_len_invalid.memAllocator();
    EXPECT_EQ(NULL, memAllocator_1.malloc_function());
    EXPECT_EQ(NULL, memAllocator_1.free_function());
    EXPECT_EQ(0, buf_len_invalid.getPayloadOffset());
    EXPECT_EQ(0, buf_len_invalid.getPayloadSize());
    
    zMedia::MediaBuffer buf(1);
    EXPECT_TRUE(NULL!=buf.data());
    EXPECT_EQ(1, buf.length());
    const zMedia::MemoryAllocator& memAllocator = buf.memAllocator();
    EXPECT_EQ(zMedia::MemoryAllocator::std_allocator.malloc_function(), memAllocator.malloc_function());
    EXPECT_EQ(zMedia::MemoryAllocator::std_allocator.free_function(), memAllocator.free_function());
    EXPECT_EQ(0, buf.getPayloadOffset());
    EXPECT_EQ(0, buf.getPayloadSize());
}

TEST(MediaData, constructor_attachData)
{
    //if param is invalid, the members in MediaBuffer is not inited, signed with the default value
    zMedia::MediaBuffer buf_data_null(NULL, 100);
    EXPECT_EQ(NULL, buf_data_null.data());
    EXPECT_EQ(0, buf_data_null.length());
    const zMedia::MemoryAllocator& memAllocator_1 = buf_data_null.memAllocator();
    EXPECT_EQ(NULL, memAllocator_1.malloc_function());
    EXPECT_EQ(NULL, memAllocator_1.free_function());
    EXPECT_EQ(0, buf_data_null.getPayloadOffset());
    EXPECT_EQ(0, buf_data_null.getPayloadSize());

    const char* pdata = "point to a static string.";
    zMedia::MediaBuffer buf_len_invalid((BYTE*)pdata, 0);
    EXPECT_EQ(NULL, buf_len_invalid.data());
    EXPECT_EQ(0, buf_len_invalid.length());
    const zMedia::MemoryAllocator& memAllocator_2 = buf_len_invalid.memAllocator();
    EXPECT_EQ(NULL, memAllocator_2.malloc_function());
    EXPECT_EQ(NULL, memAllocator_2.free_function());
    EXPECT_EQ(0, buf_len_invalid.getPayloadOffset());
    EXPECT_EQ(0, buf_len_invalid.getPayloadSize());

    size_t datalen = 100;
    char* std_malloc_data = (char*)malloc(datalen);
    zMedia::MediaBuffer buf_null_allocator((BYTE*)std_malloc_data, datalen);
    EXPECT_TRUE(NULL!=buf_null_allocator.data());
    EXPECT_EQ(datalen, buf_null_allocator.length());
    const zMedia::MemoryAllocator& memAllocator_3 = buf_null_allocator.memAllocator();
    EXPECT_EQ(NULL, memAllocator_3.malloc_function());
    EXPECT_EQ(NULL, memAllocator_3.free_function());
    //EXPECT_EQ(zMedia::MemoryAllocator::std_allocator.malloc_function(), memAllocator.malloc_function());
    //EXPECT_EQ(zMedia::MemoryAllocator::std_allocator.free_function(), memAllocator.free_function());
    EXPECT_EQ(0, buf_null_allocator.getPayloadOffset());
    EXPECT_EQ(0, buf_null_allocator.getPayloadSize());
    free(std_malloc_data);

    //Use user defined Allocator to malloc and free data
    static int malloced = 0;
    struct TempAlloc
    {
        static void* malloc(size_t len) { malloced = 1; return ::malloc(len); }
        static void free(void* pdata) { malloced = 0; ::free(pdata); }
    };

    {
        std_malloc_data = (char*)TempAlloc::malloc(datalen);
        zMedia::MediaBuffer buf_usr_allocator((BYTE*)std_malloc_data, datalen, zMedia::MemoryAllocator(TempAlloc::malloc, TempAlloc::free));
        EXPECT_EQ(1, malloced);
        EXPECT_TRUE(NULL!=buf_usr_allocator.data());
        EXPECT_EQ(datalen, buf_usr_allocator.length());
        const zMedia::MemoryAllocator& memAllocator_4 = buf_usr_allocator.memAllocator();
        EXPECT_EQ(TempAlloc::malloc, memAllocator_4.malloc_function());
        EXPECT_EQ(TempAlloc::free, memAllocator_4.free_function());
        //EXPECT_EQ(zMedia::MemoryAllocator::std_allocator.malloc_function(), memAllocator.malloc_function());
        //EXPECT_EQ(zMedia::MemoryAllocator::std_allocator.free_function(), memAllocator.free_function());
        EXPECT_EQ(0, buf_usr_allocator.getPayloadOffset());
        EXPECT_EQ(0, buf_usr_allocator.getPayloadSize());
    }
    EXPECT_EQ(0, malloced);
}

TEST(MediaData, malloc_free)
{
    zMedia::MediaBuffer buf_zero;
    //0 is a invalid param
    EXPECT_EQ(0, buf_zero.malloc(0));
    EXPECT_EQ(NULL, buf_zero.data());
    EXPECT_EQ(0, buf_zero.length());
    //malloc normal
    EXPECT_EQ(100, buf_zero.malloc(100));
    EXPECT_TRUE(NULL!= buf_zero.data());
    EXPECT_EQ(100, buf_zero.length());
    //free it
    EXPECT_EQ(100, buf_zero.free()); 
    EXPECT_EQ(NULL, buf_zero.data());
    EXPECT_EQ(0, buf_zero.length());

    zMedia::MediaBuffer buf_init_len(100);
    EXPECT_EQ(100, buf_init_len.length());
    EXPECT_TRUE(NULL!=buf_init_len.data());
    //if MediaBuffer is not null, can not malloc again befor free
    EXPECT_EQ(0, buf_init_len.malloc(100));
    EXPECT_EQ(100, buf_init_len.free()); 
    EXPECT_EQ(NULL, buf_init_len.data());
    EXPECT_EQ(0, buf_init_len.length());
    
    //Use user defined Allocator to malloc and free data
    static int malloced = 0;
    struct TempAlloc
    {
        static void* malloc(size_t len) { malloced = 1; return ::malloc(len); }
        static void free(void* pdata) { malloced = 0; ::free(pdata); }
    };
    zMedia::MediaBuffer buf_user_allocator;
    zMedia::MemoryAllocator user_allocator(TempAlloc::malloc, TempAlloc::free);
    EXPECT_EQ(100, buf_user_allocator.malloc(100, user_allocator));
    EXPECT_EQ(100, buf_user_allocator.length());
    EXPECT_TRUE(NULL!=buf_user_allocator.data());
    BYTE* pdata = buf_user_allocator.data();
    memset(pdata, 0, 100);
    EXPECT_EQ(1, malloced);
    buf_user_allocator.free();
    EXPECT_EQ(0, malloced);
}

TEST(MediaData, attach_free)
{
    int buf_size = 100;
    BYTE* malloced_buf = (BYTE*)malloc(buf_size);

    zMedia::MediaBuffer buf_zero;
    //0 is a invalid param
    EXPECT_FALSE(buf_zero.attachData(NULL, 0));
    EXPECT_FALSE(buf_zero.attachData(NULL, 100));
    EXPECT_FALSE(buf_zero.attachData(malloced_buf, 0));
    EXPECT_EQ(NULL, buf_zero.data());
    EXPECT_EQ(0, buf_zero.length());
    //attach normal
    EXPECT_TRUE(buf_zero.attachData(malloced_buf, buf_size));
    EXPECT_TRUE(NULL!= buf_zero.data());
    EXPECT_EQ(buf_size, buf_zero.length());
    //free it, but the memory is not free
    EXPECT_EQ(buf_size, buf_zero.free()); 
    EXPECT_EQ(NULL, buf_zero.data());
    EXPECT_EQ(0, buf_zero.length());
    memset(malloced_buf, 0, buf_size);

    zMedia::MediaBuffer buf_init_len(100);
    EXPECT_EQ(100, buf_init_len.length());
    EXPECT_TRUE(NULL!=buf_init_len.data());
    //if MediaBuffer is not null, can not malloc again befor free
    EXPECT_FALSE(buf_init_len.attachData(malloced_buf, (100)));
    EXPECT_EQ(100, buf_init_len.free()); 
    EXPECT_EQ(NULL, buf_init_len.data());
    EXPECT_EQ(0, buf_init_len.length());
    free(malloced_buf);
    
    //Use user defined Allocator to malloc and free data
    static int malloced = 0;
    struct TempAlloc
    {
        static void* malloc(size_t len) { malloced = 1; return ::malloc(len); }
        static void free(void* pdata) { malloced = 0; ::free(pdata); }
    };
    BYTE* user_buf = (BYTE*)TempAlloc::malloc(buf_size);
    EXPECT_EQ(1, malloced);
    zMedia::MediaBuffer buf_user_allocator;
    zMedia::MemoryAllocator user_allocator(TempAlloc::malloc, TempAlloc::free);
    EXPECT_TRUE(buf_user_allocator.attachData(user_buf, buf_size, user_allocator));
    EXPECT_EQ(buf_size, buf_user_allocator.length());
    EXPECT_TRUE(NULL!=buf_user_allocator.data());
    BYTE* pdata = buf_user_allocator.data();
    EXPECT_EQ(user_buf, pdata);
    memset(pdata, 0, buf_size);
    buf_user_allocator.free();
    EXPECT_EQ(0, malloced);
}

TEST(MediaData, payload)
{
    zMedia::MediaBuffer buf;
    buf.malloc(256);
    EXPECT_EQ(256, buf.length());
    EXPECT_TRUE(NULL!=buf.data());

    //default payload offset and size is 0
    EXPECT_EQ(0, buf.getPayloadOffset());
    EXPECT_EQ(0, buf.getPayloadSize());

    //if setpayloadsize bigger than the free size, failed.
    EXPECT_FALSE(buf.setPayloadSize(300));

    //memset data and set payload size
    memset(buf.data()+buf.getPayloadOffset(), 'A', buf.length());
    EXPECT_TRUE(buf.setPayloadSize(256));

    //change the offset, the payload size is changed auto.
    //payload size discount 10 bytes
    EXPECT_TRUE(buf.setPayloadOffset(buf.getPayloadOffset()+10));
    EXPECT_EQ(246, buf.getPayloadSize());
    EXPECT_EQ(10, buf.getPayloadOffset());
    //payload size increase 4 bytes
    EXPECT_TRUE(buf.setPayloadOffset(buf.getPayloadOffset()-4));
    EXPECT_EQ(250, buf.getPayloadSize());
    EXPECT_EQ(6, buf.getPayloadOffset());
    //use setpayloadsize to discount data off the buf tail.
    EXPECT_TRUE(buf.setPayloadSize(buf.getPayloadSize()-10));
    EXPECT_EQ(240, buf.getPayloadSize());
    EXPECT_EQ(6, buf.getPayloadOffset());

    //if offset beyond the payload size,failed.
    EXPECT_FALSE(buf.setPayloadOffset(buf.getPayloadOffset()+250));
    EXPECT_EQ(240, buf.getPayloadSize());
    EXPECT_EQ(6, buf.getPayloadOffset());
}
