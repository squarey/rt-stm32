

#ifndef _TEMPERATURE_MONITOR_H
#define _TEMPERATURE_MONITOR_H


#include "rtthread.h"

void TemperatureThreadInit(void);
rt_uint8_t GetCurrentTemperature(void);

#endif /* _TEMPERATURE_MONITOR_H */

