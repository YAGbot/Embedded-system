#ifndef __UART_H
#define __UART_H
#include "stdio.h"	
#include "sys.h"

//#define DEBUG
#ifdef DEBUG
//#define debug(argc,argv...) ({printf("%s:%s:%05d=>",__FILE__,__func__,__LINE__);printf(argc,##argv);})
#define debug(argc,argv...) printf(argc,##argv)
#else 
#define debug(argc,argv...)
#endif

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_UART5_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void UART5_Send_Data(u8 *buf,u8 len);
void uart5_init(u32 bound);

#endif
