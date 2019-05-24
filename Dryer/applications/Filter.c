

#include "Filter.h"




//static rt_uint32_t _ABS(rt_int32_t a, rt_int32_t b)
//{
//	if(a) 
//}
static void _SortMinToMax(rt_int32_t *pBuffer, rt_uint32_t Len)
{
	rt_uint32_t i =0;
	rt_int32_t j = 0, tmp = 0;
	for(i = 1; i < Len; i++){
		if(pBuffer[i] < pBuffer[i-1]){
			tmp = pBuffer[i];
			for(j = i - 1; j >= 0 && pBuffer[j] > tmp; j--){
				pBuffer[j + 1] = pBuffer[j];
			}
			pBuffer[j + 1] = tmp;            
		}              
	}
	/*for(i = 0; i < Len; i++){
		rt_kprintf("%d ", pBuffer[i]);
	}
	rt_kprintf("\n");*/
}


/*
* 函数功能: 限幅滤波
* LastValue: 前一次采样值
* CurValue: 当前采样值
* MaxChange: 最大允许变化范围
* return: 限幅后的值
*/
rt_int32_t FilterLimitAmplitude1(rt_int32_t LastValue, rt_int32_t CurValue, rt_int32_t MaxChange)
{
	//与前一次的值进行比较, 如果变化超过允许范围, 以前一次采样值为准
	if(((LastValue - CurValue) > MaxChange) || ((CurValue - LastValue) > MaxChange)){
		return LastValue;
	}else{
		return CurValue;
	}
}

/*
* 函数功能: 改进型限幅滤波
* LastValue1: 在LastValue2之前的一次采样值
* LastValue2: 在CurValue之前的一次采样值
* CurValue: 当前采样值
* MaxChange: 最大允许变化范围
* return: 限幅后的值
*/
rt_int32_t FilterLimitAmplitude2(rt_int32_t LastValue1, rt_int32_t LastValue2, rt_int32_t CurValue, rt_int32_t MaxChange)
{
	//与前一次的值进行比较, 如果变化超过允许范围, 则按趋势进行预测
	if(((LastValue2 - CurValue) > MaxChange) || ((CurValue - LastValue2) > MaxChange)){
		rt_int32_t New;
		New = 2 * LastValue2 - LastValue1;
		//12位AD采集值不可能是负数也不可能大于4095
		if(New < 0){
			New = 0;
		}else if(New > 4095){
			New = 4095;
		}
		return New;
	}else{
		return CurValue;
	}
}

/*
* 函数功能: 平均值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* return: 滤波后的值
*/
rt_int32_t FilterAverageValue(rt_int32_t *pBuffer, rt_uint32_t Len)
{
	rt_uint32_t i = 0;
	rt_int32_t TotalValue = 0;
	for(i = 0; i < Len; i++){
		TotalValue += pBuffer[i];
	}
	return TotalValue/Len;
}

/*
* 函数功能: 中值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* return: 滤波后的值
*/
rt_int32_t FilterMiddleValue(rt_int32_t *pBuffer, rt_uint32_t Len)
{
	_SortMinToMax(pBuffer, Len);
	return pBuffer[Len/2];
}

/*
* 函数功能: 去极值平均滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* return: 滤波后的值
*/
rt_int32_t FilterRemoveLimitValue(rt_int32_t *pBuffer, rt_uint32_t Len)
{
	_SortMinToMax(pBuffer, Len);
	return FilterAverageValue(&pBuffer[1], Len - 2);
}

/*
* 函数功能: 限幅加平均值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* MaxChange: 集合中最大允许变化的范围
* return: 滤波后的值
*/
rt_int32_t FilterLimitAverage1(rt_int32_t *pBuffer, rt_uint32_t Len, rt_int32_t MaxChange)
{
	rt_int32_t Total = 0;
	rt_uint32_t i = 0;
	for(i = 1; i < Len; i++){
		Total += FilterLimitAmplitude1(pBuffer[i - 1], pBuffer[i], MaxChange);
	}
	return Total /= Len;
}

/*
* 函数功能: 改进型限幅加平均值滤波
* pBuffer: 采集值集合
* Len: 采集值集合长度
* MaxChange: 集合中最大允许变化的范围
* return: 滤波后的值
*/
//rt_int32_t FilterLimitAverage2(rt_int32_t *pBuffer, rt_uint32_t Len, rt_int32_t MaxChange)
//{
//	rt_int32_t Total = 0;
//	rt_int32_t LastValue1 = 0, LastValue2 = 0, CurValue = 0;
//	rt_uint32_t i = 0;
//	LastValue1 = pBuffer[0]; 
//	LastValue2 = pBuffer[1]; 
//	CurValue = pBuffer[2]; 
//	for(i = 1; i < Len; i++){
//		Total += FilterLimitAmplitude1(pBuffer[i - 1], pBuffer[i], MaxChange);
//	}
//	return Total /= Len;
//}
/*
* 函数功能: 一阶滞后滤波器
* LastOutValue: 上一次滤波后输出值
* CurValue: 本次采样的值
* A: 滤波系数
* return: 滤波后的输出值
* 截止频率计算: f = A/(2 * PI * t)
* 其中: t: 采样间隔时间
*/
rt_int32_t FilterOneOrderLagging(rt_int32_t LastOutValue, rt_int32_t CurValue, double A)
{
	rt_int32_t Out = 0;
	Out = A * CurValue + (1 - A) * LastOutValue;
	return Out;
}
