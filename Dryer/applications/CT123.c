

#include "CT123.h"


#define CMD_CT123_INIT  "AT\r\n"
#define CMD_CT123_GET_CURRENT	"AT+C\r\n"
static rt_uint8_t _Inited = 0;
static rt_device_t _UartDevice = RT_NULL;



static rt_uint8_t _CheckCT123Sensor(rt_uint8_t Tries)
{
	char Buff[32] = "";
	rt_uint8_t ReTry = 0;
	do{
		rt_device_write(_UartDevice, 0, CMD_CT123_INIT, rt_strlen(CMD_CT123_INIT));
		rt_thread_mdelay(80);
		rt_device_read(_UartDevice, 0, Buff, 32);
//		rt_kprintf("ReTry:%d, Buff:%s\n", ReTry, Buff);
		if(rt_strstr(Buff, "OK")){
			return 1;
		}
		ReTry++;
		rt_thread_mdelay(50);
	}while(ReTry < Tries);
	return 0;
}
void CT123_Init(void)
{
	rt_uint32_t res;
	
	struct serial_configure Config = RT_SERIAL_CONFIG_DEFAULT;
    /* 查找系统中的串口设备 */
    _UartDevice = rt_device_find("uart3");   
    /* 查找到设备后将其打开 */
    if (_UartDevice != RT_NULL){   
        res = rt_device_open(_UartDevice, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);       
        /* 检查返回值 */
        if (res != RT_EOK){
            rt_kprintf("open %s device error.%d\n", "uart", res);
			return;
        }
		/* 初始化事件对象 */
    }else{
		return;
	}
	Config.baud_rate = BAUD_RATE_9600;
	rt_device_control(_UartDevice, RT_DEVICE_CTRL_CONFIG, &Config);
	rt_thread_mdelay(20);
	_Inited = _CheckCT123Sensor(5);
}

rt_uint8_t CT123_ReadCurrent(char *pCurrent)
{
	if(_Inited){
		char Buff[32] = "";
		rt_device_write(_UartDevice, 0, CMD_CT123_GET_CURRENT, rt_strlen(CMD_CT123_GET_CURRENT));
		rt_thread_mdelay(80);
		if(rt_device_read(_UartDevice, 0, Buff, 32) <= 32){
			char *pString1 = RT_NULL;
			char *pString2 = RT_NULL; 
			rt_uint32_t CpyLen = 0; 
			pString1 = rt_strstr(Buff, "+C=");
			if(RT_NULL == pString1){
				return 1;
			}
			pString2 = rt_strstr(pString1, "\r\n");
			if(RT_NULL == pString2){
				return 1;
			}
			if(pCurrent){
				CpyLen = pString2 - pString1 - 3;
				rt_strncpy(pCurrent, pString1 + 3, CpyLen);
				pCurrent[CpyLen] = '\0';
				//rt_kprintf("Current current %sA\n", pCurrent);
				return 0;
			}else{
				return 1;
			}
		}else{
			return 1;
		}
	}else{
		_Inited = _CheckCT123Sensor(0);
//		rt_kprintf("No module detect\n");
		return 1;
	}
}
