#ifndef __GU900_UART__H
#define __GU900_UART__H
#include <stm32f10x.h>

/*��������*/
extern u8 gu900_uart_buff[250];	
extern u16 gu900_uart_len;

/*��������*/
void USART3_Init(u32 pclk1,u32 baud);
//void USART3_IRQHandler(void);
void USART3_SendDat(u8 dat);
void USART3_SendString(char *str);
void gu900_fputc(char str);
void gu900_printf(char *str,...);

#endif
