#ifndef _IOCHECK_H
#define _IOCHECK_H

#include "stm32f10x.h"

#define OPEN  1
#define CLOSE 0

#define SWITCH1(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_11);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_11)

#define SWITCH2(a)	if (a)	\
					GPIO_SetBits(GPIOA,GPIO_Pin_12);\
					else		\
					GPIO_ResetBits(GPIOA,GPIO_Pin_12)
					
#define SWITCH3(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_6);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_6)
					
#define SWITCH4(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_7);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_7)
					
#define SWITCH5(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_9);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define FIFO1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)//��ȡPA11
#define FIFO2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)//��ȡPA12
#define FIFO3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//��ȡPB6 
#define FIFO4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//��ȡPB7
#define FIFO5  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡPB9
					
/* �������� */
void iocheck_init(void);



#endif
