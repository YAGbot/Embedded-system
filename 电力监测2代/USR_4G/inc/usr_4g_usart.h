#ifndef __USR_4G_USART_H
#define __USR_4G_USART_H 			   
#include "sys.h"

/*变量声明*/
#define USART1_MAX_RECV_LEN		1024				//最大接收缓存字节数
#define USART1_MAX_SEND_LEN		1024				//最大发送缓存字节数

extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART1_RX_STA;   						//接收数据状态

/*函数声明*/
void USART1_SendDat(u8 dat);
void USART1_SendString(char *str);
void gsm_fputc(char str);
void gsm_printf(char *str,...);
void sim7020_usart_init(u32 bound);

#endif


