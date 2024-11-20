#ifndef _Z_MEDIA_BASIC_MEDIA_FILTER_H_
#define _Z_MEDIA_BASIC_MEDIA_FILTER_H_

#include "IMediaFilter.h"

namespace zUtils
{
    class ThreadTaskHandler;
}
namespace zMedia
{
    class VideoFrameQueue;

    //typedef std::shared_ptr<VideoFrameQueue> VideoFrameQueue::SPtr;

    /**
     * @name    SyncDataPusher
     * @brief   A kind of IDataPusher
     *          The data is pushed to the next filters sync
     **/
    class SyncDataPusher : public IDataPusher
    {
    public:
        SyncDataPusher(IMediaFilter* mf);
        virtual ~SyncDataPusher();

        virtual int push(VideoData::SPtr& pic);
        virtual int push(AudioData::SPtr& pic);

    private:
        IMediaFilter* m_mf;
    };

    /**
     * @name    BasicMediaFilter
     * @brief   All kind of MediaFilter that have charaters below can inheritate this class:
     *          1.Not cache any data in the filter
     *          2.When media data is added, this filter pushed it imediatly
     *          3.When media data is adding, do little simple works in this filter.
     *          4.Not support pull mode in this filter.
     **/
    class BasicMediaFilter : public IMediaFilter
    {
    public:
        //BasicMediaFilter();
        BasicMediaFilter(const IMediaFilter::IDType& id);
        virtual ~BasicMediaFilter();
        
        virtual const IMediaFilter::IDType& getID() const override;

        virtual bool start() override;
        virtual void stop() override;
		virtual bool isRunning() override;

        virtual int attachOutputFilter(const IMediaFilter::SPtr& ouput) override;
        virtual bool dettachOutputFilter(const IMediaFilter::SPtr& output) override;
        virtual int attachOutputFilter(const IMediaFilter::SPtr& output, const std::string& tag);
        virtual bool dettachOutputFilter(const IMediaFilter::SPtr& output, const std::string& tag);


        virtual std::shared_ptr<IDataSink> createPullerSink() ;
        virtual bool releasePullerSink(std::shared_ptr<IDataSink>& pullerSink);

        virtual bool add_v(VideoData::SPtr vdata, IMediaFilter* from) override;
        virtual bool add_a(AudioData::SPtr adata, IMediaFilter* from) override;
    protected:
        virtual std::shared_ptr<IDataPusher> createDataPusher() override;

        /**
         * @name    onAddV
         * @brief   When the add_v() is called, the func will call.
         *          Filter can do some work for VideoData in this func
         **/
        virtual void onAddV(VideoData::SPtr& vdata);
        /**
         * @name    onAddA
         * @brief   When the add_a() is called, the func will call.
         *          Filter can do some work for AudioData in this func
         **/
        virtual void onAddA(AudioData::SPtr& adata);

        /**
         * @name    onPushV
         * @brief   Before push VideoData to the next filter, this func will call.
         **/
        virtual void onPushV(VideoData::SPtr& vdata);
        /**
         * @name    onPushA
         * @brief   Before push AudioData to the next filter, this func will call.
         **/
        virtual void onPushA(AudioData::SPtr& adata);

        /**
         * @brief  当start()接口被调用时，在启动处理线程之前，调用此方法。子类可重载此方法
         **/
        virtual void onStart() {};
        /**
         * @brief  当stop()接口被调用时，在处理线程停止之后，调用此方法。子类可重载此方法
         **/
        virtual void onStop() {};

        virtual std::shared_ptr<VideoFrameQueue> createVideoFrameQueue(int maxFrameCount = 0);
        virtual void onProcessVideoData(VideoData::SPtr& vdata);
        virtual void onPrecessAudioData(AudioData::SPtr& adata);
		// 当Filter的m_worker内的线程启动时，将在线程中调用一次dowork()，子类可以重载这个方法进行数据处理
		virtual void dowork() {}

    protected:
		// 当外部调用此Filter的add_v时将在m_worker线程中执行此函数，此函数将会调用onProcessVideoData
        void processVideoData();

		bool m_isRunning = false;			// 标记是否用户调用了stop
        IMediaFilter::IDType m_id;
        std::shared_ptr<VideoFrameQueue> m_srcQueue = nullptr;
        zUtils::ThreadTaskHandler* m_worker = nullptr;
    };
}//namespace zMedia

#endif //_Z_MEDIA_BASIC_MEDIA_FILTER_H_
