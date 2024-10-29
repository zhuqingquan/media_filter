/*
 * @Author: zhuqingquan zhuqingquan@52tt.com
 * @Date: 2023-08-08 
 * @Description: meta data of one frame
 **/
#pragma once
#ifndef _Z_MEDIA_FRAME_META_DATA_H_
#define _Z_MEDIA_FRAME_META_DATA_H_

#include <string>
#include <map>

namespace zMedia
{

class FrameMetaData
{
public:
    /**
     * @brief 设置数据。如果key之前未添加则添加key-value键值对，如果key已经存在则更新key对应的value。
     * @param key 
     * @param value 
     */
    void set(const std::string& key, const std::string& value)
    {
        m_datas[key] = value;
    }

    /**
     * @brief 设置数据。metadata中的每项都会添加进去。如果key之前未添加则添加key-value键值对，如果key已经存在则更新key对应的value。
     * @param metadata
     */
    void set(const std::map<std::string, std::string>& metadata)
    {
        for (const auto& iter : metadata)
        {
            m_datas[iter.first] = iter.second;
        }
    }

    /**
     * @brief 将meta对象中保存的key-value设置到当前对象中。
     *        如果meta中key在当前对象中不存在，则将该key-value添加到当前对象，否则使用meta中key-value更新当前对象的值
     * @param meta 源meta data
     */
    void set(const FrameMetaData& meta)
    {
        set(meta.m_datas);
    }

    /**
     * @brief 将meta对象中保存的key-value设置到当前对象中。
     *        如果meta中key在当前对象中不存在，则将该key-value添加到当前对象，否则使用meta中key-value更新当前对象的值
     * @param meta 源meta data
     */
    void set(const std::shared_ptr<FrameMetaData>& meta)
    {
        if (meta == nullptr)
            return;
        set(*meta);
    }

    /**
     * @brief 获取与key对应的value
     * @param key 
     * @return const std::string& 
     */
    const std::string& get(const std::string& key) const
    {
        const auto iter = m_datas.find(key);
        if(iter==m_datas.end()) return EMPTY_VALUE;
        return iter->second;
    }

    const std::map<std::string, std::string>& get() const { return m_datas; }

    bool empty() const { return m_datas.empty(); }

    std::string toString() const
    {
        std::string result = "[";
        for (const auto& iter : m_datas)
        {
            result += iter.first + ":" + iter.second + ",";
        }
        result += "]";
        return std::move(result);
    }
private:
    std::map<std::string, std::string> m_datas;
    const static std::string EMPTY_VALUE;
};

class FrameMetaDataValid
{
public:
    std::shared_ptr<FrameMetaData> getMetaData()
    {
        if (m_metaData == nullptr)
            m_metaData = std::make_shared<FrameMetaData>();
        return m_metaData;
    }
    void setMetaData(std::shared_ptr<FrameMetaData> meta)
    {
        if (!meta) return;
        if (m_metaData == nullptr)
            m_metaData = std::make_shared<FrameMetaData>();
        m_metaData->set(meta);
    }

protected:
    std::shared_ptr<FrameMetaData> m_metaData;
};

}//namespace zMedia

#endif//_Z_MEDIA_FRAME_META_DATA_H_