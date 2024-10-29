#pragma once
#ifndef _Z_MEDIA_TEXTURE_HOLDER_H_
#define _Z_MEDIA_TEXTURE_HOLDER_H_

#include <mutex>
#include <memory>

namespace zMedia
{
    /**
     * 使用此Holder类持有Texture的ID以及相应的mutex对象
     * 这样是为了确保Texture资源可以在多个模块之间传递并且保证多线程访问时的线程安全
     * 但是此Holder只持有Texture而不进行Texture的创建以及释放，因为OpenGL中对创建以及释放Texture的线程有要求。
     * 使用者应该使用TextureHolder::SPtr在不同模块间传递texture，并且只有Texture的创建者创建TextureHolder对象，且在创建者对Texture进行释放之前需要先调用onTextureReleased()方法
     * 这样其他使用这个Texture的线程才是安全的
     **/
    class TextureHolder
    {
    public:
        typedef std::shared_ptr<TextureHolder> SPtr;

        /**
         * 构造函数，将Texture的信息以及访问texture的mutex绑定到当前Holder对象
         **/
        TextureHolder(uint32_t texture, uint32_t target, uint32_t internalFmt, std::mutex* mutex)
            : m_glTextureID(texture), m_glTexTarget(target), m_glTexInternalFmt(internalFmt)
            , m_textureMutex(mutex)
        {
        }

		/**
		 * 构造函数，将Texture的信息以及访问texture的mutex绑定到当前Holder对象
		 **/
		TextureHolder(uint32_t texture, uint32_t target, uint32_t internalFmt, uint32_t pixelUnpackBuffer, std::mutex* mutex)
			: m_glTextureID(texture), m_glTexTarget(target), m_glTexInternalFmt(internalFmt)
			, m_glBuffer(pixelUnpackBuffer), m_textureMutex(mutex)
		{
		}

        /**
         * Texture的创建者通过调用调用该方法将当前对象内的Texture置0
         * 这样其他持有TextureHolder::SPtr的使用者在在此lock时将失败，此时将无法获取到Texture
         **/
        void onTextureReleased()
        {
            if(m_textureMutex!=nullptr)
            {
                m_textureMutex->lock();
                m_mutexLocked = true;
            }
            if(m_glTextureID!=0)
            {
                m_glTextureID = 0;
                m_glTexTarget = 0;
                m_glTexInternalFmt = 0;
            }
            if(m_textureMutex!=nullptr)
            {
                m_textureMutex->unlock();
                m_mutexLocked = false;
            }
            m_textureMutex = nullptr;
        }

        /**
         * @brief       对Texture加锁，保证线程安全。
         * @param       texture 获取加锁成功后的Texture ID
         * @return      返回是否加锁成功。当当前绑定的Texture不合法时将返回失败，其他时候返回成功。
         **/
        bool lock(uint32_t* texture)
        {
            if(m_glTextureID==0)
                return false;
            if(m_textureMutex!=nullptr)
            {
                m_textureMutex->lock();
                m_mutexLocked = true;
            }
            if(texture!=nullptr)
                *texture = m_glTextureID;
            return true;
        }

        /**
         * @brief       解除对Texture的锁
         **/
        void unlock()
        {
            if(m_textureMutex!=nullptr && m_mutexLocked)
            {
                m_textureMutex->unlock();
                m_mutexLocked = false;
            }
        }

        /**
         * @brief       获取绑定的Texture的ID
         * @return      绑定的Texture的ID
         **/
        uint32_t glTextureID() const { return m_glTextureID; }
        /**
         * @brief       获取绑定的Texture的target类型,比如GL_TEXTURE_2D
         * @return      绑定的Texture的target类型
         **/
        uint32_t glTexTarget() const { return m_glTexTarget; }
        /**
         * @brief       获取绑定的Texture的数据格式类型,比如GL_RGBA
         * @return      绑定的Texture的数据格式类型
         **/
        uint32_t glTexInternalFmt() const { return m_glTexInternalFmt; }

		/**
		 * @brief       获取绑定的Texture相关联的buffer
		 * @return      为当前Texture创建的对应的GL_PIXEL_UNPACK_BUFFER类型buffer
		 **/
		uint32_t glBuffer() const { return m_glBuffer; }
    private:
        TextureHolder(const TextureHolder&) = delete;
        TextureHolder& operator=(const TextureHolder&) = delete;

		uint32_t m_glBuffer = 0;		// 如果有为当前Texture创建对应的GL_PIXEL_UNPACK_BUFFER，则此成员不为0
        uint32_t m_glTextureID = 0;
        uint32_t m_glTexTarget = 0;
        uint32_t m_glTexInternalFmt = 0;
        std::mutex* m_textureMutex = nullptr;   // mutex for access texture
        bool m_mutexLocked = false;
    };//class TextureHolder
}//namespace zMedia
#endif//_Z_MEDIA_TEXTURE_HOLDER_H_
