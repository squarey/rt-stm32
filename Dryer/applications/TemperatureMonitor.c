

#include "TemperatureMonitor.h"
#include "NTC_10K_3950_R_Value.h"
#include "app_tm1628.h"
#include "app_adc.h"
#include "stdio.h"
#include "app_key.h"
#include "drv_gpio.h"
#include "RunControl.h"
#include "lcd12864.h"
#include "Filter.h"


#define DEF_PWM_PERIOD		100000000		//默认PWM的周期 单位ns
#define DEF_TEMP_ADC_CHANNEL 8
#define PWM_DEV_NAME 			"pwm2"

#define PWM_HOT_3KW_CHANNEL		1			//三千瓦加热继电器
#define PWM_HOT_6KW_CHANNEL		2			//六千瓦加热继电器
#define PWM_FEED_MOTOR_CHANNEL	3			//进料电机

#define SENSOR_REF_R_VALUE		1000		//热敏电阻串联电阻的阻值


#define DEF_STABLE_TEMPERATURE		60

#define HOT_3KW_PIN			GET_PIN(B, 14)		//加热3KW引脚
#define HOT_6KW_PIN			GET_PIN(B, 15)		//加热6WK引脚

#define CONVERTER_PIN		GET_PIN(A, 11)		//变频器控制引脚
#define SMASH_PIN			GET_PIN(A, 12)		//打碎电机引脚


typedef struct 
{
	rt_uint32_t	SetValue;				//设定值
	rt_int32_t 	OutMax;				//最大输出值
	rt_int32_t  OutMin;				//最小输出值
	rt_int32_t  LastError;				//上一次的误差值
	float  OutputValue;					//输出值
	float  IntegralValue;				//积分值
	float Kp, Ki, Kd;					//系数
}PID_Info;

static rt_thread_t _TemperatureThread = RT_NULL;
static rt_uint8_t _LastTemperature = 90;


static void _TemperatureThreadEntry(void *p);


void TemperatureThreadInit(void)
{
	_TemperatureThread = rt_thread_create(
            "temp",			//线程名字
            _TemperatureThreadEntry,		//线程入口函数
            RT_NULL,				//线程入口函数参数
            1 * 1024,				//线程栈大小
            5,						//线程优先级
            300						//线程时间片
            );
	rt_thread_startup(_TemperatureThread);
}

rt_uint8_t GetCurrentTemperature(void)
{
	return _LastTemperature;
}
/*
根据ADC通道获取AD值
Channel: 所要获取的ADC的通道
pValue: 获取到的AD值
Flag: 滤波算法  1: 中值滤波  其他值: 均值滤波
return: 0 获取成功  1 获取失败
*/
static rt_uint8_t _GetADCValueByChannel(rt_uint8_t Channel, rt_uint32_t *pValue, rt_uint8_t Flag)
{
	rt_uint32_t ADCBuffer[10] = {0};
	if(10 == APP_ADC_GetConvertValue(Channel, ADCBuffer, 10)){
		if(Flag){
			pValue[0] = FilterMiddleValue((rt_int32_t *)ADCBuffer, 10);
		}else{
			pValue[0] = FilterAverageValue((rt_int32_t *)ADCBuffer, 10);
		}
		return 0;
	}else{
		return 1;
	}
}
/*
根据AD值获取相对应的温度
RefValue: 参考电压的AD值
ADValue: 热敏电阻的AD值
pRValue: 计算后的电阻值
return: 计算后的温度值
*/
static rt_uint8_t _GetTemperatureByADValue(rt_uint32_t RefValue, rt_uint32_t ADValue, rt_uint32_t *pRValue)
{
	if(RefValue == ADValue){
		if(pRValue){
			pRValue[0] = TemperatureMinRValue[0] + 10000;
		}
		return 0;
	}else{
		rt_uint32_t RValue = 0;
		rt_uint8_t Temp = 0;
		//根据AD值计算NTC的电阻值
		RValue = (SENSOR_REF_R_VALUE * ADValue)/(RefValue - ADValue);
		//查询方式获取温度
		for(Temp = 0; Temp < 99; Temp++){
			if(RValue > TemperatureMinRValue[Temp]){
				break;
			}
		}
		if(pRValue){
			pRValue[0] = RValue;
		}
		return Temp;
	}
}

static void _RefreshHotPower(rt_uint8_t Power)
{
	switch(Power){
		case 3:
			rt_pin_write(HOT_3KW_PIN, PIN_HIGH);
			rt_pin_write(HOT_6KW_PIN, PIN_LOW);
		break;
		case 6:
			rt_pin_write(HOT_3KW_PIN, PIN_LOW);
			rt_pin_write(HOT_6KW_PIN, PIN_HIGH);
		break;
		case 9:
			rt_pin_write(HOT_3KW_PIN, PIN_HIGH);
			rt_pin_write(HOT_6KW_PIN, PIN_HIGH);
		break;
		default:
			rt_pin_write(HOT_3KW_PIN, PIN_LOW);
			rt_pin_write(HOT_6KW_PIN, PIN_LOW);
		break;
	}
}

static void _OpenConverter(void)
{
	rt_pin_write(CONVERTER_PIN, PIN_HIGH);
	rt_pin_write(SMASH_PIN, PIN_HIGH);
}

static void _CloseConverter(void)
{
	rt_pin_write(CONVERTER_PIN, PIN_LOW);
	rt_pin_write(SMASH_PIN, PIN_LOW);
}

static void _TempPIDInit(PID_Info *pInfo)
{
	pInfo->SetValue = 65;
	pInfo->IntegralValue = 0.0f;
	pInfo->OutMax = 9;
	pInfo->OutMin = 3;
	pInfo->LastError = 0;
	pInfo->Kd = 0.6f;
	pInfo->Ki = 0.3f;
	pInfo->Kd = 0.3f;
	pInfo->OutputValue = 3;
}

static rt_uint8_t _TempPIDProcess(PID_Info *pInfo, rt_uint8_t Temp)
{
	rt_int32_t Error = 0;
	float Index = 0.0f;
//	float OutValue = 0.0;
	float TempIndex = 0.0f;
	char fString[10] = "";
	rt_int32_t Flag = 0;
	Error = pInfo->SetValue - Temp;
	
	/***********************************
	Error		LastError		Flag
	0			1				-1
	-1			0				-1
	1			0				1
	0			-1				1
	************************************/
	
	Flag = Error - pInfo->LastError;
	if(pInfo->OutputValue >= pInfo->OutMax){
		if(Error < 0){
			pInfo->IntegralValue += Error;
		}
	}else if(pInfo->OutputValue <= pInfo->OutMin){
		if(Error > 0){
			pInfo->IntegralValue += Error;
		}
	}else{
		pInfo->IntegralValue += Error;
		//Index = (float)(pInfo->ErrorMax - Error)/(pInfo->ErrorMax - pInfo->ErrorMin);
	}
//	TempIndex = Index;
//	if(pInfo->IntegralValue >= 30){
//		if(Error > 0){
//			Index = 0.0f;
//		}
//	}else if(pInfo->IntegralValue <= -30){
//		if(Error < 0){
//			Index = 0.0f;
//		}
//	}
//	if(pInfo->IntegralValue > 30){
//		pInfo->IntegralValue = 30;
//	}else if(pInfo->IntegralValue < -30){
//		pInfo->IntegralValue = -30;
//	}
//	pInfo->IntegralValue += Index * Error;
	
	sprintf(fString, "%f", Index);
	rt_kprintf("Index:%s ", fString);
	sprintf(fString, "%f", pInfo->IntegralValue);
	rt_kprintf("IntegralValue:%s \n", fString);
	pInfo->OutputValue = pInfo->Kd * Error + pInfo->Ki * pInfo->IntegralValue + pInfo->Kd * (Error - pInfo->LastError);
	
	pInfo->LastError = Error;
	
	sprintf(fString, "%f", pInfo->OutputValue);
	rt_kprintf("Current Temp %d, PID OutValue %s\n", Temp, fString);
}

static void _TemperatureThreadEntry(void *p)
{
	char VBuffer[10] = {0};
	rt_uint8_t RunStatus = 0;
//	rt_uint8_t ChannelIndex = 0;
//	rt_uint8_t ChannelTempBuff[3] = {1, 1, 1};
	rt_uint8_t Temp = 0;
	rt_uint16_t TempErrorContinueTime = 0;
	rt_uint32_t ADCValue = 0;
	rt_uint32_t RValue = 0;
	rt_uint32_t BaseADCValue = 4030;
	rt_uint32_t HotStateTime = 0;
	rt_uint32_t StartRunTime = 0;
	rt_uint8_t FlagHotState = 0;
	rt_int32_t ADCStoreBuffer[3] = {0};
	rt_uint8_t Cnt3 = 0;
	rt_uint8_t FlagTempPower = 0;
	rt_uint8_t FlagHot = 0;
	rt_uint8_t StableTempCnt = 0;
	rt_uint8_t LocalRunStatus = 0;
	rt_uint32_t DelayTimeRecorde = 0;
	rt_pin_mode(HOT_3KW_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(HOT_6KW_PIN, PIN_MODE_OUTPUT);
	_RefreshHotPower(0);
	
	rt_pin_mode(CONVERTER_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(SMASH_PIN, PIN_MODE_OUTPUT);
	_CloseConverter();
	
	PID_Info TempPIDInfo;
	rt_uint8_t FlagPID = 0;
	_TempPIDInit(&TempPIDInfo);
	while(1){
		//获取ADC的参考电压
		if(_GetADCValueByChannel(7, &BaseADCValue, 0)){
			BaseADCValue = 4030;
		}
		//rt_kprintf("BaseADCValue:%d\n", BaseADCValue);
		if(0 == _GetADCValueByChannel(DEF_TEMP_ADC_CHANNEL, &ADCValue, 1)){
			sprintf(VBuffer, "%f", (float)(ADCValue * 3.3f)/BaseADCValue);
			if(Cnt3 < 2){
				ADCStoreBuffer[Cnt3] = ADCValue;
				Cnt3++;
			}else{
				rt_uint32_t TempADCValue = 0;
				//rt_kprintf("before ADCValue %d\n", ADCValue);
				TempADCValue = FilterLimitAmplitude2(ADCStoreBuffer[0], ADCStoreBuffer[1], ADCValue, 60);
				ADCStoreBuffer[0] = ADCStoreBuffer[1];
				ADCStoreBuffer[1] = ADCValue;
				ADCValue = TempADCValue;
			}
			Temp = _GetTemperatureByADValue(BaseADCValue, ((ADCValue > BaseADCValue) ? BaseADCValue : ADCValue), &RValue);
			//将温度值送至显示
			/*if((_LastTemperature != Temp) && (0 == DispSetModeGet())){
				_LastTemperature = Temp;
				LCD12864_ShowNumber(0, 3, _LastTemperature);
	//			TM1628_AutoDisplayNumber(_LastTemperature);
			}else{
				if(DispSetModeGet()){
					_LastTemperature = 3;
				}
			}*/
			_LastTemperature = Temp;
			if(0 == DispSetModeGet()){
				LCD12864_ShowNumber(0, 3, _LastTemperature);
			}
			//_LastTemperature = i;
			if((Temp <= 0) || (Temp >= 99)){
				TempErrorContinueTime++;
				if(TempErrorContinueTime >= 50){
					SetErrorStatus(ERROR_STATUS_TEMPERATUER);
				}
			}else{
				TempErrorContinueTime = 0;
				ClearErrorStatus(ERROR_STATUS_TEMPERATUER);
				//TM1628_AutoDisplayNumber(_LastTemperature);
				
			}
			//rt_kprintf("Read temp adc value %d, vol %s, RValue %d, temp %d, 0x%02x\n", ADCValue, VBuffer, RValue, _LastTemperature, GetErrorStatus());
		}else{
			rt_kprintf("Read temp adc value fail\n");
		}
//		for(ChannelIndex = 0; ChannelIndex < 1; ChannelIndex++){
//			if(0 == _GetADCValueByChannel(ChannelIndex, &ADCValue, 1)){
//				sprintf(VBuffer, "%f", (float)(ADCValue * 3.3f)/BaseADCValue);
//				Temp = _GetTemperatureByADValue(BaseADCValue, ((ADCValue > BaseADCValue) ? BaseADCValue : ADCValue), &RValue);
//				if(ChannelTempBuff[ChannelIndex] != Temp){
//					ChannelTempBuff[ChannelIndex] = Temp;
//					//TM1628_AutoDisplayNumber2(ChannelIndex + 1, Temp);
//					if(0 == DispSetModeGet()){
//						LCD12864_ShowNumber(0, 5 + ChannelIndex * 2, Temp);
//					}
//				}
//				//rt_kprintf("Channel %d Read temp adc value %d, vol %s, RValue %d, temp %d\n", ChannelIndex, ADCValue, VBuffer, RValue, Temp);
//			}
//		}
		//获取当前设备的运行状态
		if(RT_EOK == GetRunStatus(&RunStatus)){
			//原来是停止的状态 则开抽风
			if(0 == LocalRunStatus){
				if(RunStatus){
					LocalRunStatus = 1;
					DelayTimeRecorde = rt_tick_get();
					StartRunTime = DelayTimeRecorde;
					_OpenConverter();
				}
			}else if(1 == LocalRunStatus){
				//如果在启动10秒内关闭运行 则直接停止运行
				if(0 == RunStatus){
					LocalRunStatus = 0;
					_CloseConverter();
				}
				//抽风10秒后 再打开加热
				if(rt_tick_get() - DelayTimeRecorde >= 1000){
					LocalRunStatus = 2;
				}
			}else if(2 == LocalRunStatus){
				if(0 == RunStatus){
					DelayTimeRecorde = rt_tick_get();
					LocalRunStatus = 3;
					StableTempCnt = 0;
				}
			}else if(3 == LocalRunStatus){
				//停止工作以后 抽风机再工作30秒或者检测温度小于35度后再停止抽风
				if(_LastTemperature < 35){
					StableTempCnt++;
				}else{
					StableTempCnt = 0;
				}
				if((StableTempCnt >= 10) || (rt_tick_get() - DelayTimeRecorde >= 30000)){
					LocalRunStatus = 0;
					_CloseConverter();
				}
			}
			if((0 == DispSetModeGet()) && LocalRunStatus){
				LCD12864_ShowNumber(1, 5, (rt_tick_get() - StartRunTime)/60000);
				LCD12864_ShowNumber(1, 7, (rt_tick_get() - StartRunTime)/1000%60);
			}
			_TempPIDProcess(&TempPIDInfo, _LastTemperature);
			if(2 == LocalRunStatus){
				//如果当前温度小于DEF_STABLE_TEMPERATURE度 则同时开启3KW与6KW加热  否则只开启6KW加热
//				rt_kprintf("_LastTemperature:%d, StableTemperature:%d\n", _LastTemperature, GetStableTemperature());
				//当温度持续低于指定温度一定时间候再开启加热
				//当温度持续高于指定温度一定时间候再停止加热
				if(_LastTemperature < GetStableTemperature()){
					if(0 == FlagHotState){
						FlagHotState = 3;
						
					}else if((1 == FlagHotState) || (4 == FlagHotState) || (5 == FlagHotState)){
						HotStateTime = rt_tick_get();
						FlagHotState = 2;
					}else if(2 == FlagHotState){
						if(rt_tick_get() - HotStateTime >= 2000){
							FlagHotState = 3;
						}
					}
				}else{
					if((2 == FlagHotState) || (3 == FlagHotState) || (0 == FlagHotState)){
						FlagHotState = 4;
						HotStateTime = rt_tick_get();
					}else if(4 == FlagHotState){
						if(rt_tick_get() - HotStateTime >= 2000){
							HotStateTime = rt_tick_get();
							FlagHotState = 1;
						}
					}else if(1 == FlagHotState){
						if(rt_tick_get() - HotStateTime >= 5000){
							FlagHotState = 5;
						}
					}
				}
				if(3 == FlagHotState){
					_RefreshHotPower(9);
					FlagHot = 2;
				}else if(1 == FlagHotState){
					_RefreshHotPower(6);
					FlagHot = 1;
				}else if(5 == FlagHotState){
					_RefreshHotPower(3);
					FlagHot = 3;
				}
			}else{
				FlagPID = 0;
				FlagHot = 0;
				FlagHotState = 0;
				_TempPIDInit(&TempPIDInfo);
				_RefreshHotPower(0);
			}
		}else{
			FlagHot = 0;
			FlagHotState = 0;
			StartRunTime = rt_tick_get();
			_RefreshHotPower(0);
			_CloseConverter();
		}
		if(GetFixedHotPower() && RunStatus && (0 == DispSetModeGet())){
			switch(GetFixedHotPower()){
				case 1:
					//LCD12864_ShowString(1, 3, "3 KW");
					_RefreshHotPower(3);
				break;
				case 2:
					//LCD12864_ShowString(1, 3, "6 KW");
					_RefreshHotPower(6);
				break;
				case 3:
					//LCD12864_ShowString(1, 3, "9 KW");
					_RefreshHotPower(9);
				break;
				default:
					//LCD12864_ShowString(1, 3, "关闭");
					_RefreshHotPower(0);
				break;
			}
			FlagTempPower = 255;
		}/*else if((FlagHot != FlagTempPower) && (0 == DispSetModeGet())){
			FlagTempPower = FlagHot;
			if(0 == FlagHot){
				LCD12864_ShowString(1, 3, "关闭");
			}else if(1 == FlagHot){
				LCD12864_ShowString(1, 3, "6 KW");
			}else if(2 == FlagHot){
				LCD12864_ShowString(1, 3, "9 KW");
			}else if(3 == FlagHot){
				LCD12864_ShowString(1, 3, "3 KW");
			}
		}else if(DispSetModeGet()){
			FlagTempPower = 255;
			FlagTempPower = 5;
		}*/
		rt_thread_mdelay(1000);
	}
}

