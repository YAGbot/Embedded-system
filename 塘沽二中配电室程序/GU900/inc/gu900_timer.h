#ifndef __GU900_TIMER__H
#define __GU900_TIMER__H

#include "stm32f10x.h"                


/*��������*/
void tim7_irq_init(void);
void TIME7_Init(void);
void time7_start(void);
void time7_stop(void);
void time7_set_timeout(u16 time);
void time7_reset(void);
void TIM7_IRQHandler(void);

#endif
