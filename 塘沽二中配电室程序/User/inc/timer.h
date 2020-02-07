#ifndef __TIMER_H
#define __TIMER_H 		   
#include "stm32f10x.h"

#define CAPTURETIME 57		//采集周期	单位s

extern u8 worktimes_flag;

void time6_start(void);
void time6_stop(void);
void TIM6_Init(u16 psc,u16 arr);
#endif
