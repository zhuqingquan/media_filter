#include "gtest/gtest.h"
#include "PictureInfo.h"

TEST(PictureInfo, static_BYTE_count_of_pixfmt)
{
    EXPECT_EQ(0, zMedia::PixelBytesCount[zMedia::PIXFMT_E_NONE]);
    EXPECT_EQ(1.5, zMedia::PixelBytesCount[zMedia::PIXFMT_E_I420]);
    EXPECT_EQ(1.5, zMedia::PixelBytesCount[zMedia::PIXFMT_E_YV12]);
    EXPECT_EQ(2, zMedia::PixelBytesCount[zMedia::PIXFMT_E_YUY2]);
    EXPECT_EQ(2, zMedia::PixelBytesCount[zMedia::PIXFMT_E_UYVY]);
    EXPECT_EQ(2, zMedia::PixelBytesCount[zMedia::PIXFMT_E_RGB565]);
    EXPECT_EQ(3, zMedia::PixelBytesCount[zMedia::PIXFMT_E_RGB24]);
    EXPECT_EQ(4, zMedia::PixelBytesCount[zMedia::PIXFMT_E_RGB32]);
    EXPECT_EQ(4, zMedia::PixelBytesCount[zMedia::PIXFMT_E_RGBA]);
    EXPECT_EQ(4, zMedia::PixelBytesCount[zMedia::PIXFMT_E_BGRA]);
    EXPECT_EQ(4, zMedia::PixelBytesCount[zMedia::PIXFMT_E_MAX]);
}

TEST(PICTURE_FORMAT, base)
{
    zMedia::PICTURE_FORMAT default_fmt;
    EXPECT_FALSE(default_fmt.isValid());

    zMedia::PICTURE_FORMAT wh_fmt_rgb(200, 100, zMedia::PIXFMT_E_RGB24);
    EXPECT_EQ(200, wh_fmt_rgb.w);
    EXPECT_EQ(100, wh_fmt_rgb.h);
    EXPECT_EQ(zMedia::Align16Bytes(600), wh_fmt_rgb.stride);
    EXPECT_EQ(0, wh_fmt_rgb.u_stride);
    EXPECT_EQ(0, wh_fmt_rgb.v_stride);
    EXPECT_EQ(0, wh_fmt_rgb.a_stride);
    zMedia::PICTURE_FORMAT wh_fmt_rgb32(200, 100, zMedia::PIXFMT_E_RGB32);
    EXPECT_EQ(zMedia::Align16Bytes(800), wh_fmt_rgb32.stride);
    zMedia::PICTURE_FORMAT wh_fmt_I420(200, 100, zMedia::PIXFMT_E_I420);
    EXPECT_EQ(zMedia::Align16Bytes(200), wh_fmt_I420.y_stride);
    EXPECT_EQ(zMedia::Align16Bytes(100), wh_fmt_I420.u_stride);
    EXPECT_EQ(zMedia::Align16Bytes(100), wh_fmt_I420.v_stride);
    zMedia::PICTURE_FORMAT wh_fmt_yuy2(200, 100, zMedia::PIXFMT_E_YUY2);
    EXPECT_EQ(zMedia::Align16Bytes(400), wh_fmt_yuy2.y_stride);
}

TEST(PictureRaw, constructor)
{
    zMedia::PictureRaw picDefault;
    EXPECT_FALSE(picDefault.format().isValid());
    EXPECT_EQ(NULL, picDefault.data());
    EXPECT_EQ(NULL, picDefault.y());
    EXPECT_EQ(0, picDefault.size());

    zMedia::PICTURE_FORMAT fmt_invalid;
    EXPECT_FALSE(picDefault.allocData(fmt_invalid));

    zMedia::PICTURE_FORMAT fmt(200, 100, zMedia::PIXFMT_E_I420);
    EXPECT_TRUE(picDefault.allocData(fmt));
    EXPECT_TRUE(NULL!=picDefault.data());
    EXPECT_TRUE(NULL!=picDefault.y());
    EXPECT_TRUE(NULL!=picDefault.u());
    EXPECT_TRUE(NULL!=picDefault.v());
    EXPECT_EQ(zMedia::GetPictureSize(fmt), picDefault.size());
    picDefault.freeData();
    EXPECT_EQ(NULL, picDefault.data());
    EXPECT_EQ(NULL, picDefault.y());
    EXPECT_EQ(0, picDefault.size());
}
