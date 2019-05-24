

#ifndef _FILTER_H
#define _FILTER_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/*
* ��������: �޷��˲�
* LastValue: ǰһ�β���ֵ
* CurValue: ��ǰ����ֵ
* MaxChange: �������仯��Χ
* return: �޷����ֵ
*/
rt_int32_t FilterLimitAmplitude1(rt_int32_t LastValue, rt_int32_t CurValue, rt_int32_t MaxChange);
/*
* ��������: �Ľ����޷��˲�
* LastValue1: ��LastValue2֮ǰ��һ�β���ֵ
* LastValue2: ��CurValue֮ǰ��һ�β���ֵ
* CurValue: ��ǰ����ֵ
* MaxChange: �������仯��Χ
* return: �޷����ֵ
*/
rt_int32_t FilterLimitAmplitude2(rt_int32_t LastValue1, rt_int32_t LastValue2, rt_int32_t CurValue, rt_int32_t MaxChange);
/*
* ��������: ƽ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* return: �˲����ֵ
*/
rt_int32_t FilterAverageValue(rt_int32_t *pBuffer, rt_uint32_t Len);
/*
* ��������: ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* return: �˲����ֵ
*/
rt_int32_t FilterMiddleValue(rt_int32_t *pBuffer, rt_uint32_t Len);

/*
* ��������: ȡ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* return: �˲����ֵ
*/
rt_int32_t FilterRemoveLimitValue(rt_int32_t *pBuffer, rt_uint32_t Len);
/*
* ��������: �޷���ƽ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* MaxChange: �������������仯�ķ�Χ
* return: �˲����ֵ
*/
rt_int32_t FilterLimitAverage1(rt_int32_t *pBuffer, rt_uint32_t Len, rt_int32_t MaxChange);
/*
* ��������: һ���ͺ��˲���
* LastOutValue: ��һ���˲������ֵ
* CurValue: ���β�����ֵ
* A: �˲�ϵ��
* return: �˲�������ֵ
* ��ֹƵ�ʼ���: f = A/(2 * PI * t)
* ����: t: �������ʱ��
*/
rt_int32_t FilterOneOrderLagging(rt_int32_t LastOutValue, rt_int32_t CurValue, double A);

#endif /* _FILTER_H */


