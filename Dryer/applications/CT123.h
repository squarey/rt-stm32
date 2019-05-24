

#ifndef _CT123_H
#define _CT123_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


void CT123_Init(void);
rt_uint8_t CT123_ReadCurrent(char *pCurrent);
#endif /* _CT123_H */

