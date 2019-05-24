

#include "RunControl.h"
#include "app_key.h"
#include "drv_gpio.h"
#include "TemperatureMonitor.h"
#include "app_tm1628.h"
#include "lcd12864.h"
#include "app_local_flash.h"

#define THREAD_SLEEP_TIME		100

//#define DEF_THREE_PHASE_PIN		GET_PIN(A, 1)

#define USE_RUN_THREAD		0


#define FLAG_RUN_TO_START			1
#define FLAG_RUN_TO_STOP			2
#define FLAG_RUN_TO_WAIT_HOT		3
#define FLAG_RUN_TO_WAIT_STOP		4
#define FLAG_RUN_TO_NONE			0xff

#define DEF_SAVE_DATA_ADDR			1

typedef struct
{
	rt_uint8_t StableTemperature;
	rt_uint8_t FeedTemperature;
	rt_uint8_t HasSaved;
	rt_uint8_t AdjustValue;
}RunControlInfo;

#if USE_RUN_THREAD
static rt_thread_t _RunThread = RT_NULL;
static rt_uint8_t _FlagRun = 0;
static rt_uint32_t _WaitCnt = 0;
static rt_uint32_t _WaitTime = 0;
#endif 


//设备当前的运行状态		非0值表示已启动
static rt_uint8_t _RunStatus = 0;
static rt_uint8_t _ErrorStatus = 0;
static rt_uint8_t _LastErrorStatus = 0;

static rt_sem_t _SemRunStatus = RT_NULL;
static rt_mutex_t _MutexErrorStatus = RT_NULL;
static rt_uint8_t _DispIsSetMode = 0;


static RunControlInfo _ControlInfo = {0};
static void _KeyStartProcess(rt_uint8_t Index, rt_uint8_t PressStatus);
static void _ADCKeySetProcess(rt_uint8_t Index, rt_uint8_t PressStatus);
#if USE_RUN_THREAD
static void _RunThreadEntry(void *p);
#endif 


static void _RunStaticDispInit(void)
{
	LCD12864_ShowString(0, 0, "温度:  ");
	LCD12864_ShowString(1, 0, "加热: 关闭");
	LCD12864_ShowString(1, 5, " 0分 0");
	LCD12864_ShowString(2, 0, "室内:  0℃   0%");
	LCD12864_ShowString(3, 0, "出风:  0℃   0%");
}

static void _LocalDataInit(void)
{
	rt_uint8_t CalAdjustValue = 0;
	APP_LocalFlashRead(DEF_SAVE_DATA_ADDR, (rt_uint32_t *)&_ControlInfo, 1);
	CalAdjustValue = _ControlInfo.StableTemperature + _ControlInfo.FeedTemperature;
	if((CalAdjustValue != _ControlInfo.AdjustValue) && (_ControlInfo.AdjustValue != 0)){
		_ControlInfo.StableTemperature = 65;
		_ControlInfo.FeedTemperature = 55;
	}else{
		rt_kprintf("Last save info:\nStableTemperature:%d, FeedTemperature:%d\n", _ControlInfo.StableTemperature, _ControlInfo.FeedTemperature);
	}
	
	if(_ControlInfo.StableTemperature < 50){
		_ControlInfo.StableTemperature = 65;
		_ControlInfo.FeedTemperature = 45;
	}
}

static void _SaveLocalData(rt_uint8_t StableTemp, rt_uint8_t FeedTemp)
{
	if((_ControlInfo.StableTemperature != StableTemp) || (_ControlInfo.FeedTemperature != FeedTemp)){
		_ControlInfo.StableTemperature = StableTemp;
		_ControlInfo.FeedTemperature = FeedTemp;
		_ControlInfo.AdjustValue = StableTemp + FeedTemp;
		APP_LocalFlashWrite(DEF_SAVE_DATA_ADDR, (rt_uint32_t *)&_ControlInfo, 1);
	}
}
void RunControlInit(void)
{
#if USE_RUN_THREAD
	_RunThread = rt_thread_create(
            "run",			//线程名字
            _RuunThreadEntry,		//线程入口函数
            RT_NULL,				//线程入口函数参数
            1 * 1024,				//线程栈大小
            4,						//线程优先级
            200						//线程时间片
            );
	rt_thread_startup(_RunThread);
#endif 	
	_SemRunStatus = rt_sem_create("run", 1, RT_IPC_FLAG_FIFO);
	if(RT_NULL == _SemRunStatus){
		rt_kprintf("Run sem create fail\n");
	}
	_MutexErrorStatus = rt_mutex_create("error", RT_IPC_FLAG_FIFO);
	if(RT_NULL == _MutexErrorStatus){
		rt_kprintf("Mutex sem create fail\n");
	}
	APP_KeySetPressCallback(_KeyStartProcess);
	APP_ADCKeySetPressCallback(_ADCKeySetProcess);
	
	_LocalDataInit();
	LCD12864_Init();
	_RunStaticDispInit();
}


rt_uint8_t GetRunStatus(rt_uint8_t *pStatus)
{
	if(RT_EOK != rt_sem_take(_SemRunStatus, RT_WAITING_FOREVER)){
		rt_kprintf("GetRunStatus run sem take fail\n");
		return RT_ERROR;
	 }else{
		pStatus[0] = _RunStatus;
	 }
	 rt_sem_release(_SemRunStatus);
	 return RT_EOK;
}

rt_uint8_t SetErrorStatus(rt_uint8_t Status)
{
	if(RT_EOK != rt_mutex_take(_MutexErrorStatus, RT_WAITING_FOREVER)){
		rt_kprintf("SetErrorStatus error mutex take fail\n");
		return RT_ERROR;
	 }
	_ErrorStatus |= Status;
	 //显示错误状态
	if(_ErrorStatus != _LastErrorStatus){
		_LastErrorStatus = _ErrorStatus;
		//TM1628_DisplayError(_LastErrorStatus);
		//rt_thread_mdelay(1000);
		
	}
	rt_mutex_release(_MutexErrorStatus);
	return RT_EOK;
}

rt_uint8_t ClearErrorStatus(rt_uint8_t Status)
{
	if(RT_EOK != rt_mutex_take(_MutexErrorStatus, RT_WAITING_FOREVER)){
		rt_kprintf("ClearErrorStatus error mutex take fail\n");
		return RT_ERROR;
	 }
	_ErrorStatus &= ~Status;
	if(_ErrorStatus != _LastErrorStatus){
		_LastErrorStatus = _ErrorStatus;
		//TM1628_DisplayError(_LastErrorStatus);
		//rt_thread_mdelay(1000);
	}
	rt_mutex_release(_MutexErrorStatus);
	return RT_EOK;
}

rt_uint8_t GetErrorStatus(void)
{
	return _ErrorStatus;
}

static void _KeyStartProcess(rt_uint8_t Index, rt_uint8_t PressStatus)
{
	 if(RT_EOK != rt_sem_take(_SemRunStatus, RT_WAITING_FOREVER)){
		rt_kprintf("run sem take fail\n");
		 return;
	 }
//	 rt_kprintf("Index:%d, PressStatus:%d\n", Index, PressStatus);
	if((0 == Index) && (PressStatus != KEY_PRESS_NONE)){
#if USE_RUN_THREAD
		//如果原来是停止状态 则标记为首次启动状态
		if(0 == _RunStatus){
			rt_kprintf("Start Run\n");
			_FlagRun = FLAG_RUN_TO_START;
		}
#endif 
		_RunStatus = 1;
	}else{
#if USE_RUN_THREAD
		//如果原来是运行状态 则标记为停止状态
		if(1 == _RunStatus){
			rt_kprintf("Stop Run\n");
			_FlagRun = FLAG_RUN_TO_STOP;
		}
#endif 
		_RunStatus = 0;
	}
	rt_sem_release(_SemRunStatus);	
}


static rt_uint8_t _ForceFeed = 0;
static rt_uint32_t _PressTime = 0;
static rt_uint8_t _StableTemp = 0, _FeedTemp = 0;
static rt_uint8_t _FixedHotPower = 0;

static void _RefreshFixHotPowerState(void)
{
	if(0 == _FixedHotPower){
		LCD12864_ShowString(3 , 6, "  ");
		LCD12864_ShowString(3 , 5, "关");
	}else if(1 == _FixedHotPower){
		LCD12864_ShowString(3, 6, "KW");
		LCD12864_ShowString(3, 5, "3 ");
	}else if(2 == _FixedHotPower){
		LCD12864_ShowString(3, 6, "KW");
		LCD12864_ShowString(3, 5, "6 ");
	}else if(3 == _FixedHotPower){
		LCD12864_ShowString(3, 6, "KW");
		LCD12864_ShowString(3, 5, "9 ");
	}
}

static void _ADCKeySetProcess(rt_uint8_t Index, rt_uint8_t PressStatus)
{
	if(KEY_PRESS_NONE != PressStatus){
		//rt_kprintf("Index:%d, PressStatus:%d\n", Index, PressStatus);
		_PressTime = rt_tick_get();
		if(0 == _DispIsSetMode){
			_StableTemp = _ControlInfo.StableTemperature;
			_FeedTemp = _ControlInfo.FeedTemperature;
		}
		if(0x00 == Index){
			if(KEY_PRESS_LONG == PressStatus){
				if(0 == _DispIsSetMode){
					LCD12864_Clear();
					rt_thread_mdelay(3);
					LCD12864_ShowString(0 , 0, "维持温度:   ℃");
					LCD12864_ShowString(1 , 0, "进料温度:   ℃");
					LCD12864_ShowString(2 , 0, "强制进料: ");
					LCD12864_ShowString(2 , 5, _ForceFeed ? "开" : "关");
					LCD12864_ShowString(3 , 0, "固定加热: ");
					_RefreshFixHotPowerState();
					LCD12864_ShowNumber(1, 5, _ControlInfo.FeedTemperature);
					LCD12864_ShowNumber(0, 5, _ControlInfo.StableTemperature);
					LCD12864_ShowCursor();
					//rt_thread_mdelay(5);
					_DispIsSetMode = 1;
				}else{
					LCD12864_HideCursor();
					LCD12864_Clear();
					rt_thread_mdelay(3);
					_RunStaticDispInit();
				//	rt_thread_mdelay(5);
					_DispIsSetMode = 0;
				//	_ForceFeed = 0;
					_SaveLocalData(_StableTemp, _FeedTemp);
				}
			}else{
				if(KEY_PRESS_SHORT == PressStatus){
					if(1 == _DispIsSetMode){
						_DispIsSetMode = 2;
						LCD12864_ShowNumber(1, 5, _ControlInfo.FeedTemperature);
					}else if(2 == _DispIsSetMode){
						_DispIsSetMode = 3;
						LCD12864_ShowString(2 , 5, _ForceFeed ? "开" : "关");
					}else if(3 == _DispIsSetMode){
						_DispIsSetMode = 4;
						_RefreshFixHotPowerState();
					}else if(4 == _DispIsSetMode){
						_DispIsSetMode = 1;
						LCD12864_ShowNumber(0, 5, _ControlInfo.StableTemperature);
					}
				}				
			}
		}else if(0x01 == Index){
			if((0 == _DispIsSetMode) || (5 == _DispIsSetMode)){
				if(KEY_PRESS_LONG == PressStatus){
					if(0 == _DispIsSetMode){
						LCD12864_Clear();
						rt_thread_mdelay(3);
						LCD12864_ShowString(0, 0, "室外:  0℃   0%");
						_DispIsSetMode = 5;
					}else if(5 == _DispIsSetMode){
						LCD12864_Clear();
						rt_thread_mdelay(3);
						_RunStaticDispInit();
					//	rt_thread_mdelay(5);
						_DispIsSetMode = 0;
					}
				}
			}
			if(((KEY_PRESS_SHORT == PressStatus) || (KEY_PRESS_CONTINUE == PressStatus)) && (_DispIsSetMode < 5)){
				if(1 == _DispIsSetMode){
					if(_StableTemp < 99){
						_StableTemp++;
						LCD12864_ShowNumber(0, 5, _StableTemp);
					}
				}else if(2 == _DispIsSetMode){
					if(_FeedTemp < 99){
						_FeedTemp++;
						LCD12864_ShowNumber(1, 5, _FeedTemp);
					}
				}else if(3 == _DispIsSetMode){
					if(1 == _ForceFeed){
						_ForceFeed = 0;
						LCD12864_ShowString(2, 5, "关");
					}else{
						_ForceFeed = 1;
						LCD12864_ShowString(2, 5, "开");
					}
				}else if(4 == _DispIsSetMode){
					if(_FixedHotPower < 3){
						_FixedHotPower++;
						_RefreshFixHotPowerState();
					}
				}
			}
		}else if(0x02 == Index && ((KEY_PRESS_SHORT == PressStatus) || (KEY_PRESS_CONTINUE == PressStatus))){
			if(1 == _DispIsSetMode){
				if(_StableTemp > 50){
					_StableTemp--;
					LCD12864_ShowNumber(0, 5, _StableTemp);
				}
			}else if(2 == _DispIsSetMode){
				if(_FeedTemp > 40){
					_FeedTemp--;
					LCD12864_ShowNumber(1, 5, _FeedTemp);
				}
			}else if(4 == _DispIsSetMode){
				if(_FixedHotPower > 0){
					_FixedHotPower--;
					_RefreshFixHotPowerState();
				}
			}
		}
//		rt_thread_mdelay(200);
		//rt_kprintf("ADC Key 0x%02x\n", Index);
	}else{
		if((_DispIsSetMode != 0) && (_DispIsSetMode != 5)){
			if((rt_tick_get() - _PressTime >= 10000)){
				LCD12864_HideCursor();
				LCD12864_Clear();
				rt_thread_mdelay(3);
				_RunStaticDispInit();
			//	rt_thread_mdelay(5);
				_DispIsSetMode = 0;
				_SaveLocalData(_StableTemp, _FeedTemp);
			}
		}
		//rt_kprintf("....\n");
	}
}
#if USE_RUN_THREAD
static void _RunThreadEntry(void *p)
{
	_SemRunStatus = rt_sem_create("run", 1, RT_IPC_FLAG_FIFO);
	if(RT_NULL == _SemRunStatus){
		rt_kprintf("Run sem create fail\n");
	}
	APP_KeySetPressCallback(_KeyStartProcess);
	rt_pin_mode(DEF_THREE_PHASE_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(DEF_THREE_PHASE_PIN, PIN_LOW);
	while(1){
		switch(_FlagRun){
			//开启三相电机
			case FLAG_RUN_TO_START: 
				//开启三相电机
				rt_pin_write(DEF_THREE_PHASE_PIN, PIN_HIGH);
				_WaitTime = 2000;
				_WaitCnt = 0;
				_FlagRun = FLAG_RUN_TO_WAIT_HOT;
				rt_kprintf("Step1: To open three phase motor\n");
			break;
			case FLAG_RUN_TO_STOP:
				//开启加热
				TemperatureStartHot(0);
				_WaitTime = 20000;
				_WaitCnt = 0;
				_FlagRun = FLAG_RUN_TO_WAIT_STOP;
				rt_kprintf("Step4: To stop, and to close hot funtion\n");
			break;
			case FLAG_RUN_TO_WAIT_HOT:
				if(_WaitCnt < _WaitTime/THREAD_SLEEP_TIME){
					_WaitCnt++;
					rt_kprintf("Step2: To wait open hot, total:%d, now:%d\n", _WaitTime/THREAD_SLEEP_TIME, _WaitCnt);
				}else{
					_FlagRun = FLAG_RUN_TO_NONE;
					//开启加热
					TemperatureStartHot(1);
					rt_kprintf("Step3: To open hot\n");
				}
			break;
			case FLAG_RUN_TO_WAIT_STOP:
				if(_WaitCnt < _WaitTime/THREAD_SLEEP_TIME){
					_WaitCnt++;
					rt_kprintf("Step5: To wait close three phase motor, total:%d, now:%d\n", _WaitTime/THREAD_SLEEP_TIME, _WaitCnt);
				}else{
					_FlagRun = FLAG_RUN_TO_NONE;
					//关闭三相电机
					rt_pin_write(DEF_THREE_PHASE_PIN, PIN_LOW);
				}
			break;
			case FLAG_RUN_TO_NONE:
			break;
			
		}
		
		rt_thread_mdelay(THREAD_SLEEP_TIME);
	}
}
#endif 

rt_uint8_t DispSetModeGet(void)
{
	return _DispIsSetMode;
}

rt_uint8_t GetStableTemperature(void)
{
	return _ControlInfo.StableTemperature;
}

rt_uint8_t GetFeedTemperature(void)
{
	return _ControlInfo.FeedTemperature;
}

rt_uint8_t GetForceFeed(void)
{
	return _ForceFeed;
}

rt_uint8_t GetFixedHotPower(void)
{
	return _FixedHotPower;
}
