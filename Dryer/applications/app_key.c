

#include "app_key.h"
#include "drv_gpio.h"
#include "lcd12864.h"
#include "app_adc.h"


#define DEF_KEY_PRESS_LONG_TIME			600		//ms
#define DEF_KEY_PRESS_CONTINUE_TIME		50		//ms

#define DEF_ADC_KEY_CHANNEL				4

#define _KEY_GPIO_Index(gpio, gpio_index)                                \
{                                                                       \
	GPIO##gpio, gpio_index, 0, 0                           \
}

#define _ADC_KEY_Index(min, max)                                \
{                                                                       \
	min, max, 0, 0                           \
}

#define GPIO_TO_INDEX(PORTx, PIN) (rt_base_t)((16 * ( ((rt_base_t)PORTx - (rt_base_t)GPIOA)/(0x0400UL) )) + PIN)

struct KEY_Info
{
	const GPIO_TypeDef *GPIO;
	const rt_uint32_t Pin;
	rt_uint32_t PressTime;
	rt_uint8_t State;
};

struct ADC_Key_Info
{
	const rt_uint32_t ADValueMin;
	const rt_uint32_t ADValueMax;
	rt_uint32_t PressTime;
	rt_uint8_t State;
};

static struct KEY_Info _KeyArray[] = 
{
	_KEY_GPIO_Index(A, 8),
//	_STM32_GPIO_Index(B, 11),
};

static struct ADC_Key_Info _ADCKeyArray[] = 
{
	_ADC_KEY_Index(0, 50),
	_ADC_KEY_Index(1900, 2100),
	_ADC_KEY_Index(2640, 2750),
};

static rt_thread_t _pKeyThread = RT_NULL;
static void _KeyThreadEntry(void *p);
static rt_uint8_t _KeyCnt = 0;	//总共按键的个数
static void(*_pKeyStatusCallback)(rt_uint8_t KeyIndex, rt_uint8_t PressStatus) = RT_NULL;
static void(*_pADCKeyStatusCallback)(rt_uint8_t KeyIndex, rt_uint8_t PressStatus) = RT_NULL;

void APP_KeyThreadInit(void)
{
    _pKeyThread = rt_thread_create(
            "key",					//线程名字
            _KeyThreadEntry,		//线程入口函数
            RT_NULL,				//线程入口函数参数
            1 * 512,				//线程栈大小
            5,						//线程优先级
            200						//线程时间片
            );
	rt_thread_startup(_pKeyThread);
}


void APP_KeyThreadExit(void)
{
    rt_thread_delete(_pKeyThread);
}

void APP_KeySetPressCallback(void (*pFunc)(rt_uint8_t KeyIndex, rt_uint8_t PressStatus))
{
	_pKeyStatusCallback = pFunc;
}
void APP_ADCKeySetPressCallback(void (*pFunc)(rt_uint8_t KeyIndex, rt_uint8_t PressStatus))
{
	_pADCKeyStatusCallback = pFunc;
}
static void _KeyPinInit(void)
{
	rt_uint8_t i = 0;
	_KeyCnt = (rt_uint8_t)(sizeof(_KeyArray)/(sizeof(struct KEY_Info)));
	rt_kprintf("_KeyInfo size %d\n", _KeyCnt);
	for(i = 0; i < _KeyCnt; i++){
		rt_pin_mode(GPIO_TO_INDEX(_KeyArray[i].GPIO, _KeyArray[i].Pin), PIN_MODE_INPUT_PULLUP);
	}
	
}
static rt_uint8_t _KeyGetStatus(struct KEY_Info *pInfo)
{
	if(0 == rt_pin_read(GPIO_TO_INDEX(pInfo->GPIO, pInfo->Pin))){
		//判断原来按键是否被按下
		if(0 == pInfo->PressTime){
			pInfo->PressTime = rt_tick_get();
			return KEY_PRESS_NONE;
		}else if(((rt_tick_get() - pInfo->PressTime) >= DEF_KEY_PRESS_LONG_TIME) && 
				(0x80 != (0x80 & pInfo->State))){
			pInfo->PressTime = rt_tick_get();
			pInfo->State |= 0x80;
			return KEY_PRESS_LONG;
		}else if(((rt_tick_get() - pInfo->PressTime) >= 20) && 
				(0x01 != (0x01 & pInfo->State))){
			pInfo->State |= 0x01;
			return KEY_PRESS_SHORT;
		}else if(((rt_tick_get() - pInfo->PressTime) >= DEF_KEY_PRESS_CONTINUE_TIME) && 
				(0x80 == (0x80 & pInfo->State))){
			pInfo->PressTime = rt_tick_get();
			return KEY_PRESS_CONTINUE;	
		}else if(0 != pInfo->State){
			return KEY_PRESS_CHECKING;
		}else {
			return KEY_PRESS_NONE;
		}
	}else{
		pInfo->PressTime = 0;
		pInfo->State = 0x00;
		return KEY_PRESS_NONE;
	}
}
static void _ADC_KeyCheck(struct ADC_Key_Info *pInfo, rt_uint8_t Len)
{
	rt_uint32_t tBuff[10];
	rt_uint8_t PressState = KEY_PRESS_NONE;
	if(10 == APP_ADC_GetConvertValue(DEF_ADC_KEY_CHANNEL, tBuff, 10)){
		rt_uint8_t i = 0;
		rt_uint32_t Sum = 0;
		for(i = 0; i < 10; i++){
			Sum += tBuff[i];
		}
		Sum = Sum/10;
		for(i = 0; i < Len; i++){
			if((Sum >= pInfo[i].ADValueMin) && (Sum <= pInfo[i].ADValueMax)){
				if(0 == pInfo[i].PressTime){
					pInfo[i].PressTime = rt_tick_get();
					PressState =  KEY_PRESS_NONE;
					break;
				}else if(((rt_tick_get() - pInfo[i].PressTime) >= DEF_KEY_PRESS_LONG_TIME) && 
						(0x80 != (0x80 & pInfo[i].State))){
					pInfo[i].PressTime = rt_tick_get();
					pInfo[i].State |= 0x80;
					PressState =  KEY_PRESS_LONG;
					break;
				}else if(((rt_tick_get() - pInfo[i].PressTime) >= 20) && 
						(0x01 != (0x01 & pInfo[i].State))){
					pInfo[i].State |= 0x01;
					PressState =  KEY_PRESS_SHORT;
					break;
				}else if(((rt_tick_get() - pInfo[i].PressTime) >= DEF_KEY_PRESS_CONTINUE_TIME) && 
						(0x80 == (0x80 & pInfo[i].State))){
					pInfo[i].PressTime = rt_tick_get();
					PressState =  KEY_PRESS_CONTINUE;
					break;
				}/*else{
					Ret = KEY_PRESS_NONE;
				}*/
			}else{
				pInfo[i].PressTime = 0;
				pInfo[i].State = 0;
			}
		}
//		if(PressState != KEY_PRESS_NONE){
//			rt_kprintf("adc key %d, state:%d\n", i, PressState);
//		}
		if(_pADCKeyStatusCallback){
			_pADCKeyStatusCallback(i, PressState);
		}
	}
	
}
static void _KeyThreadEntry(void *p)
{
    rt_uint8_t Status = 0;
	rt_uint8_t i = 0;
	_KeyPinInit();
	rt_kprintf("key_thread_entry\n");
	while(1){
		for(i = 0; i < _KeyCnt; i++){
			Status = _KeyGetStatus(&_KeyArray[i]);
			
//			switch(Status){
//				case KEY_PRESS_NONE: //按键没有被按下
//				break;
//				case KEY_PRESS_SHORT: //按键短按
//					rt_kprintf("key %d shot press\n", i + 1);
//				break;
//				case KEY_PRESS_LONG: //按键长按
//					rt_kprintf("key %d long press\n", i + 1);
//				break;
//				case KEY_PRESS_CONTINUE: //连续按
//					rt_kprintf("key %d press continue\n", i + 1);
//				break;
//				default:
//				break;
//			}
			
			if((_pKeyStatusCallback) && (Status < KEY_PRESS_CHECKING)){
				_pKeyStatusCallback(i, Status);
			}
		}
		_ADC_KeyCheck(_ADCKeyArray, sizeof(_ADCKeyArray)/sizeof(struct ADC_Key_Info));
		rt_thread_mdelay(10);
	}
}

