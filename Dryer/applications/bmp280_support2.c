#include "bmp280_support.h"

#ifdef __I2C_ROUTINE__
extern s8 I2C_routine(void);
#endif 
#ifdef __SPI_ROUTINE__
extern s8 SPI_routine(void);
#endif 
extern struct bmp280_t bmp280;
/****************************************************
* 函数名称：BMP_280Init
* 功能描述：BMP280初始化
* 输入参数：无
* 输出参数：无
* 返回值：返回0成功，返回-1失败
*****************************************************/
s8 BMP_280Init(void)
{
	#ifdef __I2C_ROUTINE__  
		#ifdef __SPI_ROUTINE__
			#error "大神，你选择了两种通信方式同时用？这是什么操作?"
		#endif
	#endif
    #ifdef __I2C_ROUTINE__
        I2C_routine();
    #else
        #ifdef __SPI_ROUTINE__
            Init_myspi();
            SPI_routine();
        #else
            #error "1540909218:请选择宏__I2C_ROUTINE__或__SPI_ROUTINE__！！"
        #endif
    #endif
    if(bmp280_init(&bmp280)){
		rt_kprintf("BMP280 Init successful\n");
		return (s8)-1;
	}
		
    return 0;
}

