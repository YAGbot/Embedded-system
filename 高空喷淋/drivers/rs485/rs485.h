#ifndef _RS485__H
#define _RS485__H

#include "stm32f10x.h"
#include <stdio.h>

/*��������*/
#define USART1_REC_LEN  			200  	//�����������ֽ��� 200
#define USART1_SEND_LEN		200				//����ͻ����ֽ���
extern u8 USART1_TX_CNT;
extern u8  USART1_TX_BUF[USART1_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�	  	
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���	

/*��������*/
void uart1_init(u32 bound);
void USART1_SendByte(u8 dat);
void USART1_SendDat(u8 *buf,u8 len);
void USART1_SendString(char *str);
void get_data(void);
void time4_start(void);
void time4_stop(void);
void TIM4_Init(u16 psc,u16 arr);

#endif
