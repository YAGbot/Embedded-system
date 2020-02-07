#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"

void delay_init(void);
void delay_nms(u16 nms);
void delay_us(u32 nus);
void delay_ms(u16 ms);

#endif

