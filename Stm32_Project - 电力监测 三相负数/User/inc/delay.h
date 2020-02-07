#ifndef __DELAY__H
#define __DELAY__H 		   
#include "stm32f10x.h"


/*º¯ÊıÉùÃ÷*/
void delay_init(u8 HCLK);
void delay_us(u32 nus);
void delay_nms(u16 nms);
void delay_ms(u16 ms);

#endif
