#ifndef __RS485_H
#define __RS485_H 			   
#include "sys.h"

/*变量声明*/
extern u8 RS485_RX_BUF[64];  	
extern u8 RS485_RX_CNT;

extern u8 RS485_TX_BUF[64];  	
extern u8 RS485_TX_CNT;

extern u8 rs485_recv_flag;

/*函数声明*/
void RS485_Init(u32 pclk2,u32 baud);
void USART3_SendByte(u8 dat);
void RS485_Receive_Data(u8 *buf);
void RS485_Send_Data(u8 *buf,u8 len);
void TIME6_Init(u16 arr,u16 psc);
void time6_start(void);
void time6_stop(void);
void time6_set_timeout(u16 time);
void time6_reset(void);

#endif

