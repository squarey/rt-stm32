

#include "lcd12864.h"


#define DEF_CS_PIN			GET_PIN(B, 8)		//RS
#define DEF_SID_PIN			GET_PIN(B, 9)		//R/W
#define DEF_CLK_PIN			GET_PIN(C, 13)		//E

#define CS_LOW()			rt_pin_write(DEF_CS_PIN, PIN_LOW)
#define CS_HIGH()			rt_pin_write(DEF_CS_PIN, PIN_HIGH)
#define SDI_LOW()			rt_pin_write(DEF_SID_PIN, PIN_LOW)
#define SDI_HIGH()			rt_pin_write(DEF_SID_PIN, PIN_HIGH)
#define CLK_LOW()			rt_pin_write(DEF_CLK_PIN, PIN_LOW)
#define CLK_HIGH()			rt_pin_write(DEF_CLK_PIN, PIN_HIGH)


#define LCD12864_DELAY_US(x) rt_hw_us_delay(x)
#define EXTEND_CMD			1
#define BASE_CMD			0

static rt_mutex_t _lock_mutex = RT_NULL;
static rt_uint8_t _DrawMode = BASE_CMD;

/*
12864дһ���ֽ�����
*/
static void _LCD12864_WriteByte(rt_uint8_t Data)
{
	rt_uint8_t i = 0;
	rt_uint8_t tData = Data;
	for(i = 0; i < 8; i++){
		CLK_LOW();
		if((tData << i) & 0x80){
			SDI_HIGH();
		}else{
			SDI_LOW();
		}
		//rt_hw_us_delay(40);
		CLK_HIGH();
		//rt_hw_us_delay(10);
	}
}
/*
12864д����
*/
static void _LCD12864_WriteCom(rt_uint8_t Cmd)
{
	CS_HIGH();
	//rt_hw_us_delay(50);
	_LCD12864_WriteByte(0xf8);
	//rt_hw_us_delay(50);
	_LCD12864_WriteByte(Cmd & 0xf0);
	_LCD12864_WriteByte((Cmd << 4) & 0xf0);
	CS_LOW();
	//rt_hw_us_delay(10);
}
/*
��12864����ʾһ���ַ�
*/
static void _LCD12864_WriteData(rt_uint8_t Data)
{
	CS_HIGH();
	//rt_hw_us_delay(50);
	_LCD12864_WriteByte(0xfa);
	//rt_hw_us_delay(50);
	_LCD12864_WriteByte(Data & 0xf0);
	_LCD12864_WriteByte((Data << 4) & 0xf0);
	CS_LOW();
	//rt_hw_us_delay(10);
}

static void _LCD12864_MutexInit(void)
{
	_lock_mutex = rt_mutex_create("lcd12864", RT_IPC_FLAG_FIFO);
}

static void _LCD12864_Lock(void)
{
	rt_mutex_take(_lock_mutex, RT_WAITING_FOREVER);
}

static void _LCD12864_Unlock(void)
{
	rt_mutex_release(_lock_mutex);
}
/*
����ָ�����չָ��л���ʱ����Ҫ����
*/
static rt_uint8_t _LCD12864_SwitchMode(rt_uint8_t Mode)
{
	if(_DrawMode != Mode){
		//�л�������ָ�
		_LCD12864_WriteCom(CMD_BASE_COMMAND);
		_DrawMode = Mode;
		_LCD12864_WriteCom(0x01);
		rt_thread_mdelay(3);
		rt_kprintf("_DrawMode:%d\n", _DrawMode);
		return 1;
	}
	return 0;
}

void LCD12864_Init(void)
{
	rt_pin_mode(DEF_CS_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(DEF_SID_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(DEF_CLK_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(DEF_CS_PIN, PIN_HIGH);
	rt_pin_write(DEF_SID_PIN, PIN_HIGH);
	rt_pin_write(DEF_CLK_PIN, PIN_HIGH);
	_LCD12864_MutexInit();
	rt_thread_mdelay(10);
	//���óɻ���ָ�
	_LCD12864_WriteCom(CMD_BASE_COMMAND);
	//rt_thread_mdelay(1);
	//����ʾ�����ع��
	_LCD12864_WriteCom(CMD_DIS_ON_CURSOR_OFF);
	//����
	_LCD12864_WriteCom(0x01);
	//_LCD12864_WriteCom(0x03);
	rt_thread_mdelay(20);
	rt_kprintf("LCD12864_Init\n");
}
void LCD12864_Clear(void)
{
	//����
	_LCD12864_Lock();
	if(0 == _LCD12864_SwitchMode(BASE_CMD)){
		_LCD12864_WriteCom(0x01);
	}
	_LCD12864_Unlock();
}
//��ʾ�ַ���
//Line: �ڵڼ�����ʾ  0--3
//Pos:  �ڵ�Line�е�Posλ�ÿ�ʼ��ʾ
//pStr: ��ʾ���ַ���
void LCD12864_ShowString(rt_uint8_t Line, rt_uint8_t Pos, const char *pStr)
{
	_LCD12864_Lock();
	{
		const char *tStr = pStr;
		rt_size_t StrLen = 0;
		rt_uint8_t i = 0;
		if(Pos >= 16){
			return;
		}
		//rt_kprintf("Line:%d, Pos:%d, Str:%s...\n", Line, Pos, pStr);
		_LCD12864_SwitchMode(BASE_CMD);
		switch(Line){
			case 0:
				_LCD12864_WriteCom(0x80 + Pos);
			break;
			case 1:
				_LCD12864_WriteCom(0x90 + Pos);
			break;
			case 2:
				_LCD12864_WriteCom(0x88 + Pos);
			break;
			case 3:
				_LCD12864_WriteCom(0x98 + Pos);
			break;
			default:
			return;
		}
		while('\0' != *(tStr + i)){
			_LCD12864_WriteData(*(tStr + i));
			i++;
			//rt_thread_mdelay(1);
		}
		StrLen = rt_strlen(pStr);
		//�����ʾ���ַ���Ϊż�� ������ʾ��λ��������һλ
		if(0 == (StrLen%2)){
			_LCD12864_WriteCom(CMD_CURSOR_MOVE_LEFT);
		}
	}
	_LCD12864_Unlock();
}
//��12864����ʾ����
void LCD12864_ShowNumber(rt_uint8_t Line, rt_uint8_t Pos, rt_int32_t Number)
{
	_LCD12864_Lock();
	{
		char NumString[10];
		rt_uint8_t tPos = Pos;
		if(Number < 10){
			rt_sprintf(NumString, " %d", Number);
		}else{
			rt_sprintf(NumString, "%d", Number);
		}
		_LCD12864_SwitchMode(BASE_CMD);
		LCD12864_ShowString(Line, tPos, NumString);
	}
	_LCD12864_Unlock();
}

//��ʾ���
void LCD12864_ShowCursor(void)
{
	_LCD12864_Lock();
	_LCD12864_SwitchMode(BASE_CMD);
	_LCD12864_WriteCom(CMD_DIS_ON_CURSOR_ON);
	_LCD12864_Unlock();
}
//�رչ��
void LCD12864_HideCursor(void)
{
	_LCD12864_Lock();
	_LCD12864_SwitchMode(BASE_CMD);
	_LCD12864_WriteCom(CMD_DIS_ON_CURSOR_OFF);
	_LCD12864_Unlock();
}
//���ù��λ�ò���ʾ���
void LCD12864_SetCursor(rt_uint8_t Line, rt_uint8_t Pos)
{
	_LCD12864_Lock();
	{
		if(Pos >= 8){
			return;
		}
		_LCD12864_SwitchMode(BASE_CMD);
		switch(Line){
			case 0:
				_LCD12864_WriteCom(0x80 + Pos);
			break;
			case 1:
				_LCD12864_WriteCom(0x90 + Pos);
			break;
			case 2:
				_LCD12864_WriteCom(0x88 + Pos);
			break;
			case 3:
				_LCD12864_WriteCom(0x98 + Pos);
			break;
			default:
			return;
		}
		LCD12864_ShowCursor();
	}
	_LCD12864_Unlock();
}

static void _LCD12864_ExtendModeClear(void)
{
	rt_uint8_t i = 0, j = 0;
	for(j = 0; j < 32; j++){
		_LCD12864_WriteCom(0x80 + j);			//��д��ֱ��ַ
		_LCD12864_WriteCom(0x80);	//�°�������ʼ����Ϊ0x88
		for(i = 0; i < 16; i++){
			_LCD12864_WriteData(0x00);
		}
	}
	for(j = 0; j < 32; j++){
		_LCD12864_WriteCom(0x80 + j);			//��д��ֱ��ַ
		_LCD12864_WriteCom(0x88);	//�°�������ʼ����Ϊ0x88
		for(i = 0; i < 16; i++){
			_LCD12864_WriteData(0x00);
		}
	}
}

void LCD12864_ShowPixel(rt_uint8_t x, rt_uint8_t y)
{
	rt_uint8_t Flag = 0;
	rt_uint8_t TempLow = 0, TempHigh = 0;	//���ڴ����λ���λ����
	rt_uint8_t xByte = 0, xBit = 0;	//���������һ�ֽ���һλ
	rt_uint8_t yByte = 0, yBit = 0; //���������һ�ֽ���һλ
	if(_LCD12864_SwitchMode(EXTEND_CMD)){
		_LCD12864_WriteCom(0x34);		//��չָ�
		_LCD12864_ExtendModeClear();
		Flag = 1;
	}
	xByte = x/16;					//����ֵ�ַ(ÿ����ַ��16λ)
	xBit = x%16;				//���λ�ڵ�ǰ�ֵľ���λ��
	yByte = y/32;					//ȷ������һ��, 0�ϰ���  1�°���
	yBit = y%32;					//ȷ������һ��
//	rt_kprintf("xByte:%d, xBit:%d, yByte:%d, yBit:%d\n", xByte, xBit, yByte, yBit);
	
	_LCD12864_WriteCom(0x80 + yBit);			//��д��ֱ��ַ
	_LCD12864_WriteCom(0x80 + xByte + 8 * yByte);	//�°�������ʼ����Ϊ0x88
	
	if(xBit < 8){
		_LCD12864_WriteData(TempHigh | (0x01 << (7 - xBit)));
		_LCD12864_WriteData(TempLow);
	}else{
		_LCD12864_WriteData(TempHigh);
		_LCD12864_WriteData(TempLow | (0x01 << (15 - xBit)));
	}
	if(Flag){
		_LCD12864_WriteCom(0x36);		//����ͼ����
	}
}

void LCD12864_ShowLine(rt_uint8_t y, rt_uint8_t x0, rt_uint8_t x1)
{
	
}

void LCD12864_Test(void)
{
	LCD12864_Clear();
	rt_thread_mdelay(5);
	LCD12864_ShowString(1, 0, "12864 ��ʾ����");
}


