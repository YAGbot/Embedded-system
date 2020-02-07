#include "usart4.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"	 
#include "delay.h"
#include "timer.h"

#if 1
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
int fputc(int ch, FILE *f)
{      
	while((UART4->SR&0X40)==0);//ѭ������,ֱ���������   
    UART4->DR = (u8) ch;      
	return ch;
}
#endif 

u8 USART4_RX_BUF[USART4_REC_LEN];				 //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART4_TX_BUF[USART4_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
u8 USART4_TX_CNT=0;
//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
//u16 USART4_RX_STA=0;   	 
//void UART4_IRQHandler(void)
//{
//	u8 res;	    
//	if(UART4->SR&(1<<5))//���յ�����
//	{	 
//		res=UART4->DR; 			 
//		if(USART4_RX_STA<USART4_REC_LEN)		//�����Խ�������
//		{
//			TIM4->CNT=0;         					//���������
//			if(USART4_RX_STA==0) time4_start();	 	//ʹ�ܶ�ʱ��4���ж� 
//			USART4_RX_BUF[USART4_RX_STA++]=res;		//��¼���յ���ֵ	 
//		}else 
//		{
//			USART4_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
//		} 
//	}  											 
//}   

/* ����4 TX = PC10   RX = PC11 */
void UART4_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* ��1���� ����GPIO��UARTʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
	*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ��4���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = bound;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	
	USART_Init(UART4, &USART_InitStructure);

	 /* ��5���� NVIC ���� */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	USART_Cmd(UART4, ENABLE);		/* ʹ�ܴ��� */
}


/************************************************************************************
* �������ƣ�UART4_SendByte
* ����������UART4����һ���ַ�����
* ��ڲ�����-dat:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void USART4_SendByte(u8 dat)
{
	while(!(UART4->SR&(1<<6)));			//�ȴ���λ�Ĵ���Ϊ��
	UART4->DR = dat;
}

/************************************************************************************
* �������ƣ�UART_SendDat
* ����������UART4����len���ַ�����
* ��ڲ�����-dat:Ҫ���͵����� len:Ҫ�������ݵĳ���
* ���ڲ�������
*************************************************************************************/
void USART4_SendDat(u8 *buf,u8 len)
{
	u8 i;
  for(i=0;i<len;i++)		//ѭ����������
	{
		while((UART4->SR&0X40)==0);//�ȴ����ͽ���		  
		UART4->DR=buf[i];
	}	 
}



