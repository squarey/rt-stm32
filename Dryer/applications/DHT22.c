

#include "DHT22.h"


#define PIN_DATA	GET_PIN(A, 2)

//IO方向设置
#define DHT11_IO_IN()  rt_pin_mode(PIN_DATA, PIN_MODE_INPUT)
#define DHT11_IO_OUT() rt_pin_mode(PIN_DATA, PIN_MODE_OUTPUT)
////IO操作函数											   
#define	DHT11_DQ_OUT(x) rt_pin_write(PIN_DATA, x) //数据端口	PA0 
#define	DHT11_DQ_IN()  	rt_pin_read(PIN_DATA)  //数据端口	PA0 


//复位DHT11
static void _DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 			//SET OUTPUT
    DHT11_DQ_OUT(0); 			//拉低DQ
    rt_hw_us_delay(600);    	//拉低至少18ms
    DHT11_DQ_OUT(1); 			//DQ=1 
	rt_hw_us_delay(30);     	//主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
static rt_uint8_t _DHT11_Check(void) 	   
{   
	rt_uint8_t  retry=0;
	DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN() && (retry < 100)){//DHT11会拉低40~80us
		retry++;
		rt_hw_us_delay(1);
	};	 
	if(retry >= 100){
		return 1;
	}else{
		retry=0;
	}
    while (!DHT11_DQ_IN() && (retry < 100)){//DHT11拉低后会再次拉高40~80us
		retry++;
		rt_hw_us_delay(1);
	}
	if(retry >= 100){
		return 1;	
	}		
	return 0;
}
//从DHT11读取一个位
//返回值：1/0
static rt_uint8_t _DHT11_Read_Bit(void) 			 
{
 	rt_uint8_t  retry=0;
	while(DHT11_DQ_IN() && (retry < 100)){//等待变为低电平
		retry++;
		rt_hw_us_delay(1);
	}
	retry=0;
	while(!DHT11_DQ_IN() && (retry < 100)){//等待变高电平
		retry++;
		rt_hw_us_delay(1);
	}
	rt_hw_us_delay(40);//等待40us
	if(DHT11_DQ_IN()){
		return 1;
	}else{
		return 0;	
	}	   
}
//从DHT11读取一个字节
//返回值：读到的数据
static rt_uint8_t _DHT11_Read_Byte(void)    
{        
    rt_uint8_t i = 0, dat = 0;
	for(i = 0; i < 8; i++){
   		dat <<= 1; 
	    dat |= _DHT11_Read_Bit();
    }						    
    return dat;
}
//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
rt_uint8_t  DHT11_Read_Data(rt_uint8_t  *temp, rt_uint8_t  *humi)    
{        
 	rt_uint16_t buf[5];
	rt_uint8_t  i;
	rt_uint8_t ret = 0;
	rt_base_t level;
	level = rt_hw_interrupt_disable();
	_DHT11_Rst();
	if(_DHT11_Check() == 0){
		//读取40位数据
		for(i = 0;i < 5; i++){
			buf[i] = _DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi = (buf[0] * 256 + buf[1])/10;
			*temp = (buf[2] * 256 + buf[3])/10;
		}else{
			ret = 1;
		}
	}else{
		ret = 1;
		rt_kprintf("DHT11 Check fail\n");
	}
	rt_hw_interrupt_enable(level);
	
	return ret;	    
}



