#ifndef __4G_USART_H
#define __4G_USART_H 			   
#include "sys.h"

/*变量声明*/
#define UART5_MAX_RECV_LEN		1024				//最大接收缓存字节数
#define UART5_MAX_SEND_LEN		1024				//最大发送缓存字节数

extern u8 UART5_RX_REC_ATCOMMAD;	//at命令时间内
extern u8  UART5_RX_BUF[UART5_MAX_RECV_LEN]; 		//接收缓冲,最大UART5_MAX_RECV_LEN字节
extern u8  UART5_TX_BUF[UART5_MAX_SEND_LEN]; 		//发送缓冲,最大UART5_MAX_SEND_LEN字节
extern u16 UART5_RX_STA;   						//接收数据状态

/*函数声明*/
void UART5_SendDat(u8 dat);
void UART5_SendString(char *str);
void gsm_fputc(char str);
void gsm_printf(char *str,...);
void gprs_usart_init(u32 bound);
void GSM_Init(void);
int check_buff(u8 *buff, char *str);
u8* gprs_check_cmd(u8 *str);
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

/*函数声明*/
void TIM7_Init(void);
void time7_start(void);
void time7_stop(void);
void time7_set_timeout(u16 time);
void time7_reset(void);

#endif


