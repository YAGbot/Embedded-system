#ifndef _RS485__H
#define _RS485__H

#include "stm32f10x.h"
#include <stdio.h>

/*变量声明*/
#define USART1_REC_LEN  			200  	//定义最大接收字节数 200
#define USART1_SEND_LEN		200				//最大发送缓存字节数
extern u8 USART1_TX_CNT;
extern u8  USART1_TX_BUF[USART1_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节	  	
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记	

/*函数声明*/
void uart1_init(u32 bound);
void USART1_SendByte(u8 dat);
void USART1_SendDat(u8 *buf,u8 len);
void USART1_SendString(char *str);
void get_data(void);
void time4_start(void);
void time4_stop(void);
void TIM4_Init(u16 psc,u16 arr);

#endif
