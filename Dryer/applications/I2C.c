

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
		
		ENABLE_GPIO_CLK();			    //����GPIOʱ��
		GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  	//�������
		GPIO_Initure.Pull = GPIO_NOPULL;          	//����
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //����
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

	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  	//�������
	GPIO_Initure.Pull = GPIO_NOPULL;          	//����
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //����
	GPIO_Initure.Pin = DEF_I2C_SDA_PIN;
	HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_SET);
}

static void _I2C_SDA_PIN_INPUT(void)
{
	GPIO_InitTypeDef GPIO_Initure = {0};

	GPIO_Initure.Mode = GPIO_MODE_INPUT;  		//����
	GPIO_Initure.Pull = GPIO_NOPULL;          	//����
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //����
	GPIO_Initure.Pin = DEF_I2C_SDA_PIN;
	HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
}

//static void _I2C_SCL_PIN_OUTPUT(void)
//{
//	GPIO_InitTypeDef GPIO_Initure = {0};

//	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  	//�������
//	GPIO_Initure.Pull = GPIO_NOPULL;          	//����
//	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //����
//	GPIO_Initure.Pin = DEF_I2C_CLK_PIN;
//	HAL_GPIO_Init(DEF_I2C_PORT, &GPIO_Initure);
//	HAL_GPIO_WritePin(DEF_I2C_PORT, DEF_I2C_SDA_PIN, GPIO_PIN_SET);
//}

//static void _I2C_SCL_PIN_INPUT(void)
//{
//	GPIO_InitTypeDef GPIO_Initure = {0};

//	GPIO_Initure.Mode = GPIO_MODE_INPUT;  		//����
//	GPIO_Initure.Pull = GPIO_NOPULL;          	//����
//	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;  //����
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
	I2C_DELAY_US(DEF_DEALY_US);		//��ʱ��֤ʱ��Ƶ�ʵ���40K���Ա�ӻ�ʶ��
	_I2C_Func.CLK_PIN_HIGH();
	//_I2C_Func.SDA_PIN_INPUT();
	I2C_DELAY_US(DEF_DEALY_US);		//��ʱ��֤ʱ��Ƶ�ʵ���40K���Ա�ӻ�ʶ��
	if(!_I2C_Func.SDA_PIN_READ()){
		return 1;				//SDA��Ϊ�͵�ƽ������æ,�˳�
	}
	//_I2C_Func.SDA_PIN_OUTPUT();
	_I2C_Func.SDA_PIN_LOW();   	//SCL���ڸߵ�ƽ��ʱ��SDA����
	I2C_DELAY_US(DEF_DEALY_US);
	/*if(_I2C_Func.SDA_PIN_READ()){ 
		rt_kprintf("bbbbbbbbbbbbbbb\n");
		return 1;				//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
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
	_I2C_Func.SDA_PIN_HIGH();	//��SCL���ڸߵ�ƽ�ڼ䣬SDA�ɵ͵�ƽ��ɸߵ�ƽ             //��ʱ
	
}

void I2C_SendACK(rt_uint8_t ACKData)
{
	if(ACKData == 1){
		_I2C_Func.SDA_PIN_HIGH();	             	//׼����SDA��ƽ״̬����Ӧ��
	}else{ 
		_I2C_Func.SDA_PIN_LOW();  				//׼����SDA��ƽ״̬��Ӧ�� 	
	}
	_I2C_Func.CLK_PIN_HIGH();                    	//����ʱ����
    I2C_DELAY_US(DEF_DEALY_US);                 		//��ʱ
    _I2C_Func.CLK_PIN_LOW() ;                  	//����ʱ����
    I2C_DELAY_US(DEF_DEALY_US);  
}

rt_uint8_t I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	rt_uint16_t i=0;
	_I2C_Func.SDA_PIN_HIGH();	        //�ͷ�SDA
	_I2C_Func.SDA_PIN_INPUT();
	//I2C_DELAY_US(5);
	_I2C_Func.CLK_PIN_HIGH();         //SCL���߽��в���
	I2C_DELAY_US(1);
	while(_I2C_Func.SDA_PIN_READ()){//�ȴ�SDA����
		i++;      //�ȴ�����
		//I2C_DELAY_US(1);
		if(i == 500){//��ʱ����ѭ��
			break;
		}
	}
	if(_I2C_Func.SDA_PIN_READ()){//�ٴ��ж�SDA�Ƿ�����
		_I2C_Func.CLK_PIN_LOW(); 
		_I2C_Func.SDA_PIN_OUTPUT();
//		rt_kprintf("i��%d, No Ack\n", i);
		return 1;//�ӻ�Ӧ��ʧ�ܣ�����1
	}
	I2C_DELAY_US(DEF_DEALY_US);//��ʱ��֤ʱ��Ƶ�ʵ���40K��
	_I2C_Func.CLK_PIN_LOW();
	I2C_DELAY_US(DEF_DEALY_US); //��ʱ��֤ʱ��Ƶ�ʵ���40K��
	_I2C_Func.SDA_PIN_OUTPUT();
	return 0;//�ӻ�Ӧ��ɹ�������0
}


void I2C_SendByte(rt_uint8_t Data)
{
	rt_uint8_t i = 0;
	_I2C_Func.CLK_PIN_LOW();						//SCL���ͣ���SDA׼��
	for (i = 0; i < 8; i++){         //8λ������
		if(Data & 0x80){			//SDA׼��
			_I2C_Func.SDA_PIN_HIGH();  
		}else{ 
			_I2C_Func.SDA_PIN_LOW();
		}
		_I2C_Func.CLK_PIN_HIGH();          //����ʱ�ӣ����ӻ�����
		I2C_DELAY_US(DEF_DEALY_US);        	//��ʱ����IICʱ��Ƶ�ʣ�Ҳ�Ǹ��ӻ������г���ʱ��
		_I2C_Func.CLK_PIN_LOW();          //����ʱ�ӣ���SDA׼��
		I2C_DELAY_US(DEF_DEALY_US); 		  	//��ʱ����IICʱ��Ƶ��
		Data <<= 1;          	//�Ƴ����ݵ����λ  
	}					 
}

rt_uint8_t I2C_RecvByte(void)
{
	rt_uint8_t i;
	rt_uint8_t Data = 0;
	_I2C_Func.SDA_PIN_INPUT();
	_I2C_Func.SDA_PIN_HIGH();				//�ͷ�SDA�����ӻ�ʹ��
	I2C_DELAY_US(1);         		//��ʱ���ӻ�׼��SDAʱ��
	for (i = 0; i < 8; i++){         //8λ������
		Data <<= 1;
		_I2C_Func.CLK_PIN_HIGH();                //����ʱ���ߣ������ӻ�SDA
		if(_I2C_Func.SDA_PIN_READ()){ 		//������    
			Data |= 0x01;
		}			  
		I2C_DELAY_US(DEF_DEALY_US);     //��ʱ����IICʱ��Ƶ��		
		_I2C_Func.CLK_PIN_LOW();           	//����ʱ���ߣ�������յ�������
		I2C_DELAY_US(DEF_DEALY_US);   	//��ʱ���ӻ�׼��SDAʱ��
	} 
	_I2C_Func.SDA_PIN_OUTPUT();
	return Data;
}

rt_uint8_t I2C_WriteBytes(rt_uint8_t Slave_Address, rt_uint8_t REG_Address, rt_uint8_t *pDataBuff, rt_uint8_t Length)
{
	rt_uint8_t WriteLen = 0;
	_I2C_GPIO_Init();
	if(I2C_Start()){  //��ʼ�ź�
		I2C_Stop(); 
		return 1;
	}           
	I2C_SendByte(Slave_Address);   //�����豸��ַ+д�ź�
	if(I2C_WaitAck()){
		I2C_Stop(); 
		return 1;
	}
	I2C_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
	if(I2C_WaitAck()){
		I2C_Stop(); 
		return 1;
	}
	while(WriteLen < Length){
		I2C_SendByte(pDataBuff[WriteLen]);       //����
		WriteLen++;
		if(I2C_WaitAck()){
			I2C_Stop(); 
			return 1;
		}
	}
	I2C_Stop();   //����ֹͣ�ź�

	return 0;
}

rt_uint8_t I2C_ReadBytes(rt_uint8_t Slave_Address, rt_uint8_t REG_Address, rt_uint8_t *pDataBuff, rt_uint8_t Length)
{
	rt_uint8_t ReadLen = 0;
	_I2C_GPIO_Init();
	if(I2C_Start()){  //��ʼ�ź�
		I2C_Stop(); 
		return 1;
	}          
	I2C_SendByte(Slave_Address);    //�����豸��ַ+д�ź�
	if(I2C_WaitAck()){
		rt_kprintf("Send Addr no ack, Slave_Address:0x%02x\n", Slave_Address);
		I2C_Stop(); 
		return 1;
	} 
	I2C_SendByte(REG_Address);     //���ʹ洢��Ԫ��ַ
	if(I2C_WaitAck()){
		I2C_Stop(); 
		return 1;
	} 
	if(I2C_Start()){  //��ʼ�ź�
		I2C_Stop(); 
		return 1;
	}            
	I2C_SendByte(Slave_Address + 1);  //�����豸��ַ+���ź�
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
			I2C_SendACK(0);     			//Ӧ��	
		}
	}
	I2C_Stop();                    		//ֹͣ�ź�
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
