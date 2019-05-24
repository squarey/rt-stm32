/*
 * app_adc.h
 *
 *  Created on: 2019Äê2ÔÂ22ÈÕ
 *      Author: Lenovo
 */

#ifndef _APP_ADC_H_
#define _APP_ADC_H_

#include "rtthread.h"
#include "rtdevice.h"


rt_int32_t APP_ADC_Init(void);
rt_int32_t APP_ADC_GetConvertValue(rt_uint8_t Channel, rt_uint32_t *pBuffer, rt_uint32_t Len);
void APP_ADC_Test(void);
void ADC_TestInit(void);

#endif /* APPLICATIONS_APP_ADC_H_ */
