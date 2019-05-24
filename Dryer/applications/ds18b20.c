
#include "ds18b20.h"
#include <rtdevice.h>
#include <board.h>
#include <rthw.h>
#include "drv_gpio.h"

#define USE_RT_PIN 0

#if USE_RT_PIN
#define DEF_DS18B20_PIN         GET_PIN(C, 6)
#else
#define DEF_DS18B20_PIN         GPIO_PIN_6
#define DEF_DS18B20_PORT		GPIOC
#endif 

#define DS18B20_DELAY_US(x)        rt_hw_us_delay(x)



#if USE_RT_PIN
#define _DS18B20_PIN_SetOutput()   rt_pin_mode(DEF_DS18B20_PIN, PIN_MODE_OUTPUT_OD)
#define _DS18B20_PIN_SetInput()    rt_pin_mode(DEF_DS18B20_PIN, PIN_MODE_INPUT)
#define _DS18B20_PIN_SetHigh()     rt_pin_write(DEF_DS18B20_PIN, 1)
#define _DS18B20_PIN_SetLow()      rt_pin_write(DEF_DS18B20_PIN, 0)
#define _DS18B20_PIN_ReadData()    rt_pin_read(DEF_DS18B20_PIN)
#else
static void _DS18B20_PIN_SetOutput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DEF_DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DEF_DS18B20_PORT, &GPIO_InitStruct);
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	
}
static void _DS18B20_PIN_SetInput(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DEF_DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DEF_DS18B20_PORT, &GPIO_InitStruct);
}
static void _DS18B20_PIN_SetHigh(void)
{
	HAL_GPIO_WritePin(DEF_DS18B20_PORT, DEF_DS18B20_PIN, GPIO_PIN_SET);
}
static void _DS18B20_PIN_SetLow(void)
{
	HAL_GPIO_WritePin(DEF_DS18B20_PORT, DEF_DS18B20_PIN, GPIO_PIN_RESET);
}
static rt_uint8_t _DS18B20_PIN_ReadData(void)
{
	return HAL_GPIO_ReadPin(DEF_DS18B20_PORT, DEF_DS18B20_PIN);
}
#endif 
//static rt_uint8_t _init_done = 0;

//复位DS18B20
static void _DS18B20_Rst(void)	   
{                 
	_DS18B20_PIN_SetOutput(); //SET PA0 OUTPUT
    _DS18B20_PIN_SetLow(); //拉低DQ
    DS18B20_DELAY_US(750);    //拉低750us
    _DS18B20_PIN_SetHigh(); //DQ=1 
	DS18B20_DELAY_US(15);     //15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
static rt_uint8_t _DS18B20_Check(void) 	   
{   
	rt_uint8_t retry = 0;
	_DS18B20_PIN_SetInput();//SET PA0 INPUT	 
    while (_DS18B20_PIN_ReadData() && (retry < 200)){
		retry++;
		DS18B20_DELAY_US(1);
	} 
	if(retry >= 200){
        return 1;
    }else{
        retry=0;
    } 
    while ((!_DS18B20_PIN_ReadData()) && (retry < 240)){
		retry++;
		DS18B20_DELAY_US(1);
	};
	if(retry >= 240){
        return 1;
    }	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
static rt_uint8_t _DS18B20_ReadBit(void) 			 // read one bit
{
    rt_uint8_t data;
	_DS18B20_PIN_SetOutput();//SET PA0 OUTPUT
    _DS18B20_PIN_SetLow(); 
	DS18B20_DELAY_US(2);
    _DS18B20_PIN_SetHigh(); 
	_DS18B20_PIN_SetInput();//SET PA0 INPUT
	DS18B20_DELAY_US(12);
	if(_DS18B20_PIN_ReadData()){
        data = 1;
    }else{
        data=0;	 
    } 
    DS18B20_DELAY_US(61);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
static rt_uint8_t _DS18B20_ReadByte(void)    // read one byte
{        
    rt_uint8_t i,j,dat;
    dat = 0;
	for(i = 1; i <= 8; i++) 
	{
        j = _DS18B20_ReadBit();
        dat = (j << 7) | (dat >> 1);
    }						    
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
static void _DS18B20_WriteByte(rt_uint8_t dat)     
 {             
    rt_uint8_t j;
    rt_uint8_t testb;
	_DS18B20_PIN_SetOutput();
    for(j = 1; j <= 8; j++){
        testb = dat & 0x01;
        dat = dat >> 1;
        if(testb){
            _DS18B20_PIN_SetLow();// Write 1
            DS18B20_DELAY_US(2);                            
            _DS18B20_PIN_SetHigh();
            DS18B20_DELAY_US(60);             
        }else{
            _DS18B20_PIN_SetLow();// Write 0
            DS18B20_DELAY_US(60);             
            _DS18B20_PIN_SetHigh();
            DS18B20_DELAY_US(2);                          
        }
    }
}
//开始温度转换
static void _DS18B20_Start(void)// ds1820 start convert
{   		
	rt_base_t Level = 0;
    _DS18B20_Rst();	   
	_DS18B20_Check();	 
	Level = rt_hw_interrupt_disable(); 
    _DS18B20_WriteByte(0xcc);// skip rom
    _DS18B20_WriteByte(0x44);// convert
	rt_hw_interrupt_enable(Level);
} 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
rt_uint8_t DS18B20_Init(void)
{
    _DS18B20_PIN_SetOutput();
 	_DS18B20_PIN_SetHigh();    //输出1

	_DS18B20_Rst();

	return _DS18B20_Check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
rt_int16_t DS18B20_GetTemperature(void)
{
    rt_uint8_t temp;
    rt_uint8_t TL,TH;
	rt_int16_t tem;
	rt_base_t Level = 0;
    _DS18B20_Start();                    // ds1820 start convert
    _DS18B20_Rst();
    if(_DS18B20_Check()){
        rt_kprintf("Error: Sensor DS18B20 is not exist\n");
        return 0xfff;
    }
	Level = rt_hw_interrupt_disable(); 
    _DS18B20_WriteByte(0xcc);// skip rom
    _DS18B20_WriteByte(0xbe);// convert	    
    TL = _DS18B20_ReadByte(); // LSB   
    TH = _DS18B20_ReadByte(); // MSB  
	rt_hw_interrupt_enable(Level);
	//rt_kprintf("TH:%d, TL:%d\n", TH, TL);    	  
    if(TH > 7){
        TH = ~TH;
        TL = ~TL; 
        temp = 0;//温度为负  
    }else{
        temp=1;//温度为正	  	 
    }
//    rt_kprintf("TH:%d, TL:%d\n", TH, TL);
    tem = TH; //获得高八位
    tem <<= 8;    
    tem += TL;//获得底八位
    tem = (float)tem * 0.625;//转换     
	if(temp){
        return tem; //返回温度值
    }else{
        return -tem;    
    } 
}
/*
//从ds18b20得到温度值
//精度：0.1C
rt_int16_t DS18B20_GetTemperature(void)
{
    rt_uint8_t temp = 0;
    rt_uint8_t TL = 0, TH = 0;
	rt_int16_t tem = 0;
	rt_base_t Level = 0;
	DS18B20_Init();
	Level = rt_hw_interrupt_disable();
	_DS18B20_WriteByte(0xcc);// skip rom
    _DS18B20_WriteByte(0x44);
	rt_hw_interrupt_enable(Level); 
	DS18B20_Init();
	Level = rt_hw_interrupt_disable();
    _DS18B20_WriteByte(0xcc);// skip rom
    _DS18B20_WriteByte(0xbe);// convert	    
    TL = _DS18B20_ReadByte(); // LSB   
    TH = _DS18B20_ReadByte(); // MSB  
	rt_hw_interrupt_enable(Level);
    if(TH > 7){
        TH = ~TH;
        TL = ~TL; 
        temp = 0;//温度为负  
    }else{
        temp = 1;//温度为正	  	 
    }
 //   rt_kprintf("222TH:%02x, TL:%02x\n", TH, TL);
    tem = (TH << 8) | TL;
   // tem <<= 8;    
    //tem += TL;//获得底八位
    tem = (float)tem * 0.625;//转换    
//	tem = (tem >> 8) + (tem >> 1);
	//返回温度值
	if(temp){
        return tem;
    }else{
        return -tem;
    } 
	
}
*/
