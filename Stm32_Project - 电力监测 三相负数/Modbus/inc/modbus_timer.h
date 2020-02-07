#ifndef __MODBUS_TIMER__H
#define __MODBUS_TIMER__H

#include "stm32f10x.h"                

/*º¯ÊýÉùÃ÷*/
void tim7_irq_init(void);
void TIME7_Init(u16 arr,u16 psc);
void time7_start(void);
void time7_stop(void);
void time7_set_timeout(u16 time);
void time7_reset(void);
void TIM7_IRQHandler(void);

#endif
