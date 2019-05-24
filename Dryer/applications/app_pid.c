

#include "app_pid.h"

/*
* 1、比例系数Kp的作用是加快系统的响应速度, 提高系统的调节精度。Kp越
*	 大，系统的响应速度越快，系统的调节精度越高，但很容易产生超调，甚至
*	 会使系统不稳定，Kp取值过小, 则会降低调节精度，使响应速度缓慢，从而
*	 延长调节时间，使系统静态，动态特性变差
* 2、积分作用系数Ki的作用是消除系统的稳态误差。Ki越大，系统的静态误差消除
*	 的越快, 但是Ki过大，在响应过程的初期会产生积分饱和的现象，从而引起响应
*	 过程的较大超调，若Ki过小，将使系统静态误差难以消除，影响系统的调节精度
* 3、微分系数Kd的作用是改善系统的动态特性，其作用主要是在响应过程中抑制偏差
*	 向任何方向的变化，对偏差变化进行提前预报，但是Kd过大，会使响应过程提前
*	 制动，从而延长调节时间，而且会降低的系统的抗干扰性。
*
*	 反应系统性能的两个参数是系统误差e和误差变化律ec。
*/


typedef struct _pid
{
	float 	SetValue;		//设定值
	float	ActualValue;	//实际值
	float	Err;			//偏差值
	float	ErrLast;		//上一次偏差值
	float	Kp,Ki,Kd;		//比例、积分、微分系数
	float	Voltage;		//电压值(控制执行器的变量)
	float	Integral;		//积分值
	float	Umax;			//误差允许最大值
	float	Umin;			//误差允许最小值
}pid;

static pid _pid_info;


void PID_init(void)
{
	rt_kprintf("PID_Init start.\n");
	
	_pid_info.SetValue = 0.0f;
	_pid_info.ActualValue = 0.0f;
	_pid_info.Err = 0.0f;
	_pid_info.ErrLast = 0.0f;
	_pid_info.Kp = 0.4f;
	_pid_info.Ki = 0.2f;
	_pid_info.Kd = 0.2f;
	_pid_info.Voltage = 0.0f;
	_pid_info.Integral = 0.0f;
	_pid_info.Umax = 400;
	_pid_info.Umin = -200;
	rt_kprintf("PID_Init end.\n");
}

float abs_float(float a)
{
	if(a > 0){
		return a;
	}else{
		return -a;
	}
}
#if 0
float PID_realize(float speed)
{
	rt_uint8_t index = 0;
	_pid_info.SetValue = speed;
	_pid_info.Err = _pid_info.SetValue - _pid_info.ActualValue;
	//_pid_info.Integral += _pid_info.Err;
	//抗积分饱和
	if(_pid_info.ActualValue > _pid_info.Umax){
		//积分分离
		if(abs_float(_pid_info.Err) > speed){
			index = 0;
		}else{
			index = 1;
			if(_pid_info.Err < 0){
				_pid_info.Integral += _pid_info.Err;
			}
		}
	}else if(_pid_info.ActualValue < _pid_info.Umin){
		//积分分离
		if(abs_float(_pid_info.Err) > speed){
			index = 0;
		}else{
			index = 1;
			if(_pid_info.Err > 0){
				_pid_info.Integral += _pid_info.Err;
			}
		}
	}else{
		if(abs_float(_pid_info.Err) > speed){
			index = 0;
		}else{
			index = 1;
			_pid_info.Integral += _pid_info.Err;
		}
	}
	//矩形积分公式
	_pid_info.Voltage = _pid_info.Kp * _pid_info.Err + _pid_info.Ki * _pid_info.Integral * index + 
					_pid_info.Kd * (_pid_info.Err - _pid_info.ErrLast);
	//梯形积分公式
/*	_pid_info.Voltage = _pid_info.Kp * _pid_info.Err + _pid_info.Ki * _pid_info.Integral/2 * index + 
					_pid_info.Kd * (_pid_info.Err - _pid_info.ErrLast);*/
	_pid_info.ErrLast = _pid_info.Err;
	_pid_info.ActualValue = _pid_info.Voltage * 1.0;
	return _pid_info.ActualValue;
}

#else 
/*
* 变积分算法
* 在某个误差值区间改变积分系数
*/
float PID_realize(float speed)
{
	rt_uint8_t index = 0;
	_pid_info.SetValue = speed;
	_pid_info.Err = _pid_info.SetValue - _pid_info.ActualValue;
	
	if(abs_float(_pid_info.Err) > speed){
		index = 0;
	}else if(abs_float(_pid_info.Err) < 180){
		index = 1;
		_pid_info.Integral += _pid_info.Err;
	}else{
		//变积分
		index = (speed - abs_float(_pid_info.Err))/20;
		_pid_info.Integral += _pid_info.Err;
	}
	_pid_info.Voltage = _pid_info.Kp * _pid_info.Err + _pid_info.Ki * _pid_info.Integral * index + 
					_pid_info.Kd * (_pid_info.Err - _pid_info.ErrLast);
	_pid_info.ErrLast = _pid_info.Err;
	_pid_info.ActualValue = _pid_info.Voltage * 1.0;
	return _pid_info.ActualValue;
}
#endif 
