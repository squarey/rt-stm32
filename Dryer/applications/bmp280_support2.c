#include "bmp280_support.h"

#ifdef __I2C_ROUTINE__
extern s8 I2C_routine(void);
#endif 
#ifdef __SPI_ROUTINE__
extern s8 SPI_routine(void);
#endif 
extern struct bmp280_t bmp280;
/****************************************************
* �������ƣ�BMP_280Init
* ����������BMP280��ʼ��
* �����������
* �����������
* ����ֵ������0�ɹ�������-1ʧ��
*****************************************************/
s8 BMP_280Init(void)
{
	#ifdef __I2C_ROUTINE__  
		#ifdef __SPI_ROUTINE__
			#error "������ѡ��������ͨ�ŷ�ʽͬʱ�ã�����ʲô����?"
		#endif
	#endif
    #ifdef __I2C_ROUTINE__
        I2C_routine();
    #else
        #ifdef __SPI_ROUTINE__
            Init_myspi();
            SPI_routine();
        #else
            #error "1540909218:��ѡ���__I2C_ROUTINE__��__SPI_ROUTINE__����"
        #endif
    #endif
    if(bmp280_init(&bmp280)){
		rt_kprintf("BMP280 Init successful\n");
		return (s8)-1;
	}
		
    return 0;
}

