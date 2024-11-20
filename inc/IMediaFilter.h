/*
 * IMediaFilter.h
 *
 *  Created on: 2017年4月7日
 *      Author: zhuqingquan
 */

#ifndef IMEDIAFILTER_H_
#define IMEDIAFILTER_H_

// #include "BoostInc.h"
#include "VideoData.h"
#include "AudioData.h"
#include <vector>
#include <string>
#include <map>
#include <mutex>

namespace zMedia
{

class IDataPuller;
class IDataPusher;
class IDataSink;

class IMediaFilter 
{
public:
	typedef IMediaFilter SelfType;
	typedef std::shared_ptr<IMediaFilter> SPtr;
    typedef std::wstring IDType;

	IMediaFilter();
	virtual ~IMediaFilter() = 0;

    virtual const IDType& getID() const = 0;

	/**
	 *	Filter运行状态过程回调
	 **/
	struct Callback
	{
		/**
		 *	@brief 当打开源失败时产生此回调
		 *  @param	srcType 源类型
		 *  @param	srcUrl 源的地址。传入的文件的路径名称等
		 *  @param	reason 失败的原因。暂时未定义，可以忽略
		 **/
		virtual void onOpenSrcFailed(IMediaFilter* filter, const std::string& srcType, const std::wstring& srcUrl, int reason) = 0;

        /**
         *  @brief 当播放到了文件的结尾时回调
		 *  @param	srcType 源类型
		 *  @param	srcUrl 源的地址。传入的文件的路径名称等
         **/
		virtual void onFilePlayEOF(IMediaFilter* filter, const std::string& srcType, const std::wstring& srcUrl) = 0;
	};
	void setCallback(Callback* cb) { m_cb = cb; }

	virtual bool start() = 0;
	virtual void stop() = 0;
	virtual bool isRunning() = 0;

	virtual int attachOutputFilter(const IMediaFilter::SPtr& output) = 0;
	virtual bool dettachOutputFilter(const IMediaFilter::SPtr& output) = 0;
	virtual int attachOutputFilter(const IMediaFilter::SPtr& output, const std::string& tag) = 0;
	virtual bool dettachOutputFilter(const IMediaFilter::SPtr& output, const std::string& tag) = 0;
	//virtual int setDataPusher(const std::shared_ptr<IDataPusher>& p);
	//virtual std::shared_ptr<IDataPusher> getDataPusher() const;
	virtual int setDataPuller(const std::shared_ptr<IDataPuller>& p);
	virtual std::shared_ptr<IDataPuller> getDataPuller() const;
	virtual std::shared_ptr<IDataPusher> getDataPusher() const;

    virtual std::shared_ptr<IDataSink> createPullerSink() = 0;
    virtual bool releasePullerSink(std::shared_ptr<IDataSink>& pullerSink) = 0;

    virtual bool add_v(VideoData::SPtr vdata, IMediaFilter* from) = 0;
    virtual bool add_a(AudioData::SPtr adata, IMediaFilter* from) = 0;
protected:
	virtual int pushData(VideoData::SPtr spr);
	virtual int pushData(AudioData::SPtr spr);
	virtual int pushData(VideoData::SPtr spr, const std::string& tag);
	virtual int pushData(AudioData::SPtr spr, const std::string& tag);

	virtual std::shared_ptr<IDataPusher> createDataPusher() = 0;
	virtual void onReleaseDataPusher(std::shared_ptr<IDataPusher> pusher) {}

	std::shared_ptr<IDataPusher> m_Pusher;
	std::shared_ptr<IDataPuller> m_Puller;
	Callback* m_cb = nullptr;
	std::map<std::string, std::shared_ptr<IDataPusher>> m_tagedPusher;		// 按照不同的tag区分的IDataPusher对象
};

class IDataPusher 
{
public:
	typedef IDataPusher SelfType;
	typedef std::shared_ptr<IDataPusher> SPtr;

	IDataPusher();
	virtual ~IDataPusher() = 0;

	virtual int addFilter(const IMediaFilter::SPtr& in);
	virtual int remFilter(const IMediaFilter::SPtr& in);
	virtual int clear();
	virtual int getFilter(std::vector<IMediaFilter::SPtr>& listFilter);

	virtual int push(VideoData::SPtr& pic) = 0;
	virtual int push(AudioData::SPtr& pic) = 0;
protected:
	std::mutex mutexFilters;
	std::vector<std::shared_ptr<IMediaFilter> >vecFilters;
};

class IDataSink
{
public:
	typedef IDataSink SelfType;
	typedef std::shared_ptr<IDataSink> SPtr;

    virtual ~IDataSink() = 0;
    virtual VideoData::SPtr get_v() = 0;
    virtual AudioData::SPtr get_a() = 0;
    virtual bool next_v() = 0;
    virtual bool next_a() = 0;
};

class IDataPuller 
{
public:
	typedef IDataPuller SelfType;
	typedef std::shared_ptr<IDataPuller> SPtr;
    typedef std::map<IMediaFilter::IDType, VideoData::SPtr> DataCollectionV;
    typedef std::map<IMediaFilter::IDType, AudioData::SPtr> DataCollectionA;

	IDataPuller();
	virtual ~IDataPuller() = 0;

	virtual int addSink(const IDataSink::SPtr& in);
	virtual int remSink(const IDataSink::SPtr& in);
	virtual int clear();
	virtual int getFilter(std::vector<IDataSink::SPtr>& listSinks);

	virtual int pull(DataCollectionV& vDatas, UINT timeoutMillsec) = 0;
	virtual int pull(DataCollectionA& aDatas, UINT timeoutMillsec) = 0;

protected:
	std::mutex m_mutexSinks;
	std::vector<IDataSink::SPtr> m_vecSinks;
};

} /* namespace zMedia */

#endif /* IMEDIAFILTER_H_ */
