/**
  ******************************************************************************
  * @file    uart.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   UART ��������
  ******************************************************************************
  */
#include "uart.h"
#include "stm32f10x.h"
#include <stdio.h>




#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
u8 astrx[4096] = {0};
u16 astrx_len = 0;
int fputc(int ch, FILE *f)
{     
#define X_MAX 128
#define Y_MAX 64
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
	USART3->DR = (u8) ch;
	

	return ch;
}




void uart2_send_byte(u8 c)
{
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
	USART3->DR = (u8) c;

}

//
void uart2_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ʹ�� USART3 ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 

	/* USART3 ʹ��IO�˿����� */    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);    
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);   //��ʼ��GPIOA
	  
	/* USART3 ����ģʽ���� */
	USART_InitStructure.USART_BaudRate = bound;	//���������ã�9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ�����ã�8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//ֹͣλ���ã�1λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;  //�Ƿ���żУ�飺��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ��������ģʽ���ã�û��ʹ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�����뷢�Ͷ�ʹ��
	USART_Init(USART3, &USART_InitStructure);  //��ʼ��USART3
	
    /*ʹ�ܴ���3�ķ��ͺͽ����ж�*/
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    

	USART_Cmd(USART3, ENABLE);// USART3ʹ��
}





