

#ifndef _APP_TM1628_H
#define _APP_TM1628_H

#include "rtthread.h"

void TM1628_Init(void);
void TM1628_DisplayData(rt_uint8_t data, rt_uint8_t addr);
void TM1628_AutoDisplayNumber(rt_uint8_t Data);
void TM1628_DisplayTest(void);
void TM1628_DisplayError(rt_uint8_t Error);
void TM1628_ReadKey(rt_uint8_t Num, rt_uint8_t *pKeyValue);

#if 0
void TM1628_AutoDisplayNumber2(rt_uint8_t Index, rt_uint8_t Data);
#endif 

#endif /* _TM1620_H */
