   
 #include "bmp280_support.h"
 /*
****************************************************************************
* Copyright (C) 2015 - 2016 Bosch Sensortec GmbH
*
* bmp280_support.c
* Date: 2016/07/01
* Revision: 1.0.6
*
* Usage: Sensor Driver support file for BMP280 sensor
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/
/*---------------------------------------------------------------------------*/
/* Includes*/
/*---------------------------------------------------------------------------*/
#include "bmp280.h"

#define BMP280_API
/*Enable the macro BMP280_API to use this support file */
/*----------------------------------------------------------------------------*
*  The following functions are used for reading and writing of
*	sensor data using I2C or SPI communication
*----------------------------------------------------------------------------*/
#ifdef BMP280_API
/*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, where data is going to be read
 *	\param reg_data : This is the data read from the sensor, which is held in an array
 *	\param cnt : The no of bytes of data to be read
 */
s8 BMP280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
 /*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, where data is to be written
 *	\param reg_data : It is a value held in the array,
 *		which is written in the register
 *	\param cnt : The no of bytes of data to be written
 */
s8 BMP280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The function is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, where data is to be written
 *	\param reg_data : It is a value held in the array,
 *		which is written in the register
 *	\param cnt : The no of bytes of data to be written
 */
s8 BMP280_SPI_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The function is used as SPI bus read
 *	\Return : Status of the SPI read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, where data is going to be read
 *	\param reg_data : This is the data read from the sensor, which is held in an array
 *	\param cnt : The no of bytes of data to be read */
s8 BMP280_SPI_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*
 * \Brief: SPI/I2C init routine
*/
s8 I2C_routine(void);
s8 SPI_routine(void);
#endif
/********************End of I2C/SPI function declarations***********************/
/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMP280_delay_msek(u32 msek);
/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
//s32 bmp280_data_readout_template(/*void*/ u32 *temp,u32 *press);
/*----------------------------------------------------------------------------*
 *  struct bmp280_t parameters can be accessed by using bmp280
 *	bmp280_t having the following parameters
 *	Bus write function pointer: BMP280_WR_FUNC_PTR
 *	Bus read function pointer: BMP280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *	Chip id of the sensor: chip_id
 *---------------------------------------------------------------------------*/
struct bmp280_t bmp280;
#if 0
/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
//s32 bmp280_data_readout_template(/*void*/ u32 *temp,u32 *press)
//{
//	/* The variable used to assign the standby time*/
//	u8 v_standby_time_u8 = BMP280_INIT_VALUE;

//	/* The variables used in individual data read APIs*/
//	/* The variable used to read uncompensated temperature*/
//	s32 v_data_uncomp_tem_s32 = BMP280_INIT_VALUE;
//	/* The variable used to read uncompensated pressure*/
//	s32 v_data_uncomp_pres_s32 = BMP280_INIT_VALUE;
//	/* The variable used to read real temperature*/
//	s32 v_actual_temp_s32 = BMP280_INIT_VALUE;
//	/* The variable used to read real pressure*/
//	u32 v_actual_press_u32 = BMP280_INIT_VALUE;

//	/* The variables used in combined data read APIs*/
//	/* The variable used to read uncompensated temperature*/
//	s32 v_data_uncomp_tem_combined_s32 = BMP280_INIT_VALUE;
//	/* The variable used to read uncompensated pressure*/
//	s32 v_data_uncomp_pres_combined_s32 = BMP280_INIT_VALUE;
//	/* The variable used to read real temperature*/
//	s32 v_actual_temp_combined_s32 = BMP280_INIT_VALUE;
//	/* The variable used to read real pressure*/
//	u32 v_actual_press_combined_u32 = BMP280_INIT_VALUE;

//	/* result of communication results*/
//	s32 com_rslt = ERROR;
//    //static u8 k=123;
//    
//   
//        
///*********************** START INITIALIZATION ************************/
//  /*	Based on the user need configure I2C or SPI interface.
//   *	It is example code to explain how to use the bmp280 API*/
//   #ifdef BMP280_API

//    #ifdef __I2C_ROUTINE__
//    I2C_routine();
//    #else
//    #ifdef __SPI_ROUTINE__
//    SPI_routine(); 
//    #else
//    #error "1540909218�此�。。。。。。。。。。。。。。。。"
//    #endif
//    #endif
//    #endif
///*--------------------------------------------------------------------------*
// *  This function used to assign the value/reference of
// *	the following parameters
// *	I2C address
// *	Bus Write
// *	Bus read
// *	Chip id
//*-------------------------------------------------------------------------*/
// 
//    //if(k==123){
//	//com_rslt = bmp280_init(&bmp280);k=0;}

//	/*	For initialization it is required to set the mode of
//	 *	the sensor as "NORMAL"
//	 *	data acquisition/read/write is possible in this mode
//	 *	by using the below API able to set the power mode as NORMAL*/
//	/* Set the power mode as NORMAL*/
//	com_rslt += bmp280_set_power_mode(BMP280_NORMAL_MODE);
//	/*	For reading the pressure and temperature data it is required to
//	 *	set the work mode
//	 *	The measurement period in the Normal mode is depends on the setting of
//	 *	over sampling setting of pressure, temperature and standby time
//	 *
//	 *	OSS				pressure OSS	temperature OSS
//	 *	ultra low power			x1			x1
//	 *	low power			x2			x1
//	 *	standard resolution		x4			x1
//	 *	high resolution			x8			x2
//	 *	ultra high resolution		x16			x2
//	 */
//	/* The oversampling settings are set by using the following API*/
//	com_rslt += bmp280_set_work_mode(BMP280_ULTRA_HIGH_RESOLUTION_MODE);
///*------------------------------------------------------------------------*
//************************* START GET and SET FUNCTIONS DATA ****************
//*---------------------------------------------------------------------------*/
//	/* This API used to Write the standby time of the sensor input
//	 *	value have to be given*/
//	 /*	Normal mode comprises an automated perpetual cycling between an (active)
//	 *	Measurement period and an (inactive) standby period.
//	 *	The standby time is determined by the contents of the register t_sb.
//	 *	Standby time can be set using BMP280_STANDBYTIME_125_MS.
//	 *	Usage Hint : BMP280_set_standbydur(BMP280_STANDBYTIME_125_MS)*/

//	com_rslt += bmp280_set_standby_durn(BMP280_STANDBY_TIME_1_MS);

//	/* This API used to read back the written value of standby time*/
//	com_rslt += bmp280_get_standby_durn(&v_standby_time_u8);
///*-----------------------------------------------------------------*
//************************* END GET and SET FUNCTIONS ****************
//*------------------------------------------------------------------*/
// 
///************************* END INITIALIZATION *************************/
//      //////////////////////////////////////////////////if
///*------------------------------------------------------------------*
//****** INDIVIDUAL APIs TO READ UNCOMPENSATED PRESSURE AND TEMPERATURE*******
//*---------------------------------------------------------------------*/
//	/* API is used to read the uncompensated temperature*/
//	com_rslt += bmp280_read_uncomp_temperature(&v_data_uncomp_tem_s32);

//	/* API is used to read the uncompensated pressure*/
//	com_rslt += bmp280_read_uncomp_pressure(&v_data_uncomp_pres_s32);

//	/* API is used to read the true temperature*/
//	/* Input value as uncompensated temperature*/
//	v_actual_temp_s32 = bmp280_compensate_temperature_int32(v_data_uncomp_tem_s32);

//	/* API is used to read the true pressure*/
//	/* Input value as uncompensated pressure*/
//	v_actual_press_u32 = bmp280_compensate_pressure_int32(v_data_uncomp_pres_s32);

//    
///*------------------------------------------------------------------*
//******* STAND-ALONE APIs TO READ COMBINED TRUE PRESSURE AND TEMPERATURE********
//*---------------------------------------------------------------------*/


//	/* API is used to read the uncompensated temperature and pressure*/
//	com_rslt += bmp280_read_uncomp_pressure_temperature(&v_data_uncomp_pres_combined_s32,
//	&v_data_uncomp_tem_combined_s32);

//	/* API is used to read the true temperature and pressure*/
//	com_rslt += bmp280_read_pressure_temperature(&v_actual_press_combined_u32,
//	&v_actual_temp_combined_s32);

//    *temp=v_actual_temp_combined_s32;
//    *press=v_actual_press_combined_u32;

///************************* START DE-INITIALIZATION ***********************/

//	/*	For de-initialization it is required to set the mode of
//	 *	the sensor as "SLEEP"
//	 *	the device reaches the lowest power consumption only
//	 *	In SLEEP mode no measurements are performed
//	 *	All registers are accessible
//	 *	by using the below API able to set the power mode as SLEEP*/
//	 /* Set the power mode as SLEEP*/
//	com_rslt += bmp280_set_power_mode(BMP280_SLEEP_MODE);
//   return com_rslt;
///************************* END DE-INITIALIZATION **********************/
//}
#endif//

#ifdef BMP280_API
/*--------------------------------------------------------------------------*
*	The following function is used to map the I2C bus read, write, delay and
*	device address with global structure bmp280_t
*-------------------------------------------------------------------------*/

static void _BMO280_mdelay(u32 x)
{
	rt_thread_mdelay(x);
}
s8 I2C_routine(void) {
/*--------------------------------------------------------------------------*
 *  By using bmp280 the following structure parameter can be accessed
 *	Bus write function pointer: BMP280_WR_FUNC_PTR
 *	Bus read function pointer: BMP280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *--------------------------------------------------------------------------*/
	bmp280.bus_write = I2C_WriteBytes;
	bmp280.bus_read = I2C_ReadBytes;
	bmp280.dev_addr = 0xee;
	bmp280.delay_msec = _BMO280_mdelay;

	return BMP280_INIT_VALUE;
}

/*---------------------------------------------------------------------------*
 * The following function is used to map the SPI bus read, write and delay
 * with global structure bmp280_t
 *--------------------------------------------------------------------------*/
#ifdef __SPI_ROUTINE__
s8 SPI_routine(void) {
/*--------------------------------------------------------------------------*
 *  By using bmp280 the following structure parameter can be accessed
 *	Bus write function pointer: BMP280_WR_FUNC_PTR
 *	Bus read function pointer: BMP280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *--------------------------------------------------------------------------*/

	bmp280.bus_write = BMP280_SPI_bus_write;
	bmp280.bus_read = BMP280_SPI_bus_read;
	bmp280.delay_msec = BMP280_delay_msek;

	return BMP280_INIT_VALUE;
}
#endif 
#endif
