


#include "app_includes.h"
#include "fft.h"


static rt_uint8_t _IsEnable = 0;
static rt_mutex_t _ADC_Mutex = RT_NULL;


rt_int32_t APP_ADC_Init(void)
{
	if(RT_NULL == _ADC_Mutex){
		_ADC_Mutex = rt_mutex_create("adc1", RT_IPC_FLAG_FIFO);
	}
	if(RT_EOK != rt_mutex_take(_ADC_Mutex, RT_WAITING_FOREVER)){
		return -1;
	}else{
		rt_device_t _DevADC = RT_NULL;
		rt_int32_t ret = RT_EOK;
		_DevADC = rt_device_find("adc1");
		if(RT_NULL == _DevADC){
			rt_kprintf("Cannot find adc1 device\n");
			return RT_ERROR;
		}
		rt_device_open(_DevADC, RT_DEVICE_OFLAG_RDWR);
		if(0 == _IsEnable){
			_IsEnable = 1;
			rt_device_control(_DevADC, RT_ADC_CMD_ENABLE, RT_NULL);
		}
		rt_device_close(_DevADC);
		rt_mutex_release(_ADC_Mutex);
		return ret;
	}
	
}

rt_int32_t APP_ADC_GetConvertValue(rt_uint8_t Channel, rt_uint32_t *pBuffer, rt_uint32_t Len)
{
	if(RT_NULL == _ADC_Mutex){
		_ADC_Mutex = rt_mutex_create("adc1", RT_IPC_FLAG_FIFO);
	}
	if(RT_EOK != rt_mutex_take(_ADC_Mutex, RT_WAITING_FOREVER)){
		return -1;
	}else{
		rt_device_t _DevADC = RT_NULL;
		rt_int32_t ret = RT_EOK;
		_DevADC = rt_device_find("adc1");
		if(RT_NULL == _DevADC){
			rt_kprintf("Cannot find adc1 device\n");
			return RT_ERROR;
		}
		rt_device_open(_DevADC, RT_DEVICE_OFLAG_RDWR);
		if(0 == _IsEnable){
			_IsEnable = 1;
			rt_device_control(_DevADC, RT_ADC_CMD_ENABLE, RT_NULL);
		}
		ret = rt_device_read(_DevADC, Channel, pBuffer, Len);
		rt_device_close(_DevADC);
		rt_mutex_release(_ADC_Mutex);
		return ret;
	}
	
}
/*
typedef struct
{
    rt_uint32_t P;
    rt_uint16_t Temp;
    rt_uint16_t Hum;
    rt_uint16_t Alt;
} bme;

static bme Bme={0,0,0,0};

static void _GY39Test(void)
{
	rt_uint8_t  raw_data[13]={0};
	rt_uint16_t data_16[2]={0};
	char buff[10] = "";
	rt_uint32_t Lux = 0;
	if(0 == I2C_ReadByte(0xb6, 0x04,raw_data,10)){
		Bme.Temp=(raw_data[0]<<8)|raw_data[1];
		data_16[0]=(((uint16_t)raw_data[2])<<8)|raw_data[3];
		data_16[1]=(((uint16_t)raw_data[4])<<8)|raw_data[5];
		Bme.P=(((uint32_t)data_16[0])<<16)|data_16[1];
		Bme.Hum=(raw_data[6]<<8)|raw_data[7];
		Bme.Alt=(raw_data[8]<<8)|raw_data[9];
	}else{
		rt_kprintf("__Read GY39 Data Fail\n");
		return;
	}
	if(0 == I2C_ReadByte(0xb6,0x00,raw_data,4)){
		data_16[0]=(((uint16_t)raw_data[0])<<8)|raw_data[1];
		data_16[1]=(((uint16_t)raw_data[2])<<8)|raw_data[3];
		Lux=(((uint32_t)data_16[0])<<16)|data_16[1];
	}else{
		rt_kprintf("==Read GY39 Data Fail\n");
		return;
	}
	sprintf(buff, "%f", (float)Bme.Temp/100);
	rt_kprintf("温度: %s摄氏度\r\n",buff);
	sprintf(buff, "%f", (float)Bme.P/100);
	rt_kprintf("压强: %sfPa\r\n",buff);
	sprintf(buff, "%f", (float)Bme.Hum/100);
	rt_kprintf("湿度: %sf\r\n",buff);
	sprintf(buff, "%f", (float)Bme.Alt);
	rt_kprintf("海拔: %sm\r\n",buff);
	sprintf(buff, "%f", (float)Lux/100);
	rt_kprintf("光照度: %sflux\r\n\r\n",buff);  
}

void APP_ADC_Test(void)
{
	//_GY39Test();
}


static void _KeyProcess(rt_uint8_t Index, rt_uint8_t PressStatus)
{
	if((0 == Index) && (0 != PressStatus)){
		APP_ADC_Test();
	}
}

void ADC_TestInit(void)
{
	APP_KeySetPressCallback(_KeyProcess);
}
*/

