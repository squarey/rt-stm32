

#ifndef DHT22_H
#define DHT22_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

rt_uint8_t  DHT11_Read_Data(rt_uint8_t  *temp, rt_uint8_t  *humi);

#endif /* DHT22_H */

