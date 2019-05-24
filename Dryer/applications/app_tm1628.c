
#include "app_tm1628.h"
#include <rtdevice.h>
#include <board.h>
#include <rthw.h>
#include "drv_gpio.h"


#define PIN_DIO		GET_PIN(C, 13)
#define PIN_CLK		GET_PIN(C, 9)
#define PIN_STB		GET_PIN(C, 8)


#define TM1628_DELAY_US(x)     rt_hw_us_delay(x)

static rt_uint8_t _Inited = 0;
static rt_mutex_t _TM1628_Lock = RT_NULL;

//'H'       0x76
//'L'       0x38
//TM1628驱动共阳极数码管0-9数字表
static const rt_uint8_t TM1629_NUMBER_TABLE[][7] = {
	//0x00 0x02  0x04  0x06  0x08  0x0a  0x0c
	{0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00},		//0
    {0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00},		//1
    {0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01},		//2
	{0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01},		//3
	{0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01},		//4
	{0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01},		//5
	{0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01},		//6
	{0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00},		//7
	{0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},		//8
	{0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01},		//9
	{0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01},		//E
};



static void _TM1628_STB_PIN_SetLow(void)
{
	rt_pin_write(PIN_STB, PIN_LOW);
}

static void _TM1628_STB_PIN_SetHigh(void)
{
	rt_pin_write(PIN_STB, PIN_HIGH);
}

static void _TM1628_CLK_PIN_SetLow(void)
{
	rt_pin_write(PIN_CLK, PIN_LOW);
}

static void _TM1628_CLK_PIN_SetHigh(void)
{
	rt_pin_write(PIN_CLK, PIN_HIGH);
}

static void _TM1628_DIO_PIN_SetLow(void)
{
	rt_pin_write(PIN_DIO, PIN_LOW);
}

static void _TM1628_DIO_PIN_SetHigh(void)
{
	rt_pin_write(PIN_DIO, PIN_HIGH);
}

static void _TM1628_DIO_PIN_Input(void)
{
	rt_pin_mode(PIN_DIO, PIN_MODE_INPUT_PULLUP);
}
static void _TM1628_DIO_PIN_Output(void)
{
	rt_pin_mode(PIN_DIO, PIN_MODE_OUTPUT);
}
static rt_uint8_t _TM1628_Read_DIO_PIN(void)
{
	return rt_pin_read(PIN_DIO);
}

static void _TM1628_SendByte(rt_uint8_t Data)
{
    rt_uint8_t i = 0;
    rt_uint8_t tdata = Data;
    //_TM1628_STB_PIN_SetLow();
//	_TM1628_DIO_PIN_SetHigh();
//	_TM1628_CLK_PIN_SetHigh();
    TM1628_DELAY_US(10);
    for (i = 0; i < 8; i++) {
        _TM1628_CLK_PIN_SetLow();
        //TM1628_DELAY_US(100);
        if(tdata & 0x01){
            _TM1628_DIO_PIN_SetHigh();
        }else{
            _TM1628_DIO_PIN_SetLow();
        }
        tdata >>= 1;
        TM1628_DELAY_US(10);
        _TM1628_CLK_PIN_SetHigh();	
        //TM1628_DELAY_US(10);
    }
    _TM1628_CLK_PIN_SetLow();
    _TM1628_DIO_PIN_SetLow();
    TM1628_DELAY_US(10);
    //rt_kprintf("TM1628 send Data %x\n", Data);
}

static rt_uint8_t _TM1628_ReadByte(void)
{
	rt_uint8_t i = 0;
	rt_uint8_t Data = 0;
	_TM1628_DIO_PIN_Input();
	TM1628_DELAY_US(20);
	for(i = 0; i < 8; i++){
		Data = Data >> 1;
		_TM1628_CLK_PIN_SetLow();
		_TM1628_CLK_PIN_SetHigh();
		if(_TM1628_Read_DIO_PIN()){
			Data |= 0x80;
		}
	}
	_TM1628_DIO_PIN_Output();
	return Data;
}

static void _TM1628_SendCommand(rt_uint8_t Cmd)
{
	_TM1628_STB_PIN_SetLow();
	_TM1628_SendByte(Cmd);
	_TM1628_STB_PIN_SetHigh();
//	rt_thread_mdelay(1);
}
void TM1628_Init(void)
{
	if(0 == _Inited){
		rt_pin_mode(PIN_DIO, PIN_MODE_OUTPUT);
		rt_pin_mode(PIN_CLK, PIN_MODE_OUTPUT);
		rt_pin_mode(PIN_STB, PIN_MODE_OUTPUT);
		
		_TM1628_STB_PIN_SetHigh();
		_TM1628_CLK_PIN_SetLow();
		_TM1628_DIO_PIN_SetLow();
		
		_TM1628_SendCommand(0x03);
		
		_TM1628_SendCommand(0x8f);
		
		if(RT_NULL == _TM1628_Lock){
			_TM1628_Lock =  rt_mutex_create("TM628", RT_IPC_FLAG_FIFO);
			if(RT_NULL == _TM1628_Lock){
				rt_kprintf("Create TIM1628 mutex fail\n");
			}
		}
		_Inited = 1;
		rt_thread_mdelay(1);
	}
    //_TM1628_SendByte(0x44);
    //_TM1628_STB_PIN_SetHigh();
    //TM1628_DELAY_US(10);
}

//设置数码管显示的数据
//Data: 需要显示的数据
//addr: 需要显示的数据的位置
void TM1628_DisplayData(rt_uint8_t Data, rt_uint8_t addr)
{
//	rt_thread_mdelay(1);
	/*
    TM1628_DELAY_US(10);
    _TM1628_SendByte(0x8b);  
    _TM1628_STB_PIN_SetHigh();
	*/
}
/*
自动显示0-99的数字
*/
void TM1628_AutoDisplayNumber(rt_uint8_t Data)
{
	rt_uint8_t i = 0;
	rt_uint8_t High = 0, Low = 0;
	TM1628_Init();
	if(RT_EOK != rt_mutex_take(_TM1628_Lock, RT_WAITING_FOREVER)){
		rt_kprintf("Take tm1628 mutex fail\n");
		return;
	}
    _TM1628_SendCommand(0x40);
//    _TM1628_STB_PIN_SetLow();
	TM1628_DELAY_US(10);
    if(Data < 10){
        for(i = 0; i < 7; i++){
			_TM1628_STB_PIN_SetLow();
			_TM1628_SendByte(0xC0 + i * 2);
			_TM1628_SendByte(TM1629_NUMBER_TABLE[Data][i] << 1);
			_TM1628_STB_PIN_SetHigh();
		}
		
    }else{
		for(i = 0; i < 7; i++){
			High = TM1629_NUMBER_TABLE[Data/10][i];
			Low = TM1629_NUMBER_TABLE[Data%10][i];
			_TM1628_STB_PIN_SetLow();
			_TM1628_SendByte(0xC0 + i * 2);
			_TM1628_SendByte((Low << 1) | High);
			_TM1628_STB_PIN_SetHigh();
		}
    }
	
	/*for(i = 0; i < 7; i++){
		_TM1628_STB_PIN_SetLow();
		_TM1628_SendByte(0xC0 + i * 2);
		_TM1628_SendByte(TM1628_NUMBER_TABLE2[i]);
		_TM1628_STB_PIN_SetHigh();
	}*/
	rt_mutex_release(_TM1628_Lock);
}

void TM1628_DisplayError(rt_uint8_t Error)
{
	rt_uint8_t i = 0;
	rt_uint8_t High = 0, Low = 0;
	TM1628_Init();
	if(RT_EOK != rt_mutex_take(_TM1628_Lock, RT_WAITING_FOREVER)){
		rt_kprintf("Take tm1628 mutex fail\n");
		return;
	}
    _TM1628_SendCommand(0x40);
    _TM1628_STB_PIN_SetLow();
	TM1628_DELAY_US(10);
	for(i = 0; i < 7; i++){
		High = TM1629_NUMBER_TABLE[10][i];
		Low = TM1629_NUMBER_TABLE[Error][i];
		_TM1628_SendByte(0xC0 + i * 2);
		_TM1628_SendByte((Low << 1) | High);
	}
	_TM1628_STB_PIN_SetHigh();
	rt_mutex_release(_TM1628_Lock);
}

void TM1628_ReadKey(rt_uint8_t Num, rt_uint8_t *pKeyValue)
{
	TM1628_Init();
	if(RT_EOK != rt_mutex_take(_TM1628_Lock, RT_WAITING_FOREVER)){
		rt_kprintf("Take tm1628 mutex fail\n");
		return;
	}
	_TM1628_STB_PIN_SetLow();
	_TM1628_SendByte(0x42);
	pKeyValue[0] = _TM1628_ReadByte();
	_TM1628_STB_PIN_SetHigh();
	rt_mutex_release(_TM1628_Lock);
}

void TM1628_DisplayTest(void)
{
    rt_uint8_t i = 0;
    TM1628_DisplayData(0, 0);
    for(i = 0; i < 20; i++){
       TM1628_AutoDisplayNumber(i);
        rt_thread_delay(500);
    }
}

#if 0
/********************************************************************************************
********************************************************************************************/
#include "drv_gpio.h"
#define TM1628_PIN_DIO2		GET_PIN(C, 13)
#define TM1628_PIN_CLK2		GET_PIN(B, 9)
#define TM1628_PIN_STB2		GET_PIN(B, 8)

static rt_mutex_t _TM1628_Lock2 = RT_NULL;

static const rt_uint8_t TM1628_NUMBER_TABLE2[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
    /*0xdd, 0xe9, 0x6c, 0xf1, 0x6d,
    0x4d, 0x68, 0xf8, 0xd9, 0x01, 0x00*/
};

static const rt_uint8_t DISP_ADDR[6][2] = {
	{0xc0, 0xc2},
	{0xc4, 0xc6},
	{0xc8, 0xca}
};


static rt_uint8_t _Inited2 = 0;
static void _TM1628_SendByte2(rt_uint8_t Data)
{
    rt_uint8_t i = 0;
    rt_uint8_t tdata = Data;
    //_TM1628_STB_PIN_SetLow();
	rt_pin_write(TM1628_PIN_DIO2, PIN_HIGH);
	rt_pin_write(TM1628_PIN_CLK2, PIN_HIGH);
    TM1628_DELAY_US(10);
    for (i = 0; i < 8; i++) {
        rt_pin_write(TM1628_PIN_CLK2, PIN_LOW);;
        TM1628_DELAY_US(10);
        if(tdata & 0x01){
            rt_pin_write(TM1628_PIN_DIO2, PIN_HIGH);
        }else{
            rt_pin_write(TM1628_PIN_DIO2, PIN_LOW);
        }
        tdata >>= 1;
        TM1628_DELAY_US(10);
        rt_pin_write(TM1628_PIN_CLK2, PIN_HIGH);
        TM1628_DELAY_US(10);
    }
    rt_pin_write(TM1628_PIN_DIO2, PIN_HIGH);
	rt_pin_write(TM1628_PIN_CLK2, PIN_HIGH);
    TM1628_DELAY_US(10);
    //rt_kprintf("TM1628 send Data %x\n", Data);
}

static void _TM1628_SendCommand2(rt_uint8_t Cmd)
{
	rt_pin_write(TM1628_PIN_STB2, PIN_LOW);
	TM1628_DELAY_US(10);
	_TM1628_SendByte2(Cmd);
	rt_pin_write(TM1628_PIN_STB2, PIN_HIGH);
//	rt_thread_mdelay(1);
}

static void _TM1628_Init2(void)
{
	if(0 == _Inited2){
		rt_pin_mode(TM1628_PIN_DIO2, PIN_MODE_OUTPUT);
		rt_pin_mode(TM1628_PIN_CLK2, PIN_MODE_OUTPUT);
		rt_pin_mode(TM1628_PIN_STB2, PIN_MODE_OUTPUT);
		
		rt_pin_write(TM1628_PIN_DIO2, PIN_HIGH);
		rt_pin_write(TM1628_PIN_CLK2, PIN_HIGH);
		rt_pin_write(TM1628_PIN_STB2, PIN_HIGH);
		
		_TM1628_SendCommand2(0x03);
			
		_TM1628_SendCommand2(0x8f);
		
		if(RT_NULL == _TM1628_Lock2){
			_TM1628_Lock2 =  rt_mutex_create("TM628_2", RT_IPC_FLAG_FIFO);
			if(RT_NULL == _TM1628_Lock2){
				rt_kprintf("Create TIM1628 mutex 2 fail\n");
			}
		}
		
		_Inited2 = 1;
	}
	
}


void TM1628_AutoDisplayNumber2(rt_uint8_t Index, rt_uint8_t Data)
{
	//rt_uint8_t i = 0;
	_TM1628_Init2();
	if(RT_EOK != rt_mutex_take(_TM1628_Lock2, RT_WAITING_FOREVER)){
		rt_kprintf("Take tm1628 mutex fail\n");
		return;
	}
    _TM1628_SendCommand2(0x40);
    rt_pin_write(TM1628_PIN_STB2, PIN_LOW);
	TM1628_DELAY_US(10);
	if(Data < 10){
		_TM1628_SendByte2(DISP_ADDR[Index - 1][0]);
		_TM1628_SendByte2(0);
		_TM1628_SendByte2(DISP_ADDR[Index - 1][1]);
		_TM1628_SendByte2(TM1628_NUMBER_TABLE2[Data]);
	}else{
		_TM1628_SendByte2(DISP_ADDR[Index - 1][0]);
		_TM1628_SendByte2(TM1628_NUMBER_TABLE2[Data/10]);
		_TM1628_SendByte2(DISP_ADDR[Index - 1][1]);
		_TM1628_SendByte2(TM1628_NUMBER_TABLE2[Data%10]);
	}
	rt_pin_write(TM1628_PIN_STB2, PIN_HIGH);
	rt_mutex_release(_TM1628_Lock2);
}

#endif 


