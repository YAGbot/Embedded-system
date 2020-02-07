#ifndef __RS485__H
#define __RS485__H
#include "stm32f10x.h"
#include <stdio.h>
/*��������*/
#define USART2_REC_LEN  			200  	//�����������ֽ��� 200
#define USART2_SEND_LEN		200				//����ͻ����ֽ���
extern u8 RS485_TX_CNT;
extern u8  RS485_TX_BUF[USART2_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�	  	
extern u8  RS485_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	

/*��������*/
void RS485_Init(u32 pclk2,u32 baud);
void USART2_IRQHandler(void);
void USART2_SendByte(u8 dat);
void RS485_Send_Data(u8 *buf,u8 len);

#endif
