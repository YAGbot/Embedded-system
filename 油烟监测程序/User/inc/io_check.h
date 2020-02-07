#ifndef __IO_CHECK_H
#define	__IO_CHECK_H

#include "sys.h"

#define FIFO1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//��ȡPC0
#define FIFO2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//��ȡPC1
#define FIFO3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//��ȡPC2 
#define FIFO4  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)//��ȡPC3

u8 io_init(void);
void IO_check_init(void);
void io_poll(void);

#endif
