#ifndef __HX711_H__
#define	__HX711_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

void hx711_delay(uint16_t time);
void ADInit(void); //��ʼ��ADоƬ������ʹ�õ�I/O�˿ڡ�
unsigned long ReadAD(void);  //��ADоƬ��������ݡ�

#endif
