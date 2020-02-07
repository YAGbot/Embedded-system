#ifndef __TIMER_H
#define __TIMER_H 		   
#include "sys.h"

extern u8 worktimes_flag;

void time3_start(void);
void time3_stop(void);
void TIM3_Init(u16 psc,u16 arr);

#endif
