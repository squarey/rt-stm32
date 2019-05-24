

#ifndef _LCD12864_H
#define _LCD12864_H

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_gpio.h"

#define CMD_BASE_COMMAND			0x30		//基本指令集
#define CMD_DIS_OFF_CURSOR_OFF		0x08		//显示关游标位置关
#define CMD_DIS_ON_CURSOR_OFF		0x0c		//显示开游标位置关
#define CMD_DIS_ON_CURSOR_ON		0x0f		//显示开游标位置开
#define CMD_SHOW_CURSOR_ORIGIN		0x02		//显示位置回原点 游标回到原点
#define CMD_DIS_CLEAR				0x01		//清屏
#define CMD_CURSOR_MOVE_LEFT		0x10		//光标左移
#define CMD_CURSOR_MOVE_RIGHT		0x14		//光标右移


void LCD12864_Init(void);
void LCD12864_Clear(void);
void LCD12864_ShowString(rt_uint8_t Line, rt_uint8_t Pos, const char *pStr);
void LCD12864_ShowNumber(rt_uint8_t Line, rt_uint8_t Pos, rt_int32_t Number);
void LCD12864_ShowCursor(void);
void LCD12864_HideCursor(void);
void LCD12864_SetCursor(rt_uint8_t Line, rt_uint8_t Pos);
void LCD12864_Test(void);

#endif /* _LCD12864_H */


