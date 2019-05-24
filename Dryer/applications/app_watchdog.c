

#include "app_watchdog.h"

/* 看 门 狗 设 备名 称 */
#define IWDG_DEVICE_NAME "wdt" 
/* 看 门 狗 设 备 句柄 */
static rt_device_t _wdg_dev; 

static void _idle_hook(void)
{
	/* 在 空 闲 线 程 的 回 调 函 数 里 喂 狗 */
	rt_device_control(_wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
//	rt_kprintf("feed the dog!\n ");
}


void APP_WatchDogInit(void)
{
	rt_err_t ret = RT_EOK;
	/* 溢 出 时 间 */
	rt_uint32_t Timeout = 1000; 
	_wdg_dev = rt_device_find(IWDG_DEVICE_NAME);
	if (!_wdg_dev){
		rt_kprintf("find %s failed!\n", IWDG_DEVICE_NAME);
		return;
	}
	/* 初 始 化 设 备 */
	ret = rt_device_init(_wdg_dev);
	if (ret != RT_EOK){
		rt_kprintf("initialize %s failed!\n", IWDG_DEVICE_NAME);
		return;
	}
	/* 设 置 看 门 狗 溢 出 时 间 */
	ret = rt_device_control(_wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &Timeout);
	if (ret != RT_EOK){
		rt_kprintf("set %s timeout failed!\n", IWDG_DEVICE_NAME);
		return;
	}
	/* 设 置 空 闲 线 程 回 调 函 数 */
	rt_thread_idle_sethook(_idle_hook);
}

