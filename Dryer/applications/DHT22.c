

#include "DHT22.h"


#define PIN_DATA	GET_PIN(A, 2)

//IO��������
#define DHT11_IO_IN()  rt_pin_mode(PIN_DATA, PIN_MODE_INPUT)
#define DHT11_IO_OUT() rt_pin_mode(PIN_DATA, PIN_MODE_OUTPUT)
////IO��������											   
#define	DHT11_DQ_OUT(x) rt_pin_write(PIN_DATA, x) //���ݶ˿�	PA0 
#define	DHT11_DQ_IN()  	rt_pin_read(PIN_DATA)  //���ݶ˿�	PA0 


//��λDHT11
static void _DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 			//SET OUTPUT
    DHT11_DQ_OUT(0); 			//����DQ
    rt_hw_us_delay(600);    	//��������18ms
    DHT11_DQ_OUT(1); 			//DQ=1 
	rt_hw_us_delay(30);     	//��������20~40us
}
//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
static rt_uint8_t _DHT11_Check(void) 	   
{   
	rt_uint8_t  retry=0;
	DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN() && (retry < 100)){//DHT11������40~80us
		retry++;
		rt_hw_us_delay(1);
	};	 
	if(retry >= 100){
		return 1;
	}else{
		retry=0;
	}
    while (!DHT11_DQ_IN() && (retry < 100)){//DHT11���ͺ���ٴ�����40~80us
		retry++;
		rt_hw_us_delay(1);
	}
	if(retry >= 100){
		return 1;	
	}		
	return 0;
}
//��DHT11��ȡһ��λ
//����ֵ��1/0
static rt_uint8_t _DHT11_Read_Bit(void) 			 
{
 	rt_uint8_t  retry=0;
	while(DHT11_DQ_IN() && (retry < 100)){//�ȴ���Ϊ�͵�ƽ
		retry++;
		rt_hw_us_delay(1);
	}
	retry=0;
	while(!DHT11_DQ_IN() && (retry < 100)){//�ȴ���ߵ�ƽ
		retry++;
		rt_hw_us_delay(1);
	}
	rt_hw_us_delay(40);//�ȴ�40us
	if(DHT11_DQ_IN()){
		return 1;
	}else{
		return 0;	
	}	   
}
//��DHT11��ȡһ���ֽ�
//����ֵ������������
static rt_uint8_t _DHT11_Read_Byte(void)    
{        
    rt_uint8_t i = 0, dat = 0;
	for(i = 0; i < 8; i++){
   		dat <<= 1; 
	    dat |= _DHT11_Read_Bit();
    }						    
    return dat;
}
//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
rt_uint8_t  DHT11_Read_Data(rt_uint8_t  *temp, rt_uint8_t  *humi)    
{        
 	rt_uint16_t buf[5];
	rt_uint8_t  i;
	rt_uint8_t ret = 0;
	rt_base_t level;
	level = rt_hw_interrupt_disable();
	_DHT11_Rst();
	if(_DHT11_Check() == 0){
		//��ȡ40λ����
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



