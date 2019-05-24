

#ifndef _APP_PID_H
#define _APP_PID_H
#include <rtthread.h>



void PID_init(void);
float PID_realize(float speed);


#endif /* _APP_PID_H */


