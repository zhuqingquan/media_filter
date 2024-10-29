/*
 * @Author: zhuqingquan
 * @FilePath: /media_filter/inc/PcmDataHelper.h
 * @Description: helper fun for pcm data
 */
#ifndef _MEDIA_FILTER_COMMON_PCM_DATA_HELPER_H_
#define _MEDIA_FILTER_COMMON_PCM_DATA_HELPER_H_

#include "PcmData.h"
#include <vector>

namespace zMedia
{
	PcmData::SPtr clonePcmData(const PcmData::SPtr& srcData);
	// 将srcData中的数据拷贝到dstData中，如果srcData数据只拷贝了部分，返回值中返回剩余未拷贝的那部分数据，如果全部拷贝完了，则返回null
	// 一般用于将小块的srcData收集起来的操作
	//PcmData::SPtr CopyPcmData(PcmData::SPtr dstData, const PcmData::SPtr& srcData);

	PcmData::SPtr convertPcmData(const PcmData::SPtr& srcData, uint32_t dstChannels, uint32_t dstSampleRate, AudioSampelTypeSize dstSampleTypeSize);

	int float2short(const float* floatData, int floatCount, short* shortBuffer, int shortCount);
	int short2float(const short* shortData, int shortCount, float* floatBuffer, int floatCount);
	void mixPcms(const std::vector<float*>& pcms, int samples, const std::vector<double>& volumeFactors, float* output, double masterVolumeFactor);
	void mixPcms(const std::vector<short*>& pcms, int samples, const std::vector<double>& volumeFactors, short* output, double masterVolumeFactor);

    /**
     * 计算PCM数据的音量值，暂时未完整实现，现在只能用于判断PCM数据是否全为0
     **/
	int calculatePcmLevel(const PcmData::SPtr& srcData);

    /**
     * @brief 调节音量
     * @param data 待调节的PCM数据，float类型的采样数据
     * @param sampleCount data指向的数据中心包含的采样个数
     * @param volFactor 音量调节的倍数，[0, +)
     */
    void scaleVolume(float* data, int sampleCount, double volFactor);

    /**
     * @brief 调节音量
     * @param data 待调节的PCM数据，float类型的采样数据
     * @param sampleCount data指向的数据中心包含的采样个数
     * @param volFactor 音量调节的倍数，[0, +)
     */
    void scaleVolume(short* data, int sampleCount, double volFactor);

    /**
     *  @brief 改变PCM数据的内存布局，从packed数据布局改成planar布局。会进行数据的复制
     *  @param packedData 数据源，packed数据。比如2个声道的数据，此时数据保存的布局格式是 L R L R L R
     *  @param samplecount packedData中保存的数据的采样个数，单个通道的采样个数，总采样个数=samplecount*channel
     *  @param channel 声道数
     *  @param planaData 保存转换后的planar布局的PCM数据的内存指针数组。数组的长度必须与channel一直否则可能有问题
     *  @return 返回已处理的采样数（单个通道的）
     **/
    int PCMPackedDataToPlanar(const float* packedData, int samplecount, int channel, float* planaData[]);

	class PcmDataConvert
	{
	public:
		PcmDataConvert();
		~PcmDataConvert();

		bool float2short(const BYTE* floatData, int bytesCount );
		bool short2float(const BYTE* shortData, int bytesCount );

		const BYTE* data() const { return m_buf.data(); }
		size_t size() const { return m_dataSize; }
		void clear() { m_dataSize = 0; }

	private:
		void alloc(int size);

	private:
		MediaBuffer m_buf;
		//BYTE* m_buffer;
		int m_dataSize;
		//int m_bufferSize;
	};
}//namespace zMedia

#endif //_MEDIA_FILTER_COMMON_PCM_DATA_HELPER_H_
