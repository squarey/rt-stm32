

#include "app_pid.h"

/*
* 1������ϵ��Kp�������Ǽӿ�ϵͳ����Ӧ�ٶ�, ���ϵͳ�ĵ��ھ��ȡ�KpԽ
*	 ��ϵͳ����Ӧ�ٶ�Խ�죬ϵͳ�ĵ��ھ���Խ�ߣ��������ײ�������������
*	 ��ʹϵͳ���ȶ���Kpȡֵ��С, ��ή�͵��ھ��ȣ�ʹ��Ӧ�ٶȻ������Ӷ�
*	 �ӳ�����ʱ�䣬ʹϵͳ��̬����̬���Ա��
* 2����������ϵ��Ki������������ϵͳ����̬��KiԽ��ϵͳ�ľ�̬�������
*	 ��Խ��, ����Ki��������Ӧ���̵ĳ��ڻ�������ֱ��͵����󣬴Ӷ�������Ӧ
*	 ���̵Ľϴ󳬵�����Ki��С����ʹϵͳ��̬�������������Ӱ��ϵͳ�ĵ��ھ���
* 3��΢��ϵ��Kd�������Ǹ���ϵͳ�Ķ�̬���ԣ���������Ҫ������Ӧ����������ƫ��
*	 ���κη���ı仯����ƫ��仯������ǰԤ��������Kd���󣬻�ʹ��Ӧ������ǰ
*	 �ƶ����Ӷ��ӳ�����ʱ�䣬���һή�͵�ϵͳ�Ŀ������ԡ�
*
*	 ��Ӧϵͳ���ܵ�����������ϵͳ���e�����仯��ec��
*/


typedef struct _pid
{
	float 	SetValue;		//�趨ֵ
	float	ActualValue;	//ʵ��ֵ
	float	Err;			//ƫ��ֵ
	float	ErrLast;		//��һ��ƫ��ֵ
	float	Kp,Ki,Kd;		//���������֡�΢��ϵ��
	float	Voltage;		//��ѹֵ(����ִ�����ı���)
	float	Integral;		//����ֵ
	float	Umax;			//����������ֵ
	float	Umin;			//���������Сֵ
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
	//�����ֱ���
	if(_pid_info.ActualValue > _pid_info.Umax){
		//���ַ���
		if(abs_float(_pid_info.Err) > speed){
			index = 0;
		}else{
			index = 1;
			if(_pid_info.Err < 0){
				_pid_info.Integral += _pid_info.Err;
			}
		}
	}else if(_pid_info.ActualValue < _pid_info.Umin){
		//���ַ���
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
	//���λ��ֹ�ʽ
	_pid_info.Voltage = _pid_info.Kp * _pid_info.Err + _pid_info.Ki * _pid_info.Integral * index + 
					_pid_info.Kd * (_pid_info.Err - _pid_info.ErrLast);
	//���λ��ֹ�ʽ
/*	_pid_info.Voltage = _pid_info.Kp * _pid_info.Err + _pid_info.Ki * _pid_info.Integral/2 * index + 
					_pid_info.Kd * (_pid_info.Err - _pid_info.ErrLast);*/
	_pid_info.ErrLast = _pid_info.Err;
	_pid_info.ActualValue = _pid_info.Voltage * 1.0;
	return _pid_info.ActualValue;
}

#else 
/*
* ������㷨
* ��ĳ�����ֵ����ı����ϵ��
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
		//�����
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
