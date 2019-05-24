

#ifndef GY39_H
#define GY39_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


typedef struct
{
	rt_uint32_t Pressure;
	rt_uint16_t Temperature;
	rt_uint16_t Humidity;
	rt_uint16_t Altitude;
}GY39_Data;


rt_uint8_t GY39_GetData(GY39_Data *pData);

#endif /* GY39_H */

