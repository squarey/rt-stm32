

#ifndef _FILTER_H
#define _FILTER_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/*
* 函数功能: 限幅滤波
* LastValue: 前一次采样值
* CurValue: 当前采样值
* MaxChange: 最大允许变化范围
* return: 限幅后的值
*/
rt_int32_t FilterLimitAmplitude1(rt_int32_t LastValue, rt_int32_t CurValue, rt_int32_t MaxChange);
/*
* 函数功能: 改进型限幅滤波
* LastValue1: 在LastValue2之前的一次采样值
* LastValue2: 在CurValue之前的一次采样值
* CurValue: 当前采样值
* MaxChange: 最大允许变化范围
* return: 限幅后的值
*/
rt_int32_t FilterLimitAmplitude2(rt_int32_t LastValue1, rt_int32_t LastValue2, rt_int32_t CurValue, rt_int32_t MaxChange);
/*
* 函数功能: 平均值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* return: 滤波后的值
*/
rt_int32_t FilterAverageValue(rt_int32_t *pBuffer, rt_uint32_t Len);
/*
* 函数功能: 中值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* return: 滤波后的值
*/
rt_int32_t FilterMiddleValue(rt_int32_t *pBuffer, rt_uint32_t Len);

/*
* 函数功能: 取极值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* return: 滤波后的值
*/
rt_int32_t FilterRemoveLimitValue(rt_int32_t *pBuffer, rt_uint32_t Len);
/*
* 函数功能: 限幅加平均值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* MaxChange: 集合中最大允许变化的范围
* return: 滤波后的值
*/
rt_int32_t FilterLimitAverage1(rt_int32_t *pBuffer, rt_uint32_t Len, rt_int32_t MaxChange);
/*
* 函数功能: 一阶滞后滤波器
* LastOutValue: 上一次滤波后输出值
* CurValue: 本次采样的值
* A: 滤波系数
* return: 滤波后的输出值
* 截止频率计算: f = A/(2 * PI * t)
* 其中: t: 采样间隔时间
*/
rt_int32_t FilterOneOrderLagging(rt_int32_t LastOutValue, rt_int32_t CurValue, double A);

#endif /* _FILTER_H */


