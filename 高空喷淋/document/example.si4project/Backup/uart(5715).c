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
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;

	astrx[astrx_len++%4096] = ch;

#if 0
	static u16 x = 0, y = 0;	
	u8 string[16]; 
	
	if(ch == '\n')   //����	
	{	 
		x = 0;   
		y += 12;  
		
		return ch;  
	}  

	if(x > X_MAX - 6) //X_MAX�Ǻ궨��, ��ֵΪ240  
	{
		x = 0;//x����	
		y += 12;//y������һ��  
	}  
	
	if(y > Y_MAX - 12) //Y_MAX�Ǻ궨��, ��ֵ��320	
	{	 
		y = 0;
		oled_clear();
		return ch; 	 //ֱ���˳�
	}	

	oled_char(x, y, ch, 12, 1);
	//oled_str(x, y, line_buf, 11);
	

	x += 8;				  //��ת����һ��λ��, �Ƿ�Խ�������溯���ж�

#endif	


	return ch;
}


void uart1_send_byte(u8 c)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) c;

}

//
void uart1_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	//NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	//NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = /*USART_Mode_Rx |*/ USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}





