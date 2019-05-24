

#ifndef _RUNCONTROL_H
#define _RUNCONTROL_H


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define ERROR_STATUS_TEMPERATUER	0x01
#define ERROR_STATUS_CURRENT		0x02


void RunControlInit(void);
rt_uint8_t GetRunStatus(rt_uint8_t *pStatus);
rt_uint8_t SetErrorStatus(rt_uint8_t Status);
rt_uint8_t ClearErrorStatus(rt_uint8_t Status);
rt_uint8_t GetErrorStatus(void);
rt_uint8_t DispSetModeGet(void);
rt_uint8_t GetStableTemperature(void);
rt_uint8_t GetFeedTemperature(void);
rt_uint8_t GetForceFeed(void);
rt_uint8_t GetFixedHotPower(void);

#endif /* _RUNCONTROL_H */


