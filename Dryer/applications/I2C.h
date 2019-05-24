

#ifndef _IIC_H
#define _IIC_H

#include "rtthread.h"
#include "stm32f1xx_hal.h"
#include "rthw.h"

typedef enum{
  ACK                      = 0,
  NO_ACK                   = 1,
}etI2cAck;

typedef struct
{
	void (*CLK_PIN_HIGH)(void);
	void (*CLK_PIN_LOW)(void);
	void (*SDA_PIN_HIGH)(void);
	void (*SDA_PIN_LOW)(void);
	void (*SDA_PIN_INPUT)(void);
	void (*SDA_PIN_OUTPUT)(void);
	rt_uint8_t (*SDA_PIN_READ)(void);
}I2C_GPIOFunc;

rt_uint8_t I2C_Start(void);
void I2C_Stop(void);
void I2C_SendACK(rt_uint8_t ACKData);
rt_uint8_t I2C_WaitAck(void);
void I2C_SendByte(rt_uint8_t Data);
rt_uint8_t I2C_RecvByte(void);

	
rt_uint8_t I2C_WriteBytes(rt_uint8_t Slave_Address, rt_uint8_t REG_Address, rt_uint8_t *pDataBuff, rt_uint8_t Length);
rt_uint8_t I2C_ReadBytes(rt_uint8_t Slave_Address, rt_uint8_t REG_Address, rt_uint8_t *pDataBuff, rt_uint8_t Length);
void I2C_SetGPIOFunc(I2C_GPIOFunc *pFunc, rt_uint8_t Flag);


#endif 
