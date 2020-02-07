#ifndef __RS485__H
#define __RS485__H
#include "stm32f10x.h"
#include <stdio.h>
/*变量声明*/
#define USART2_REC_LEN  			200  	//定义最大接收字节数 200
#define USART2_SEND_LEN		200				//最大发送缓存字节数
extern u8 RS485_TX_CNT;
extern u8  RS485_TX_BUF[USART2_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节	  	
extern u8  RS485_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART2_RX_STA;         		//接收状态标记	

/*函数声明*/
void RS485_Init(u32 pclk2,u32 baud);
void USART2_IRQHandler(void);
void USART2_SendByte(u8 dat);
void RS485_Send_Data(u8 *buf,u8 len);

#endif
