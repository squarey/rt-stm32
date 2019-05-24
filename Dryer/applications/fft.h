

#ifndef _FFT_H_
#define _FFT_H_

#include <rtthread.h>

struct complex
{
	float real;
	float imag;
};


void FFT(struct complex *xin);

#endif /* _FFT_H_ */


