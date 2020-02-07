#ifndef __IO_CHECK_H
#define	__IO_CHECK_H

#include "sys.h"

#define FIFO1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//∂¡»°PC0
#define FIFO2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//∂¡»°PC1
#define FIFO3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//∂¡»°PC2 

u8 io_init(void);
void IO_check_init(void);
void io_poll(void);

#endif
