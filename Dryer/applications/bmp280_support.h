

#ifndef __BMP280_SUPPORT_H__
#define __BMP280_SUPPORT_H__
#include "stdio.h"
#include "bmp280.h"
#include "math.h"
#include "stdbool.h"
#include "I2C.h"
//��ѡ��ͬ�ĺ���ʹ�ò�ͬ��ͨ�ŷ�ʽ,ע��Ҫʹ����Ӧ������
#define __I2C_ROUTINE__
//#define __SPI_ROUTINE__

//BMP280��ʼ��
s8 BMP_280Init(void);

#endif

