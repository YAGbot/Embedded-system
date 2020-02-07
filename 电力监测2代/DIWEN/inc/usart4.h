#ifndef __USART4__H
#define __USART4__H

#include "sys.h"
#include <stdio.h>
   
/*��������*/
#define USART4_REC_LEN  			200  	//�����������ֽ��� 200
#define USART4_SEND_LEN		200				//����ͻ����ֽ���
extern u8 USART4_TX_CNT;
extern u8  USART4_TX_BUF[USART4_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�	  	
extern u8  USART4_RX_BUF[USART4_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART4_RX_STA;         		//����״̬���	

/*��������*/
void UART4_Init(u32 bound);
void USART4_SendByte(u8 dat);
void USART4_SendDat(u8 *buf,u8 len);
void USART4_SendString(char *str);

#endif


