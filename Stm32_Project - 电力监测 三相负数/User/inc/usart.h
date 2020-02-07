#ifndef __USART__H
#define __USART__H
#include"stm32f10x.h"
#include<stdio.h>

/*变量声明*/
extern char Rx1Buf[50];
extern u8 Rx1Count;

/*函数声明*/
void USART1_Init(u32 pclk1, u32 baud);
void USART1_SendByte(u8 dat);
void USART1_SendString(char *s);
void USART1_IRQHandler(void);
void USART1_Send_Data(u8 *buf,u8 len);
#endif

