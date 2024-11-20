#include "BasicMediaFilter.h"
#include "VideoFrameQueue.h"
#include "ThreadTaskHandler.h"
#include "TextHelper.h"
#include "logger.h"

using namespace zMedia;
typedef std::lock_guard<std::mutex> scoped_lock;
static std::atomic<uint64_t> BasicFilterIDIndex;

const std::string FrameMetaData::EMPTY_VALUE;

SyncDataPusher::SyncDataPusher(IMediaFilter* mf)
: m_mf(mf)
{
}

SyncDataPusher::~SyncDataPusher()
{
}


int SyncDataPusher::push(VideoData::SPtr& pic)
{
	bool allFailed = true;
    scoped_lock lock(mutexFilters);
    std::vector<IMediaFilter::SPtr>::const_iterator iter = vecFilters.begin();
    for(; iter!=vecFilters.end(); iter++)
    {
        IMediaFilter::SPtr pmf = *iter;
		if (pmf)
		{
			if (pmf->add_v(pic, m_mf))
				allFailed = false;
		}
    }
    return allFailed ? -1 : 0;
}

int SyncDataPusher::push(AudioData::SPtr& pic)
{
    scoped_lock lock(mutexFilters);
    std::vector<IMediaFilter::SPtr>::const_iterator iter = vecFilters.begin();
    for(; iter!=vecFilters.end(); iter++)
    {
        IMediaFilter::SPtr pmf = *iter;
        if(pmf) pmf->add_a(pic, m_mf);
    }
    return 0;
}

//BasicMediaFilter::BasicMediaFilter()
//{
//}

BasicMediaFilter::BasicMediaFilter(const IMediaFilter::IDType& id)
{
	uint64_t index = BasicFilterIDIndex.fetch_add(1);
	m_id = id + std::to_wstring(index);
}

BasicMediaFilter::~BasicMediaFilter()
{
    stop();
}

bool BasicMediaFilter::start()
{
    if(m_worker==nullptr)
        m_worker = new zUtils::ThreadTaskHandler(); 
	m_isRunning = true;
    onStart();
	bool ret = m_worker->start();
	if (ret)
	{
		m_worker->add([&](void*) {
			dowork();
		});
	}
	return ret;
}

void BasicMediaFilter::stop()
{
	m_isRunning = false;
    if (m_worker != nullptr)
    {
        delete m_worker;
        m_worker = nullptr;
    }
    onStop();
}

bool BasicMediaFilter::isRunning()
{
	return m_isRunning;
}

int BasicMediaFilter::attachOutputFilter(const IMediaFilter::SPtr& output)
{
    if(m_Pusher==nullptr)
        m_Pusher = createDataPusher();
    if(m_Pusher==nullptr)
        return -1;
    return m_Pusher->addFilter(output);
}

bool BasicMediaFilter::dettachOutputFilter(const IMediaFilter::SPtr& output)
{
    if(m_Pusher==nullptr)
        return false;
    return 0==m_Pusher->remFilter(output);
}

int BasicMediaFilter::attachOutputFilter(const IMediaFilter::SPtr& output, const std::string& tag)
{
    auto iter = m_tagedPusher.find(tag);
    zMedia::IDataPusher::SPtr pusher;
    if(iter==m_tagedPusher.end() || iter->second==nullptr)
    {
        pusher = createDataPusher();
        if(pusher!=nullptr)
            m_tagedPusher[tag] = pusher;
        else
            return -1;
    }
    return pusher->addFilter(output);
}

bool BasicMediaFilter::dettachOutputFilter(const IMediaFilter::SPtr& output, const std::string& tag)
{
    auto iter = m_tagedPusher.find(tag);
    if(iter==m_tagedPusher.end() || iter->second==nullptr)
    {
        return false;
    }
    return 0==iter->second->remFilter(output);
}

std::shared_ptr<IDataPusher> BasicMediaFilter::createDataPusher()
{
    return std::make_shared<SyncDataPusher>(this);
}

const IMediaFilter::IDType& BasicMediaFilter::getID() const
{
    return m_id;
}

std::shared_ptr<IDataSink> BasicMediaFilter::createPullerSink()
{
    return IDataSink::SPtr();
}

bool BasicMediaFilter::releasePullerSink(std::shared_ptr<IDataSink>& pullerSink)
{
    return true;
}

std::shared_ptr<VideoFrameQueue> BasicMediaFilter::createVideoFrameQueue(int maxFrameCount)
{
    // 一般情况下创建普通的VideoFrameQueue，但一些特殊场景需要对队列进行特殊配置
    if (maxFrameCount > 0)
        return std::make_shared <VideoFrameQueue>(maxFrameCount);
    return std::make_shared<VideoFrameQueue>();
}

bool BasicMediaFilter::add_v(VideoData::SPtr vdata, IMediaFilter* from)
{
    if(vdata==nullptr)
        return false;
    if(m_srcQueue==nullptr)
        m_srcQueue = createVideoFrameQueue();
    if(m_srcQueue==nullptr)
        return false;
    if (!m_srcQueue->queue(vdata))
    {
        logw("add video frame to queue failed. maybe drop one frame.filter=%S", getID().c_str());
    }
    onAddV(vdata);

    if(m_worker!=nullptr)
    {
        // 每次添加一个数据都尝试去唤醒线程去处理，在processVideoData中会一直从队列取数据，直到队列为空
        m_worker->add([&](void*) {
            processVideoData();
        });
    }

    //onPushV(vdata);
    //pushData(vdata);
    return true;
}

bool BasicMediaFilter::add_a(AudioData::SPtr adata, IMediaFilter* from)
{
    onAddA(adata);

    if(m_worker!=nullptr)
    {
        // 每次添加一个数据都尝试去唤醒线程去处理
        m_worker->add([adata, this](void*) {
            AudioData::SPtr tmp = adata;
            onPrecessAudioData(tmp);
        });
    }
    //onPushA(adata);
    //pushData(adata);
    return true;
}

void BasicMediaFilter::onAddV(VideoData::SPtr& vdata)
{
}

void BasicMediaFilter::onAddA(AudioData::SPtr& adata)
{
}

void BasicMediaFilter::onPushV(VideoData::SPtr& vdata)
{
}

void BasicMediaFilter::onPushA(AudioData::SPtr& adata)
{
}

void BasicMediaFilter::processVideoData()
{
    do {
        VideoData::SPtr vdata = m_srcQueue->dequeue();
        if (vdata == nullptr)
        {
            break;
        }
        onProcessVideoData(vdata);
    }while(1);
}

void BasicMediaFilter::onProcessVideoData(VideoData::SPtr& vdata)
{

}

void BasicMediaFilter::onPrecessAudioData(AudioData::SPtr& adata)
{

}
