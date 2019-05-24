

#include "fft.h"
#include <math.h>


#define FFT_N 	1024

#define PI		3.14159f





/************************************
����ԭ��: struct complex EE(struct complex a, struct complex b)
��������: �������������г˷�����
�������: �����Խṹ�嶨��ĸ���a, b
�������: a��b�ĳ˻�, �Խṹ����ʽ���
*****************************************/

static struct complex _EE(struct complex a, struct complex b)
{
	struct complex c;
	c.real = a.real * b.real - a.imag * b.imag;
	c.imag = a.real * b.imag + a.imag * b.real;
	return c;
}


/****************************


************************************/

void FFT(struct complex *xin)
{
	int f = 0, m = 0, nv2 = 0, nm1 = 0, i = 0, k = 0, l = 0, j = 0;
	struct complex u, w, t;
	nv2 = FFT_N/2;			//��ַ���� ������Ȼ���б�ɵ�λ�� �����׵��㷨
	nm1 = FFT_N - 1;
	for(i = 0; i < nm1; i++){
		if(i < j){
			t = xin[j];
			xin[j] = xin[i];
			xin[i] = t;
		}
		k = nv2;		//��j����һ����λ��
		while(k <= j){	//���k <= j ��ʾj�����λΪ1
			j = j - k;	//�����λ���0
			k = k/2;	//k/2 �Ƚϴθ�λ �Դ����� ����Ƚ� ֱ��ĳ��λΪ0 
		}
		j = j + k;		//��0���1
	}
	{
		//FFT ����� ʹ�õ����������FFT����
		int le = 0, lei = 0, ip = 0;
		f = FFT_N/2;
		rt_kprintf("********************************\r\n");
		rt_kprintf("f/2=%d\n", f/2);
		
		for(l = 1; f != l; l++){	//����l��ֵ ��������μ���
			//rt_kprintf("l:%d\n", l);
		}
		rt_kprintf("-------------------------------------\r\n");
		for(m = 1; m <= 1; m++){			//���Ƶ��νἶ��
			le = 2 << (m - 1);				//��ʾ��m������  lΪ����m������l = log(2)N
											//���ν���� ����m�����εĵ��ν����le��
			lei = le/2;						//ͬһ���ν��вμ����������ľ���
			u.real = 1.0f;					//���ν��ʼ����ϵ��u ��ʼֵΪ1
			u.imag = 0.0f;	
			w.real = cos(PI/lei);			//ϵ���� ��ǰϵ����ǰһ��ϵ������
			w.imag = sin(PI/lei);
			
			for(j = 0; j <= lei - 1; j++){	//���Ƽ��㲻ͬ�ֵ��ν� ������ϵ����ͬ�ĵ��ν�
				for(i = j; i <= FFT_N - 1; i += le){// ����ͬһ���ν�����  ������ϵ����ͬ�ĵ��ν�
					ip = i + lei;			//ip�ֱ��ʾ�ӽ��������������ڵ�
					t = _EE(xin[ip], u);	//��������
					xin[ip].real = xin[i].real - t.real;
					xin[ip].imag = xin[i].imag - t.imag;
					xin[i].real = xin[i].real + t.real;
					xin[i].imag = xin[i].imag + t.imag;
				}
				u = _EE(u, w);				//�ı�ϵ�� ������һ����������	
			}
		}
	}
}

