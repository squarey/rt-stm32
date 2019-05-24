

#include "app_watchdog.h"

/* �� �� �� �� ���� �� */
#define IWDG_DEVICE_NAME "wdt" 
/* �� �� �� �� �� ��� */
static rt_device_t _wdg_dev; 

static void _idle_hook(void)
{
	/* �� �� �� �� �� �� �� �� �� �� �� ι �� */
	rt_device_control(_wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
//	rt_kprintf("feed the dog!\n ");
}


void APP_WatchDogInit(void)
{
	rt_err_t ret = RT_EOK;
	/* �� �� ʱ �� */
	rt_uint32_t Timeout = 1000; 
	_wdg_dev = rt_device_find(IWDG_DEVICE_NAME);
	if (!_wdg_dev){
		rt_kprintf("find %s failed!\n", IWDG_DEVICE_NAME);
		return;
	}
	/* �� ʼ �� �� �� */
	ret = rt_device_init(_wdg_dev);
	if (ret != RT_EOK){
		rt_kprintf("initialize %s failed!\n", IWDG_DEVICE_NAME);
		return;
	}
	/* �� �� �� �� �� �� �� ʱ �� */
	ret = rt_device_control(_wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &Timeout);
	if (ret != RT_EOK){
		rt_kprintf("set %s timeout failed!\n", IWDG_DEVICE_NAME);
		return;
	}
	/* �� �� �� �� �� �� �� �� �� �� */
	rt_thread_idle_sethook(_idle_hook);
}

