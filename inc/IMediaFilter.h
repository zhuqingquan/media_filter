/*
 * IMediaFilter.h
 *
 *  Created on: 2017年4月7日
 *      Author: zhuqingquan
 */

#ifndef IMEDIAFILTER_H_
#define IMEDIAFILTER_H_

#include "BoostInc.h"
#include "IMediaData.h"
#include <vector>

namespace zMedia
{

class IDataPuller;
class IDataPusher;
class IDataSink;

/*
class IDataPin{
public:
	IDataPin();
	virtual ~IDataPin();
public:
	virtual int addFilter(const boost::shared_ptr<IMediaFilter>& in);
	virtual int remFilter(const boost::shared_ptr<IMediaFilter>& in);
	virtual int clear();
	virtual int getFilter(std::vector<boost::shared_ptr<IMediaFilter> >& listFilter);
protected:
	boost::mutex mutexFilters;
	std::vector<boost::shared_ptr<IMediaFilter> >vecFilters;
};
*/

class IMediaFilter 
{
public:
	typedef IMediaFilter SelfType;
	typedef boost::shared_ptr<IMediaFilter> SPtr;

	IMediaFilter();
	virtual ~IMediaFilter() = 0;

	virtual int setDataPusher(const IDataPusher::SPtr& p);
	virtual IDataPusher::SPtr& getDataPusher() const;
	virtual int setDataPuller(const IDataPuller::SPtr& p);
	virtual IDataPuller::SPtr& getDataPuller() const;

    virtual IDataSink::SPtr createPullerSink() = 0;
    virtual bool releasePullerSink(IDataSink::SPtr& pullerSink) = 0;

    virtual bool add_v(VideoData::SPtr& vdata) = 0;
    virtual bool add_a(AudioData::SPtr& adata) = 0;

	//virtual int pullData(PictrueData::SPtr& spr);
	//virtual int onPull(PictrueData::SPtr& spr);
	//virtual int onPush(const PictrueData::SPtr& spr);
	//virtual int pullData(PcmData::SPtr& spr);
	//virtual int onPull(PcmData::SPtr& spr);
	//virtual int onPush(const PcmData::SPtr& spr);
	//virtual int addPullFilter(const boost::shared_ptr<IMediaFilter>& in);
	//virtual int remPullFilter(const boost::shared_ptr<IMediaFilter>& in);
	//virtual int addPushFilter(const boost::shared_ptr<IMediaFilter>& in);
	//virtual int remPushFilter(const boost::shared_ptr<IMediaFilter>& in);
protected:
	virtual int pushData(const VideoData::SPtr& spr);
	virtual int pushData(const AudioData::SPtr& spr);

	IDataPusher::SPtr pPusher;
	IDataPuller::SPtr pPuller;

};

class IDataPusher 
{
public:
	typedef IDataPusher SelfType;
	typedef boost::shared_ptr<IDataPusher> SPtr;

	IDataPusher();
	virtual ~IDataPusher();

	virtual int addFilter(const boost::shared_ptr<IMediaFilter>& in);
	virtual int remFilter(const boost::shared_ptr<IMediaFilter>& in);
	virtual int clear();
	virtual int getFilter(std::vector<boost::shared_ptr<IMediaFilter> >& listFilter);

	virtual int push(const VideoData::SPtr& pic);
	virtual int push(const AudioData::SPtr& pic);
protected:
	boost::mutex mutexFilters;
	std::vector<boost::shared_ptr<IMediaFilter> >vecFilters;
};

class IDataSink
{
public:
	typedef IDataSink SelfType;
	typedef boost::shared_ptr<IDataSink> SPtr;

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
	typedef boost::shared_ptr<IDataPuller> SPtr;

	IDataPuller();
	virtual ~IDataPuller();

	virtual int addSink(const IDataSink::SPtr& in);
	virtual int remSink(const IDataSink::SPtr& in);
	virtual int clear();
	virtual int getFilter(std::vector<IDataSink::SPtr>& listFilter);

	virtual int pull(PictrueData::SPtr& pic);
	virtual int pull(PcmData::SPtr& pic);

protected:
	boost::mutex m_mutexSinks;
	std::vector<IDataSink::SPtr> m_vecSinks;
};

} /* namespace vmixerapp */

#endif /* IMEDIAFILTER_H_ */
