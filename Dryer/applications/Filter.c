

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
* ��������: �޷��˲�
* LastValue: ǰһ�β���ֵ
* CurValue: ��ǰ����ֵ
* MaxChange: �������仯��Χ
* return: �޷����ֵ
*/
rt_int32_t FilterLimitAmplitude1(rt_int32_t LastValue, rt_int32_t CurValue, rt_int32_t MaxChange)
{
	//��ǰһ�ε�ֵ���бȽ�, ����仯��������Χ, ��ǰһ�β���ֵΪ׼
	if(((LastValue - CurValue) > MaxChange) || ((CurValue - LastValue) > MaxChange)){
		return LastValue;
	}else{
		return CurValue;
	}
}

/*
* ��������: �Ľ����޷��˲�
* LastValue1: ��LastValue2֮ǰ��һ�β���ֵ
* LastValue2: ��CurValue֮ǰ��һ�β���ֵ
* CurValue: ��ǰ����ֵ
* MaxChange: �������仯��Χ
* return: �޷����ֵ
*/
rt_int32_t FilterLimitAmplitude2(rt_int32_t LastValue1, rt_int32_t LastValue2, rt_int32_t CurValue, rt_int32_t MaxChange)
{
	//��ǰһ�ε�ֵ���бȽ�, ����仯��������Χ, �����ƽ���Ԥ��
	if(((LastValue2 - CurValue) > MaxChange) || ((CurValue - LastValue2) > MaxChange)){
		rt_int32_t New;
		New = 2 * LastValue2 - LastValue1;
		//12λAD�ɼ�ֵ�������Ǹ���Ҳ�����ܴ���4095
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
* ��������: ƽ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* return: �˲����ֵ
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
* ��������: ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* return: �˲����ֵ
*/
rt_int32_t FilterMiddleValue(rt_int32_t *pBuffer, rt_uint32_t Len)
{
	_SortMinToMax(pBuffer, Len);
	return pBuffer[Len/2];
}

/*
* ��������: ȥ��ֵƽ���˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* return: �˲����ֵ
*/
rt_int32_t FilterRemoveLimitValue(rt_int32_t *pBuffer, rt_uint32_t Len)
{
	_SortMinToMax(pBuffer, Len);
	return FilterAverageValue(&pBuffer[1], Len - 2);
}

/*
* ��������: �޷���ƽ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* MaxChange: �������������仯�ķ�Χ
* return: �˲����ֵ
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
* ��������: �Ľ����޷���ƽ��ֵ�˲�
* pBuffer: �ɼ�ֵ����
* Len: �ɼ�ֵ���ϳ���
* MaxChange: �������������仯�ķ�Χ
* return: �˲����ֵ
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
* ��������: һ���ͺ��˲���
* LastOutValue: ��һ���˲������ֵ
* CurValue: ���β�����ֵ
* A: �˲�ϵ��
* return: �˲�������ֵ
* ��ֹƵ�ʼ���: f = A/(2 * PI * t)
* ����: t: �������ʱ��
*/
rt_int32_t FilterOneOrderLagging(rt_int32_t LastOutValue, rt_int32_t CurValue, double A)
{
	rt_int32_t Out = 0;
	Out = A * CurValue + (1 - A) * LastOutValue;
	return Out;
}
