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

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_UART5_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void UART5_Send_Data(u8 *buf,u8 len);
void uart5_init(u32 bound);

#endif
