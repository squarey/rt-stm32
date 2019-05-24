

#include "GY39.h"
#include "I2C.h"

#define GY39_I2C_ADDR	0xb6

rt_uint8_t GY39_GetData(GY39_Data *pData)
{
	if(RT_NULL == pData){
		return 1;
	}else{
		rt_uint8_t DataBuff[10];
		if(0 == I2C_ReadBytes(GY39_I2C_ADDR, 0x04, DataBuff, 10)){
				pData->Temperature = (DataBuff[0] << 8) | DataBuff[1];
				pData->Pressure = (((uint32_t)DataBuff[2]) << 24) | (DataBuff[3] << 16) | (DataBuff[4] << 8) | DataBuff[5];
				pData->Humidity = (DataBuff[6] << 8) | DataBuff[7];
				pData->Altitude = (DataBuff[8] << 8) | DataBuff[9];
		}else{
			rt_kprintf("GY-39 Read Data Fail\n");
			return 1;
		}
		return 0;
	}
}



