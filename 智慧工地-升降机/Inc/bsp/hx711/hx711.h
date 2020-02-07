#ifndef __HX711_H__
#define	__HX711_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

void hx711_delay(uint16_t time);
void ADInit(void); //初始化AD芯片，和它使用的I/O端口。
unsigned long ReadAD(void);  //读AD芯片输出的数据。

#endif
