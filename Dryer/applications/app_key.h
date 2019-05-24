

#ifndef _APP_KEY_H
#define _APP_KEY_H


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define KEY_PRESS_NONE          0
#define KEY_PRESS_SHORT         1
#define KEY_PRESS_LONG          2
#define KEY_PRESS_CONTINUE      3
#define KEY_PRESS_CHECKING		4


void APP_KeyThreadInit(void);
void APP_KeyThreadStart(void);
void APP_KeyThreadExit(void);
void APP_KeySetPressCallback(void (*pFunc)(rt_uint8_t KeyIndex, rt_uint8_t PressStatus));
void APP_ADCKeySetPressCallback(void (*pFunc)(rt_uint8_t KeyIndex, rt_uint8_t PressStatus));


#endif /* _APP_KEY_H */



