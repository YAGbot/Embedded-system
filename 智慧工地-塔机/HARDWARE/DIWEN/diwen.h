#ifndef __DIWEN__H
#define __DIWEN__H

#include "sys.h"
#include <stdio.h>
   
/*变量声明*/
#define USART4_REC_LEN  			200  	//定义最大接收字节数 200
#define USART4_SEND_LEN		200				//最大发送缓存字节数
extern u8 USART4_TX_CNT;
extern u8  USART4_TX_BUF[USART4_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节	  	
extern u8  USART4_RX_BUF[USART4_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART4_RX_STA;         		//接收状态标记	
extern float my5010[3][13];
/*函数声明*/
void UART4_Init(u32 bound);
void USART4_SendByte(u8 dat);
void USART4_SendDat(u8 *buf,u8 len);
void USART4_SendString(char *str);
void diwen_send_data16(uint16_t _reg,int16_t _data);
void diwen_send_numascii(u16 _reg,u8 *buf,u8 len);
void diwen_send_rectangle(u16 _reg,u16 x1,u16 y1,u16 x2,u16 y2);
void liebiao_view(void);
#endif


