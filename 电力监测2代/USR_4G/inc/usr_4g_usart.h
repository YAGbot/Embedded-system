#ifndef __USR_4G_USART_H
#define __USR_4G_USART_H 			   
#include "sys.h"

/*��������*/
#define USART1_MAX_RECV_LEN		1024				//�����ջ����ֽ���
#define USART1_MAX_SEND_LEN		1024				//����ͻ����ֽ���

extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART1_RX_STA;   						//��������״̬

/*��������*/
void USART1_SendDat(u8 dat);
void USART1_SendString(char *str);
void gsm_fputc(char str);
void gsm_printf(char *str,...);
void sim7020_usart_init(u32 bound);

#endif


