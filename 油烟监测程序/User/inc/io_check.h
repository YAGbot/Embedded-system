#ifndef __IO_CHECK_H
#define	__IO_CHECK_H

#include "sys.h"

#define FIFO1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//读取PC0
#define FIFO2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//读取PC1
#define FIFO3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//读取PC2 
#define FIFO4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)//读取PC3

u8 io_init(void);
void IO_check_init(void);
void io_poll(void);

#endif
