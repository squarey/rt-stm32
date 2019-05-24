

#include "I2C.h"

#define DEF_I2C_PORT		GPIOA
#define DEF_I2C_CLK_PIN		GPIO_PIN_2
#define DEF_I2C_SDA_PIN		GPIO_PIN_3
#define ENABLE_GPIO_CLK()	__HAL_RCC_GPIOA_CLK_ENABLE()		

static rt_uint8_t _Inited = 0;

static void _I2C_GPIO_Init(void)
{
	if(0 == _Inited){
		GPIO_InitTypeDef GPIO_Initure = {0};
		
		ENABLE_GPIO_CLK();			    //开启GPIO时钟
		GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  	//推挽输出
		GPIO_Initure.Pull = GPIO_NOPULL;          	//上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //高速
		GPIO_Initure.Pin = DEF_I2C_CLK_PIN | DEF_I2C_SDA_PIN;
		HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
		HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_CLK_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_SET);
		
		I2C_Stop(); 
		_Inited = 1;
	}
}

static void _I2C_SDA_PIN_OUTPUT(void)
{
	GPIO_InitTypeDef GPIO_Initure = {0};

	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  	//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;          	//上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //高速
	GPIO_Initure.Pin = DEF_I2C_SDA_PIN;
	HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_SET);
}

static void _I2C_SDA_PIN_INPUT(void)
{
	GPIO_InitTypeDef GPIO_Initure = {0};

	GPIO_Initure.Mode = GPIO_MODE_INPUT;  		//输入
	GPIO_Initure.Pull = GPIO_NOPULL;          	//上拉
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //高速
	GPIO_Initure.Pin = DEF_I2C_SDA_PIN;
	HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
}

//static void _I2C_SCL_PIN_OUTPUT(void)
//{
//	GPIO_InitTypeDef GPIO_Initure = {0};

//	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  	//推挽输出
//	GPIO_Initure.Pull = GPIO_NOPULL;          	//上拉
//	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //高速
//	GPIO_Initure.Pin = DEF_I2C_CLK_PIN;
//	HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
//	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_SET);
//}

//static void _I2C_SCL_PIN_INPUT(void)
//{
//	GPIO_InitTypeDef GPIO_Initure = {0};

//	GPIO_Initure.Mode = GPIO_MODE_INPUT;  		//输入
//	GPIO_Initure.Pull = GPIO_NOPULL;          	//上拉
//	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //高速
//	GPIO_Initure.Pin = DEF_I2C_CLK_PIN;
//	HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
//}

//#define _I2C_Func.CLK_PIN_HIGH()		
//#define _I2C_Func.CLK_PIN_LOW()		HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_CLK_PIN, GPIO_PIN_RESET)
//#define _I2C_Func.SDA_PIN_HIGH()		HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_SET)
//#define _I2C_Func.SDA_PIN_LOW()		HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_RESET)
//#define _I2C_Func.SDA_PIN_READ()		HAL_GPIO_ReadPin(DEF_I2C_PORT, DEF_I2C_SDA_PIN)
//#define _I2C_Func.SDA_PIN_OUTPUT()	_I2C_SDA_PIN_OUTPUT()
//#define _I2C_Func.SDA_PIN_INPUT()		_I2C_SDA_PIN_INPUT()

#define I2C_DELAY_US(x)		rt_hw_us_delay(x)

#define DEF_DEALY_US		15

static void I2C_CLK_PIN_HIGH(void)
{
	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_CLK_PIN, GPIO_PIN_SET);
}
static void I2C_CLK_PIN_LOW(void)
{
	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_CLK_PIN, GPIO_PIN_RESET);
}
static void I2C_SDA_PIN_HIGH(void)
{
	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_SET);
}
static void I2C_SDA_PIN_LOW(void)
{
	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_RESET);
}
static void I2C_SDA_PIN_OUTPUT(void)
{
	_I2C_SDA_PIN_OUTPUT();
}
static void I2C_SDA_PIN_INPUT(void)
{
	_I2C_SDA_PIN_INPUT();
}
static rt_uint8_t I2C_SDA_PIN_DATA(void)
{
	return HAL_GPIO_ReadPin(DEF_I2C_PORT, DEF_I2C_SDA_PIN);
}
static I2C_GPIOFunc	_I2C_Func = 
{
	I2C_CLK_PIN_HIGH,
	I2C_CLK_PIN_LOW,
	I2C_SDA_PIN_HIGH,
	I2C_SDA_PIN_LOW,
	I2C_SDA_PIN_INPUT,
	I2C_SDA_PIN_OUTPUT,
	I2C_SDA_PIN_DATA
};
rt_uint8_t I2C_Start(void)
{
	_I2C_Func.SDA_PIN_HIGH(); 
	I2C_DELAY_US(DEF_DEALY_US);		//延时保证时钟频率低于40K，以便从机识别
	_I2C_Func.CLK_PIN_HIGH();
	//_I2C_Func.SDA_PIN_INPUT();
	I2C_DELAY_US(DEF_DEALY_US);		//延时保证时钟频率低于40K，以便从机识别
	if(!_I2C_Func.SDA_PIN_READ()){
		return 1;				//SDA线为低电平则总线忙,退出
	}
	//_I2C_Func.SDA_PIN_OUTPUT();
	_I2C_Func.SDA_PIN_LOW();   	//SCL处于高电平的时候，SDA拉低
	I2C_DELAY_US(DEF_DEALY_US);
	/*if(_I2C_Func.SDA_PIN_READ()){ 
		rt_kprintf("bbbbbbbbbbbbbbb\n");
		return 1;				//SDA线为高电平则总线出错,退出
	}*/
	_I2C_Func.CLK_PIN_LOW();
	I2C_DELAY_US(DEF_DEALY_US);
	return 0;
}

void I2C_Stop(void)
{
    _I2C_Func.SDA_PIN_LOW();
	_I2C_Func.CLK_PIN_LOW();
	I2C_DELAY_US(DEF_DEALY_US);
	_I2C_Func.CLK_PIN_HIGH();
	I2C_DELAY_US(DEF_DEALY_US);
	_I2C_Func.SDA_PIN_HIGH();	//当SCL处于高电平期间，SDA由低电平变成高电平             //延时
	
}

void I2C_SendACK(rt_uint8_t ACKData)
{
	if(ACKData == 1){
		_I2C_Func.SDA_PIN_HIGH();	             	//准备好SDA电平状态，不应答
	}else{ 
		_I2C_Func.SDA_PIN_LOW();  				//准备好SDA电平状态，应答 	
	}
	_I2C_Func.CLK_PIN_HIGH();                    	//拉高时钟线
    I2C_DELAY_US(DEF_DEALY_US);                 		//延时
    _I2C_Func.CLK_PIN_LOW() ;                  	//拉低时钟线
    I2C_DELAY_US(DEF_DEALY_US);  
}

rt_uint8_t I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	rt_uint16_t i=0;
	_I2C_Func.SDA_PIN_HIGH();	        //释放SDA
	_I2C_Func.SDA_PIN_INPUT();
	//I2C_DELAY_US(5);
	_I2C_Func.CLK_PIN_HIGH();         //SCL拉高进行采样
	I2C_DELAY_US(1);
	while(_I2C_Func.SDA_PIN_READ()){//等待SDA拉低
		i++;      //等待计数
		//I2C_DELAY_US(1);
		if(i == 500){//超时跳出循环
			break;
		}
	}
	if(_I2C_Func.SDA_PIN_READ()){//再次判断SDA是否拉低
		_I2C_Func.CLK_PIN_LOW(); 
		_I2C_Func.SDA_PIN_OUTPUT();
//		rt_kprintf("i：%d, No Ack\n", i);
		return 1;//从机应答失败，返回1
	}
	I2C_DELAY_US(DEF_DEALY_US);//延时保证时钟频率低于40K，
	_I2C_Func.CLK_PIN_LOW();
	I2C_DELAY_US(DEF_DEALY_US); //延时保证时钟频率低于40K，
	_I2C_Func.SDA_PIN_OUTPUT();
	return 0;//从机应答成功，返回0
}


void I2C_SendByte(rt_uint8_t Data)
{
	rt_uint8_t i = 0;
	_I2C_Func.CLK_PIN_LOW();						//SCL拉低，给SDA准备
	for (i = 0; i < 8; i++){         //8位计数器
		if(Data & 0x80){			//SDA准备
			_I2C_Func.SDA_PIN_HIGH();  
		}else{ 
			_I2C_Func.SDA_PIN_LOW();
		}
		_I2C_Func.CLK_PIN_HIGH();          //拉高时钟，给从机采样
		I2C_DELAY_US(DEF_DEALY_US);        	//延时保持IIC时钟频率，也是给从机采样有充足时间
		_I2C_Func.CLK_PIN_LOW();          //拉低时钟，给SDA准备
		I2C_DELAY_US(DEF_DEALY_US); 		  	//延时保持IIC时钟频率
		Data <<= 1;          	//移出数据的最高位  
	}					 
}

rt_uint8_t I2C_RecvByte(void)
{
	rt_uint8_t i;
	rt_uint8_t Data = 0;
	_I2C_Func.SDA_PIN_INPUT();
	_I2C_Func.SDA_PIN_HIGH();				//释放SDA，给从机使用
	I2C_DELAY_US(1);         		//延时给从机准备SDA时间
	for (i = 0; i < 8; i++){         //8位计数器
		Data <<= 1;
		_I2C_Func.CLK_PIN_HIGH();                //拉高时钟线，采样从机SDA
		if(_I2C_Func.SDA_PIN_READ()){ 		//读数据    
			Data |= 0x01;
		}			  
		I2C_DELAY_US(DEF_DEALY_US);     //延时保持IIC时钟频率		
		_I2C_Func.CLK_PIN_LOW();           	//拉低时钟线，处理接收到的数据
		I2C_DELAY_US(DEF_DEALY_US);   	//延时给从机准备SDA时间
	} 
	_I2C_Func.SDA_PIN_OUTPUT();
	return Data;
}

rt_uint8_t I2C_WriteBytes(rt_uint8_t Slave_Address, rt_uint8_t REG_Address, rt_uint8_t *pDataBuff, rt_uint8_t Length)
{
	rt_uint8_t WriteLen = 0;
	_I2C_GPIO_Init();
	if(I2C_Start()){  //起始信号
		I2C_Stop(); 
		return 1;
	}           
	I2C_SendByte(Slave_Address);   //发送设备地址+写信号
	if(I2C_WaitAck()){
		I2C_Stop(); 
		return 1;
	}
	I2C_SendByte(REG_Address);    //内部寄存器地址，
	if(I2C_WaitAck()){
		I2C_Stop(); 
		return 1;
	}
	while(WriteLen < Length){
		I2C_SendByte(pDataBuff[WriteLen]);       //数据
		WriteLen++;
		if(I2C_WaitAck()){
			I2C_Stop(); 
			return 1;
		}
	}
	I2C_Stop();   //发送停止信号

	return 0;
}

rt_uint8_t I2C_ReadBytes(rt_uint8_t Slave_Address, rt_uint8_t REG_Address, rt_uint8_t *pDataBuff, rt_uint8_t Length)
{
	rt_uint8_t ReadLen = 0;
	_I2C_GPIO_Init();
	if(I2C_Start()){  //起始信号
		I2C_Stop(); 
		return 1;
	}          
	I2C_SendByte(Slave_Address);    //发送设备地址+写信号
	if(I2C_WaitAck()){
		rt_kprintf("Send Addr no ack, Slave_Address:0x%02x\n", Slave_Address);
		I2C_Stop(); 
		return 1;
	} 
	I2C_SendByte(REG_Address);     //发送存储单元地址
	if(I2C_WaitAck()){
		I2C_Stop(); 
		return 1;
	} 
	if(I2C_Start()){  //起始信号
		I2C_Stop(); 
		return 1;
	}            
	I2C_SendByte(Slave_Address + 1);  //发送设备地址+读信号
	if(I2C_WaitAck()){
		I2C_Stop(); 
		return 1;
	}
	while(ReadLen < Length){
		pDataBuff[ReadLen] = I2C_RecvByte();
		ReadLen++;
		if(Length == ReadLen){
			I2C_SendACK(1);               		
		}else{
			I2C_SendACK(0);     			//应答	
		}
	}
	I2C_Stop();                    		//停止信号
	return 0;
}

void I2C_SetGPIOFunc(I2C_GPIOFunc *pFunc, rt_uint8_t Flag)
{
	if(1 == Flag){
		_I2C_Func.CLK_PIN_HIGH = pFunc->CLK_PIN_HIGH;
		_I2C_Func.CLK_PIN_LOW = pFunc->CLK_PIN_LOW;
		_I2C_Func.SDA_PIN_HIGH = pFunc->SDA_PIN_HIGH;
		_I2C_Func.SDA_PIN_LOW = pFunc->SDA_PIN_LOW;
		_I2C_Func.SDA_PIN_INPUT = pFunc->SDA_PIN_INPUT;
		_I2C_Func.SDA_PIN_OUTPUT = pFunc->SDA_PIN_OUTPUT;
		_I2C_Func.SDA_PIN_READ = pFunc->SDA_PIN_READ;
	}else{
		_I2C_Func.CLK_PIN_HIGH = I2C_CLK_PIN_HIGH;
		_I2C_Func.CLK_PIN_LOW = I2C_CLK_PIN_LOW;
		_I2C_Func.SDA_PIN_HIGH = I2C_SDA_PIN_HIGH;
		_I2C_Func.SDA_PIN_LOW = I2C_SDA_PIN_LOW;
		_I2C_Func.SDA_PIN_INPUT = I2C_SDA_PIN_INPUT;
		_I2C_Func.SDA_PIN_OUTPUT = I2C_SDA_PIN_OUTPUT;
		_I2C_Func.SDA_PIN_READ = I2C_SDA_PIN_DATA;
	}
	
}
