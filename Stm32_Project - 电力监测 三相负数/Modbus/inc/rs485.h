#ifndef __RS485__H
#define __RS485__H
#include "stm32f10x.h"
#include <stdio.h>
/*变量声明*/
extern u8 RS485_RX_BUF[64];  	
extern u8 RS485_RX_CNT;

extern u8 RS485_TX_BUF[64];  	
extern u8 RS485_TX_CNT;

extern u8 rs485_recv_flag;
extern u16 USART2_RX_STA;   						//接收数据状态
/*函数声明*/
void RS485_Init(u32 pclk2,u32 baud);
void USART2_IRQHandler(void);
void RS485_Receive_Data(u8 *buf);
void USART2_SendByte(u8 dat);
void RS485_Send_Data(u8 *buf,u8 len);

#endif
