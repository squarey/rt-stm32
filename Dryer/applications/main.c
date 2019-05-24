/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     balanceTWK   change to new framework
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "app_includes.h"
#include "drv_gpio.h"
/* defined the LED0 pin: PC0 */
#define LED0_PIN    GET_PIN(B, 12)

int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
//	APP_ControlInit();
	APP_ADC_Init();
	rt_kprintf("currrnt tick %d\n", rt_tick_get());
	APP_KeyThreadInit();
	APP_WatchDogInit();
//	APP_KeyThreadStart();
	RunControlInit();
	TemperatureThreadInit();
	CurrentThreadInit();
/*	TM1628_AutoDisplayNumber2(1, 12);
	TM1628_AutoDisplayNumber2(2, 34);
	TM1628_AutoDisplayNumber2(3, 56);*/
	
    while (count++)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
		
    }

    return RT_EOK;
}
