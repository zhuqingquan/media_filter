
#include "IMediaFilter.h"
#include <algorithm>

using namespace zMedia;
typedef std::lock_guard<std::mutex> scoped_lock;

// IDataPusher
IDataPusher::IDataPusher()
{
}

IDataPusher::~IDataPusher()
{
}

int IDataPusher::addFilter(const IMediaFilter::SPtr& in)
{
    scoped_lock lock(mutexFilters);
    auto iter = std::find(vecFilters.begin(), vecFilters.end(), in);
    if(iter!=vecFilters.end())
        return 0;
    vecFilters.push_back(in);
    return 0;
}

int IDataPusher::remFilter(const IMediaFilter::SPtr& in)
{
    scoped_lock lock(mutexFilters);
    vecFilters.erase( std::remove(vecFilters.begin(), vecFilters.end(), in), vecFilters.end());
    return 0;
}

int IDataPusher::clear()
{
    scoped_lock lock(mutexFilters);
    vecFilters.clear();
    return 0;
}

int IDataPusher::getFilter(std::vector<IMediaFilter::SPtr >& listFilter)
{
    scoped_lock lock(mutexFilters);
    listFilter = vecFilters;
    return 0;
}

// IDataPuller
IDataPuller::IDataPuller()
{
}

IDataPuller::~IDataPuller()
{
}

int IDataPuller::addSink(const IDataSink::SPtr& in)
{
    scoped_lock lock(m_mutexSinks);
    m_vecSinks.push_back(in);
    return 0;
}

int IDataPuller::remSink(const IDataSink::SPtr& in)
{
    scoped_lock lock(m_mutexSinks);
    m_vecSinks.erase( std::remove(m_vecSinks.begin(), m_vecSinks.end(), in), m_vecSinks.end());
    return 0;
}

int IDataPuller::clear()
{
    scoped_lock lock(m_mutexSinks);
    m_vecSinks.clear();
    return 0;
}

int IDataPuller::getFilter(std::vector<IDataSink::SPtr>& listSinks)
{
    scoped_lock lock(m_mutexSinks);
    listSinks = m_vecSinks;
    return 0;
}

// IMediaFilter
IMediaFilter::IMediaFilter()
{
}

IMediaFilter::~IMediaFilter()
{
}
/*
	int IMediaFilter::pullData(PictrueData::SPtr& spr){
		if (pPuller){
			pPuller->pull(spr);
			return 1;
		}
		return 0;
	}

	int IMediaFilter::pushData(const PictrueData::SPtr& spr){
		if (pPusher){
			pPusher->push(spr);
			return 1;
		}
		return 0;
	}

	int IMediaFilter::onPull(PictrueData::SPtr& spr){
		return 0;
	}

	int IMediaFilter::onPush(const PictrueData::SPtr& spr){
		return 0;
	}

	int IMediaFilter::pullData(PcmData::SPtr& spr){
		if (pPuller){
			pPuller->pull(spr);
			return 1;
		}
		return 0;
	}

	int IMediaFilter::pushData(const PcmData::SPtr& spr){
		if (pPusher){
			pPusher->push(spr);
			return 1;
		}
		return 0;
	}

	int IMediaFilter::onPull(PcmData::SPtr& spr){
		return 0;
	}
	int IMediaFilter::onPush(const PcmData::SPtr& spr){
		return 0;
	}
*/
//int IMediaFilter::setDataPusher(const std::shared_ptr<IDataPusher>& p)
//{
//    m_Pusher = p;
//    return 0;
//}
//
//std::shared_ptr<IDataPusher> IMediaFilter::getDataPusher() const
//{
//    return m_Pusher;
//}

int IMediaFilter::setDataPuller(const IDataPuller::SPtr& p)
{
    m_Puller = p;
    return 0;
}

IDataPuller::SPtr IMediaFilter::getDataPuller() const
{
    return m_Puller;
}

IDataPusher::SPtr IMediaFilter::getDataPusher() const
{
    return m_Pusher;
}

int IMediaFilter::pushData(VideoData::SPtr spr)
{
    if(m_Pusher)
    {
        return m_Pusher->push(spr);
    }
    return -1;
}

int IMediaFilter::pushData(AudioData::SPtr spr, const std::string& tag)
{
	if(tag.empty()) return pushData(spr);
	auto iter = m_tagedPusher.find(tag);
	if(iter==m_tagedPusher.end() || iter->second==nullptr)
		return -1;
    return iter->second->push(spr);
}

int IMediaFilter::pushData(VideoData::SPtr spr, const std::string& tag)
{
	if(tag.empty()) return pushData(spr);
	auto iter = m_tagedPusher.find(tag);
	if(iter==m_tagedPusher.end() || iter->second==nullptr)
		return -1;
    return iter->second->push(spr);
}

int IMediaFilter::pushData(AudioData::SPtr spr)
{
    if(m_Pusher)
    {
        return m_Pusher->push(spr);
    }
    return -1;
}
/*
	int IMediaFilter::addPullFilter(const std::shared_ptr<IMediaFilter>& in){
		if (pPuller){
			pPuller->addFilter(in);
			return 1;
		}
		return 0;
	}

	int IMediaFilter::remPullFilter(const std::shared_ptr<IMediaFilter>& in){
		if (pPuller){
			pPuller->remFilter(in);
			return 1;
		}
		return 0;
	}

	int IMediaFilter::addPushFilter(const std::shared_ptr<IMediaFilter>& in){
		if (pPusher){
			pPusher->addFilter(in);
			return 1;
		}
		return 0;
	}

	int IMediaFilter::remPushFilter(const std::shared_ptr<IMediaFilter>& in){
		if (pPusher){
			pPusher->remFilter(in);
			return 1;
		}
		return 0;
	}
*/
