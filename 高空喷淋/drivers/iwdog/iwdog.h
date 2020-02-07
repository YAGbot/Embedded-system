#ifndef __IWDOG_H
#define __IWDOG_H
#include "stm32f10x.h"
 

void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);
#endif
