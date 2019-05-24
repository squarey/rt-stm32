

#include "fft.h"
#include <math.h>


#define FFT_N 	1024

#define PI		3.14159f





/************************************
函数原型: struct complex EE(struct complex a, struct complex b)
函数功能: 对两个负数进行乘法运算
输入参数: 两个以结构体定义的负数a, b
输出参数: a和b的乘机, 以结构体形式输出
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
	nv2 = FFT_N/2;			//变址运算 即把自然序列变成倒位序 采用雷德算法
	nm1 = FFT_N - 1;
	for(i = 0; i < nm1; i++){
		if(i < j){
			t = xin[j];
			xin[j] = xin[i];
			xin[i] = t;
		}
		k = nv2;		//求j的下一个倒位序
		while(k <= j){	//如果k <= j 表示j的最高位为1
			j = j - k;	//把最高位变成0
			k = k/2;	//k/2 比较次高位 以此类推 逐个比较 直到某个位为0 
		}
		j = j + k;		//把0变成1
	}
	{
		//FFT 运算核 使用蝶形运算完成FFT运算
		int le = 0, lei = 0, ip = 0;
		f = FFT_N/2;
		rt_kprintf("********************************\r\n");
		rt_kprintf("f/2=%d\n", f/2);
		
		for(l = 1; f != l; l++){	//计算l的值 即计算蝶形级数
			//rt_kprintf("l:%d\n", l);
		}
		rt_kprintf("-------------------------------------\r\n");
		for(m = 1; m <= 1; m++){			//控制蝶形结级数
			le = 2 << (m - 1);				//表示第m级蝶形  l为蝶形m级总数l = log(2)N
											//蝶形结距离 即第m级蝶形的蝶形结相距le点
			lei = le/2;						//同一蝶形结中参加运算的两点的距离
			u.real = 1.0f;					//蝶形结初始运算系数u 初始值为1
			u.imag = 0.0f;	
			w.real = cos(PI/lei);			//系数商 当前系数与前一个系数的商
			w.imag = sin(PI/lei);
			
			for(j = 0; j <= lei - 1; j++){	//控制计算不同种蝶形结 即计算系数不同的蝶形结
				for(i = j; i <= FFT_N - 1; i += le){// 控制同一蝶形结运算  即计算系数相同的蝶形结
					ip = i + lei;			//ip分别表示灿姐蝶形运算的两个节点
					t = _EE(xin[ip], u);	//蝶形运算
					xin[ip].real = xin[i].real - t.real;
					xin[ip].imag = xin[i].imag - t.imag;
					xin[i].real = xin[i].real + t.real;
					xin[i].imag = xin[i].imag + t.imag;
				}
				u = _EE(u, w);				//改变系数 进行下一个蝶形运算	
			}
		}
	}
}

