#include "rs485.h"
#include "sys.h"
#include "delay.h"
#include "modbus_timer.h"
#include "string.h"

u8 RS485_RX_BUF[USART2_REC_LEN];				 //���ջ���,���USART_REC_LEN���ֽ�.
u8 RS485_TX_BUF[USART2_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
u8 RS485_TX_CNT=0;
u16 USART2_RX_STA=0;   
/************************************************************************************
* �������ƣ�RS485_Init
* ����������USART2��ʼ������(PA2.PA3)
* ��ڲ�����-baud:������
*           -pclk2:APB2����ʱ��Ƶ��,��λMHZ
* ���ڲ�������
*************************************************************************************/
void RS485_Init(u32 pclk2,u32 baud)
{
	float USARTDIV;
	u16 DIV_Mantissa;		//��������
	u16 DIV_Fraction;		//С������
	
	pclk2 = pclk2 * 1000000;		//��λת��ΪHz
	
	/*USART2��Tx��Rx�˿�����*/
	RCC->APB2ENR |= 1<<2;		 										//��ʼ��PA��ʱ��
	GPIOA->CRL &= ~(0XF<<(4*2));								//Tx(PA2)�ܽ�Ϊ���츴�����,50MHZ
	GPIOA->CRL |= (0XB<<(4*2));
	GPIOA->CRL &= ~(0xF<<(4*3));    						//Rx(PA3)�ܽ�Ϊ��������
	GPIOA->CRL |= (0x4<<(4*3));
	//GPIOA->ODR |= 1<<3;
	
	RCC->APB1ENR |= 1<<17;											//ʹ��USART2ʱ��
	RCC->APB1RSTR |= 1<<17;											//��λUSART2,��Ҫ��λ�����ȶ�����
	RCC->APB1RSTR &= ~(1<<17);									//ֹͣ��λ
	
	/*USART2��������*/
	USART2->CR1 |= 1<<13;												//UEλ��1ʹ��USART
	USART2->CR1 &= ~(1<<12);										//���ݳ���Ϊ8
	USART2->CR2 &= ~((1<<12)|(1<<13));					//1λֹͣλ
	
	USARTDIV = (float)pclk2/baud/16;						//��baud=fck/(16*USARTDIV)����
	DIV_Mantissa = USARTDIV;										//�õ���������
	DIV_Fraction = (USARTDIV-DIV_Mantissa)*16;	//�õ�С������
	USART2->BRR = (DIV_Mantissa<<4) + DIV_Fraction;	//�õ�BRR��ֵ
	
	NVIC_SetPriority(USART2_IRQn,NVIC_EncodePriority(7-2,2,2));
							//USART2�ж�ͨ��ʹ��  
	NVIC_EnableIRQ(USART2_IRQn);
	USART2->CR1|=(1<<5);							//ʹ�ܽ����ж� 
	
	USART2->CR1 |= 1<<3;												//����ʹ��
	USART2->CR1 |= 1<<2;												//����ʹ��
	
	TIME7_Init(99,7199);		//10ms�ж�
	USART2_RX_STA=0;		//����
	time7_stop();			//�رն�ʱ��
}


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
	
void USART2_IRQHandler(void)
{
	u8 res;	    
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		res=USART2->DR; 			 
		if(USART2_RX_STA<USART2_REC_LEN)		//�����Խ�������
		{
			TIM7->CNT=0;         					//���������
			if(USART2_RX_STA==0) time7_start();	 	//ʹ�ܶ�ʱ��4���ж� 
			RS485_RX_BUF[USART2_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART2_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	}  											 	 
}   


/************************************************************************************
* �������ƣ�USART2_SendByte
* ����������USART3����һ���ַ�����
* ��ڲ�����-dat:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void USART2_SendByte(u8 dat)
{
	while(!(USART2->SR&(1<<6)));			//�ȴ���λ�Ĵ���Ϊ��
	USART2->DR = dat;
}

/************************************************************************************
* �������ƣ�RS485_Send_Data
* ����������USART3����len���ֽں���
* ��ڲ�����-buf:Ҫ���͵�����,lenҪ���͵����ݳ���
* ���ڲ�������
*************************************************************************************/
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 i;
  for(i=0;i<len;i++)		//ѭ����������
	{
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���		  
		USART2->DR=buf[i];
	}	 
}

