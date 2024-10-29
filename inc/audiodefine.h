/*
 * @Author: zhuqingquan zqq_222@163.com
 * @Date: 2023-06-21 
 * @FilePath: avcommon\audiodefine.h
 * @Description: 定义一些音频相关的公用结构体或者宏
 */
#pragma once
#ifndef _Z_MEDIA_AUDIO_DEFINE_H_
#define _Z_MEDIA_AUDIO_DEFINE_H_

#include <inttypes.h>

namespace zMedia
{
    /**
     * @brief PCM数据的数据格式。可以通过这个枚举值获得PCM数据的位宽以及内存存放的方式
     */
    enum E_PCMFMT {
        PCM_FMT_NONE = -1,
        PCM_FMT_U8,          ///< unsigned 8 bits
        PCM_FMT_S16,         ///< signed 16 bits
        PCM_FMT_S32,         ///< signed 32 bits
        PCM_FMT_FLT,         ///< float
        PCM_FMT_DBL,         ///< double

        PCM_FMT_U8P,         ///< unsigned 8 bits, planar
        PCM_FMT_S16P,        ///< signed 16 bits, planar
        PCM_FMT_S32P,        ///< signed 32 bits, planar
        PCM_FMT_FLTP,        ///< float, planar
        PCM_FMT_DBLP,        ///< double, planar
        PCM_FMT_S64,         ///< signed 64 bits
        PCM_FMT_S64P,        ///< signed 64 bits, planar

        PCM_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
    };

    static const uint8_t PCM_BTPS_ARRAY[PCM_FMT_NB] = { 8, 16, 32, 32, 64, 8, 16, 32, 32, 64, 64, 64};              // array of bits per sample with E_PCMFMT
    inline uint8_t PCM_BitsPerSample(E_PCMFMT fmt) { return fmt>=0 && fmt<PCM_FMT_NB ? PCM_BTPS_ARRAY[fmt] : 0; }   // bits per sample with E_PCMFMT
    static const bool PCM_ISPLANAR_ARRAY[PCM_FMT_NB] = { false, false, false, false, false, true, true, true, true, true, false, true};
    inline bool PCM_IsPlanar(E_PCMFMT fmt) { return fmt>=0 && fmt<PCM_FMT_NB ? PCM_ISPLANAR_ARRAY[fmt] : false; }

    /**
     * @brief 音频PCM数据的数据格式
     */
    struct PCM_FORMAT
    {
        int channel = 0;            // 通道数
        int samplerate = 0;         // 采样率
        uint8_t bitsPerSample = 0;  // bits per sample
        bool isPlanar = false;      // 是否每个声道独立的planar存放
        bool isLE = true;           // 是否小端
        E_PCMFMT pcmfmt = PCM_FMT_NONE;     // PCM数据的类型
        int sampleCountPerFrame = 0;        // 每个音频帧所包含的每个channel的采样个数

        PCM_FORMAT() {}
        PCM_FORMAT(int ch, int sr, E_PCMFMT _pcmfmt)
            : channel(ch), samplerate(sr), pcmfmt(_pcmfmt)
            , bitsPerSample(PCM_BitsPerSample(_pcmfmt))
        {

        }
    };
}
#endif//_Z_MEDIA_AUDIO_DEFINE_H_