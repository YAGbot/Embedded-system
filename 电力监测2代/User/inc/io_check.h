#ifndef __IO_CHECK_H
#define	__IO_CHECK_H

#include "sys.h"

#define FIFO8  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//∂¡»°PC0


void IO_check_init(void);
void EXTIX_Init(void);



#endif
