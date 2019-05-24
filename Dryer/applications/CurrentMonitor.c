

#include "CurrentMonitor.h"
#include "app_uart.h"
#include "RunControl.h"
#include "drv_gpio.h"
#include "TemperatureMonitor.h"
#include "lcd12864.h"
#include "app_adc.h"
#include "GY39.h"
#include "CT123.h"
#include "SHT2x.h"
#include "I2C.h"



#define DEF_ADC_CHANNEL		9

#define DEF_NORMAL_AD_MAX_VALUE		2550
#define DEF_NORMAL_AD_MIN_VALUE		1350
#define DEF_ERROR_AD_MAX_VALUE		4000
#define DEF_ERROR_AD_MIN_VALUE		100

#define USE_CURRENT_MONITOT		1

#define DEF_START_FEED_TEMP			55			//所需到达的进料的温度
#define DEF_STOP_FEED_MIN_TIME		8000		//停止进料最少持续时间	ms
#define DEF_START_FEED_MIN_TIME		8000		//开始进料最少持续时间	ms
#define DEF_FEED_CONTINUE_TIME		60000		//连续进料DEF_FEED_CONTINUE_TIME后强制停止进料DEF_STOP_FEED_MIN_TIME时间 ms
#define FEED_PIN		GET_PIN(B, 13)


static rt_thread_t _CurrentThread = RT_NULL;
static void _CurrentThreadEntry(void *p);

void CurrentThreadInit(void)
{
	_CurrentThread = rt_thread_create(
            "Current",				//线程名字
            _CurrentThreadEntry,	//线程入口函数
            RT_NULL,				//线程入口函数参数
            1 * 512,				//线程栈大小
            4,						//线程优先级
            300						//线程时间片
            );
	rt_thread_startup(_CurrentThread);
}

static void _OpenFeed(rt_uint8_t Force)
{
	if((0 == GetForceFeed()) || Force){
		rt_pin_write(FEED_PIN, PIN_HIGH);
	}
}

static void _CloseFeed(rt_uint8_t Force)
{
	if((0 == GetForceFeed()) || Force){
		rt_pin_write(FEED_PIN, PIN_LOW);
	}
}


#include "math.h"
#include <stdlib.h>

static rt_uint32_t _CurrentADBaseValue = 0;
static rt_uint8_t _GetCurrentValue(rt_uint32_t *pValue)
{
	rt_uint32_t ADValue = 0;
	rt_uint32_t i = 0;
	rt_device_t _DevADC = RT_NULL;
//	rt_uint32_t CurTick = 0;
	rt_uint32_t TotalValue = 0;
	rt_uint32_t TempValue = 0;	
	rt_uint32_t AverageValue = 0;
	rt_uint8_t RunState = 0;
	float CurrentValue = 0.0f;
	char CrrentStringBuff[10];
	//char Buffer[10];
	if(RT_ERROR == GetRunStatus(&RunState)){
		return 1;
	}
	_DevADC = rt_device_find("adc1");
	if(RT_NULL == _DevADC){
		rt_kprintf("Cannot find adc1 device\n");
		return 1;
	}
	rt_device_open(_DevADC, RT_DEVICE_OFLAG_RDWR);
//	CurTick = rt_tick_get();
	//rt_kprintf("Start ADC CurTick:%d\n", CurTick);
	//读取当前电流值
	for(i = 0; i < 256; i++){
		if(1 == rt_device_read(_DevADC, DEF_ADC_CHANNEL, &ADValue, 1)){
			AverageValue += ADValue;
			if(RunState && (_CurrentADBaseValue > 2000)){
				TempValue = (ADValue > _CurrentADBaseValue) ? (ADValue - _CurrentADBaseValue) : (_CurrentADBaseValue - ADValue);
				TotalValue += (TempValue * TempValue);
			}
			//rt_kprintf("TotalValue:%d, TempValue:%d\n", TotalValue, TempValue);
		}
		rt_hw_us_delay(62);
	}
	//未启动的时候计算偏压值
	if(0 == RunState){
		_CurrentADBaseValue = AverageValue/256;
	}
//	rt_kprintf("Convert ADC take %dms, AverageValue%d, TotalValue %d\n", rt_tick_get() - CurTick, AverageValue/256, TotalValue);
	if(AverageValue > 100){
		TotalValue /= 256;
		if(TotalValue){
			TotalValue = sqrt(TotalValue) + 50;
		}
		//ACS712-20A  1A电流对应的AD值为122.212
		CurrentValue = (float)TotalValue/122.212f;
	}else{
		CurrentValue = 0.00f;
	}
	
	sprintf(CrrentStringBuff, "%4.2fA", CurrentValue);
	//rt_kprintf("Current Value %s\n", CrrentStringBuff);
	LCD12864_ShowString(0, 5, CrrentStringBuff);
	//CurTick = rt_tick_get();
	rt_device_close(_DevADC);
//	rt_kprintf("MaxValue:%d,   MinValue:%d\n", MaxValue, MinValue);
	return 0;
}
static void _GetHumidity(void)
{
	GY39_Data tData = {0};
	rt_uint8_t Temp = 0, Humi = 0;
	if(0 == GY39_GetData(&tData)){
		//rt_kprintf("Temp %d, Humi:%d\n", tData.Temperature, tData.Humidity);
		LCD12864_ShowNumber(2, 3, tData.Temperature/100);
		Humi = tData.Humidity/100;
		LCD12864_ShowNumber(2, 6, (Humi < 95) ? (Humi + 5) : Humi);
	}/*else{
		LCD12864_ShowNumber(2, 3, 0);
		LCD12864_ShowNumber(2, 6, 0);
	}*/
	if(0 == SHT2x_ReadTemperatureHumidity(&Temp, &Humi)){
		LCD12864_ShowNumber(3, 3, Temp);
		LCD12864_ShowNumber(3, 6, (Humi > 99) ? 99 : Humi);
	}
}

static void _RefreshHotPower(rt_uint8_t RunState)
{
	char CBuffer[10] = "0";
	rt_uint32_t PowerValue = 0;
	if(0 == CT123_ReadCurrent(CBuffer)){
		if(RunState){
			float CurrentValue = 0.0f;
		//	LCD12864_ShowString(1, 5, CBuffer);
			CurrentValue = atof(CBuffer);
			PowerValue = (rt_uint32_t)(CurrentValue * 220);
			rt_memset(CBuffer, 0, 10);
			sprintf(CBuffer, "%4.1f", (float)PowerValue/1000);
			LCD12864_ShowString(1, 3, CBuffer);
		}else{
			LCD12864_ShowString(1, 3, "关闭");
		}
	}else{
		LCD12864_ShowString(1, 3, "ERR ");
	}
}

#define I2C2_CLK_PIN	GET_PIN(B, 6)
#define I2C2_SDA_PIN	GET_PIN(B, 7)

static void _I2C2_CLK_HIGH(void)
{
	rt_pin_write(I2C2_CLK_PIN, PIN_HIGH);
}
static void _I2C2_CLK_LOW(void)
{
	rt_pin_write(I2C2_CLK_PIN, PIN_LOW);
}
static void _I2C2_SDA_HIGH(void)
{
	rt_pin_write(I2C2_SDA_PIN, PIN_HIGH);
}

static void _I2C2_SDA_LOW(void)
{
	rt_pin_write(I2C2_SDA_PIN, PIN_LOW);
}

static void _I2C2_SDA_INPUT(void)
{
	rt_pin_mode(I2C2_SDA_PIN, PIN_MODE_INPUT);
}

static void _I2C2_SDA_OUTPUT(void)
{
	rt_pin_mode(I2C2_SDA_PIN, PIN_MODE_OUTPUT);
}

static rt_uint8_t _I2C2_SDA_DATA(void)
{
	return rt_pin_read(I2C2_SDA_PIN);
}

static void _RefreshExitState(void)
{
	rt_uint8_t Temp = 0, Humi = 0;
	I2C_GPIOFunc Func = {
		_I2C2_CLK_HIGH, _I2C2_CLK_LOW, 
		_I2C2_SDA_HIGH, _I2C2_SDA_LOW, 
		_I2C2_SDA_INPUT, _I2C2_SDA_OUTPUT,
		_I2C2_SDA_DATA
	};
	
	rt_pin_mode(I2C2_CLK_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(I2C2_SDA_PIN, PIN_MODE_OUTPUT);
	I2C_SetGPIOFunc(&Func, 1);
	if(0 == SHT2x_ReadTemperatureHumidity(&Temp, &Humi)){
		LCD12864_ShowNumber(0, 3, Temp);
		LCD12864_ShowNumber(0, 6, Humi);
	}
	I2C_SetGPIOFunc(&Func, 0);
}

static void _CurrentThreadEntry(void *p)
{

	rt_uint8_t RunStatus = 0;
	rt_uint8_t FeedOpenFlag = 0;
	rt_uint32_t StartFeedTime = 0;
	rt_uint32_t StopFeedTime = 0;
	rt_uint32_t TempTime = 0;
	rt_uint8_t TempFlag = 0;
	rt_uint32_t CurrentValue = 0;
	rt_uint32_t _LastConvertTime = 0;
	rt_pin_mode(FEED_PIN, PIN_MODE_OUTPUT);
	_CloseFeed(1);
	CT123_Init();
	while(1){
		if(RT_EOK == GetRunStatus(&RunStatus)){
			if((rt_tick_get() - _LastConvertTime) > 1000){
				if(0 == DispSetModeGet()){
					_LastConvertTime = rt_tick_get();
					_GetCurrentValue(&CurrentValue);
					_GetHumidity();
					_RefreshHotPower(RunStatus);
				}else if(5 == DispSetModeGet()){
					_RefreshExitState();
				}
			}
			if(RunStatus){
				//rt_kprintf("FeedOpenFlag:%d, Temperature:%d, FeedTemperature:%d\n", FeedOpenFlag, GetCurrentTemperature(), GetFeedTemperature());
				//当温度持续低于进料温度一定时间后再停止进料
				//当温度持续高于进料温度一定时间后再开始进料
				if(GetCurrentTemperature() >= (GetFeedTemperature() + 5)){
					if((0 == TempFlag) || (3 == TempFlag)){
						TempFlag = 1;
						TempTime = rt_tick_get();
					}else if(1 == TempFlag){
						if(rt_tick_get() - TempTime >= 3000){
							TempFlag = 2;
						}
					}
				}else if(GetCurrentTemperature() < GetFeedTemperature()){
					if((2 == TempFlag) || (1 == TempFlag)){
						TempTime = rt_tick_get();
						TempFlag = 3;
					}else if(3 == TempFlag){
						if(rt_tick_get() - TempTime >= 3000){
							TempFlag = 0;
						}
					}
				}
				//rt_kprintf("TempFlag:%d, FeedOpenFlag:%d\n", TempFlag, FeedOpenFlag);
				if(2 == TempFlag){
					if(0 == FeedOpenFlag){
						FeedOpenFlag = 1;
						//获取当前启动的时间点
						StartFeedTime = rt_tick_get();
						//rt_kprintf("Line：%d, 温度大于53度, 需要开启进料!\n", __LINE__);
					}
				}else if(0 == TempFlag){
					if(2 == FeedOpenFlag){
						//获取当前停止的时间点
						FeedOpenFlag = 3;
						StopFeedTime = rt_tick_get();
						//rt_kprintf("Line：%d, 温度低于53度, 需要关闭进料!\n", __LINE__);
					}else if(0 == FeedOpenFlag){
					}
				}
				if(GetForceFeed()){
					_OpenFeed(1);
				}else if(0 == FeedOpenFlag){
				
				}else if(1 == FeedOpenFlag){
					_OpenFeed(0);
					//进料时间大于6s才能停止进料
					if((rt_tick_get() - StartFeedTime) >= DEF_START_FEED_MIN_TIME){
						FeedOpenFlag = 2;
					}
					//rt_kprintf("Line：%d, 正在进料..., 进料时间%d\n", __LINE__, (rt_tick_get() - StartFeedTime)/1000);
//						LCD12864_ShowNumber(0, 6, FeedTimeRecorde);
					//有料的时候 温度会变低  当温度低于50度的时候停止进料
				}else if(3 == FeedOpenFlag){
					//至少要停止6s
					if((rt_tick_get() - StopFeedTime) >= DEF_STOP_FEED_MIN_TIME){
						FeedOpenFlag = 0;
					}
				//	StartFeedTime = rt_tick_get();
					_CloseFeed(0);
					//rt_kprintf("Line：%d, 停止进料, 停止时间%d\n", __LINE__, (rt_tick_get() - StopFeedTime)/1000);
//						LCD12864_ShowString(1, 6, "    ");
//						LCD12864_ShowNumber(1, 6, FeedTimeRecorde);
				}/*else if(FeedOpenFlag < 4){
					if(0 == DispSetModeGet()){
						sprintf(StringBuff, "3 正在进料...%d", (rt_tick_get() - StartFeedTime)/1000);
						LCD12864_ShowString(3, 0, StringBuff);
					}
					_OpenFeed();
					//如果连续进料1分钟  则强制停止进料一定时间
					//rt_kprintf("Line：%d, 正在进料..., 进料时间%d\n", __LINE__, (rt_tick_get() - StartFeedTime)/1000);
					if((rt_tick_get() - StartFeedTime) >= DEF_FEED_CONTINUE_TIME){
						FeedOpenFlag = 3;
						StopFeedTime = rt_tick_get();
						//rt_kprintf("Line:%d, 进料超过一分钟, 强制停止进料\n", __LINE__);
					}
				}*/
			}else{
				StopFeedTime = rt_tick_get();
				StartFeedTime = StopFeedTime;
				FeedOpenFlag = 0;
				TempFlag = 0;
				_CloseFeed(0);
			}		
		}else{
			StopFeedTime = rt_tick_get();
			StartFeedTime = StopFeedTime;
			FeedOpenFlag = 0;
			TempFlag = 0;
			_CloseFeed(1);
		}
		rt_thread_mdelay(300);
	}
}

