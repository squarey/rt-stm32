

#ifndef __BMP280_SUPPORT_H__
#define __BMP280_SUPPORT_H__
#include "stdio.h"
#include "bmp280.h"
#include "math.h"
#include "stdbool.h"
#include "I2C.h"
//在选择不同的宏以使用不同的通信方式,注意要使用相应的引脚
#define __I2C_ROUTINE__
//#define __SPI_ROUTINE__

//BMP280初始化
s8 BMP_280Init(void);

#endif

