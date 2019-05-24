

#ifndef _LCD12864_H
#define _LCD12864_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_gpio.h"

#define CMD_BASE_COMMAND			0x30		//����ָ�
#define CMD_DIS_OFF_CURSOR_OFF		0x08		//��ʾ���α�λ�ù�
#define CMD_DIS_ON_CURSOR_OFF		0x0c		//��ʾ���α�λ�ù�
#define CMD_DIS_ON_CURSOR_ON		0x0f		//��ʾ���α�λ�ÿ�
#define CMD_SHOW_CURSOR_ORIGIN		0x02		//��ʾλ�û�ԭ�� �α�ص�ԭ��
#define CMD_DIS_CLEAR				0x01		//����
#define CMD_CURSOR_MOVE_LEFT		0x10		//�������
#define CMD_CURSOR_MOVE_RIGHT		0x14		//�������


void LCD12864_Init(void);
void LCD12864_Clear(void);
void LCD12864_ShowString(rt_uint8_t Line, rt_uint8_t Pos, const char *pStr);
void LCD12864_ShowNumber(rt_uint8_t Line, rt_uint8_t Pos, rt_int32_t Number);
void LCD12864_ShowCursor(void);
void LCD12864_HideCursor(void);
void LCD12864_SetCursor(rt_uint8_t Line, rt_uint8_t Pos);
void LCD12864_Test(void);

#endif /* _LCD12864_H */


