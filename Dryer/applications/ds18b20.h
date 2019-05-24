

#ifndef _DS18B20_H
#define _DS18B20_H

#include "rtthread.h"


rt_uint8_t DS18B20_Init(void);
rt_int16_t DS18B20_GetTemperature(void);


#endif /* _DS18B20_H */


