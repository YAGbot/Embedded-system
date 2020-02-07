#include "diwen.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"	 
#include "delay.h"
#include "timer.h"
#include "usart.h"

/** MY-5010�ͺ������������� **/

float my5010[3][13]={{0.00,15.00,20.00,25.00,30.00,35.00,40.00,45.00,50.00},	//����1-13
												{3.00,3.00,3.00,3.00,2.88,2.38,2.01,1.72,1.50},							//a=2��Ӧ����
												{6.00,6.00,4.51,3.46,2.77,2.28,1.92,1.64,1.42}};		//MY-5015�ͺ�������������
const char* my5010_display1="0.00  15.00 20.00 25.00 30.00 35.00 40.00 45.00 50.00                         ";	//��һ����ʾ
const char* my5010_display2="3.00  3.00  3.00  3.00  2.88  2.38  2.01  1.72  1.50                          ";	//�ڶ�����ʾ												
const char* my5010_display3="6.00  6.00  4.51  3.46  2.77  2.28  1.92  1.64  1.42                          ";	//��������ʾ
												
u8 USART4_RX_BUF[USART4_REC_LEN];				 //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART4_TX_BUF[USART4_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
u8 USART4_TX_CNT=0;
//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART4_RX_STA=0;   	 
void UART4_IRQHandler(void)
{
	u8 res;	    
	if(UART4->SR&(1<<5))//���յ�����
	{	 
		res=UART4->DR; 			 
		if(USART4_RX_STA<USART4_REC_LEN)		//�����Խ�������
		{
			TIM4->CNT=0;         					//���������
			if(USART4_RX_STA==0) time4_start();	 	//ʹ�ܶ�ʱ��4���ж� 
			USART4_RX_BUF[USART4_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART4_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	}  											 
}   

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
	
	TIM4_Init(99,7199);		//10ms�ж�
	USART4_RX_STA=0;		//����
	time4_stop();			//�رն�ʱ��
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


/************************************************************************************
* �������ƣ�diwen_send_data16
* ��������������������ʵʱ����		������
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_data16(uint16_t _reg,int16_t _data)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//����ͷ
	USART4_TX_BUF[USART4_TX_CNT++] = 0x05;	//���͵����ݳ���
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//дָ��
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 8;	//д������
	USART4_TX_BUF[USART4_TX_CNT++] = _data;	//д������
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
	
	//USART1_Send_Data(USART4_TX_BUF,USART4_TX_CNT);
}

/************************************************************************************
* �������ƣ�diwen_send_data
* ����������������������ĸ����ASCII��
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_numascii(u16 _reg,u8 *buf,u8 len)
{
	u8 i;
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//����ͷ
	USART4_TX_BUF[USART4_TX_CNT++] = len+3;	//���͵����ݳ���
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//дָ��
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	for(i=0;i<len;i++)
	{
		USART4_TX_BUF[USART4_TX_CNT++]=buf[i];
	}
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
	//USART1_Send_Data(USART4_TX_BUF,USART4_TX_CNT);
}

/************************************************************************************
* �������ƣ�diwen_send_rectangle
* ���������������������γ���
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_rectangle(u16 _reg,u16 x1,u16 y1,u16 x2,u16 y2)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//����ͷ
	USART4_TX_BUF[USART4_TX_CNT++] = 0x11;	//���͵����ݳ���
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//дָ��
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x04;	//���������ָ��
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;	//���һ������
	USART4_TX_BUF[USART4_TX_CNT++] = x1>>8;		//x���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = x1;		//x���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = y1>>8;		//y���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = y1;		//y���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = x2>>8;		//x���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = x2;		//x���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = y2>>8;		//y���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = y2;		//y���� ���ֽ�
	USART4_TX_BUF[USART4_TX_CNT++] = 0x04;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x1F;	//�����ɫ
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
	//USART1_Send_Data(USART4_TX_BUF,USART4_TX_CNT);
}

/************************************************************************************
* �������ƣ�liebiao_view
* �����������������б���ʾ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void liebiao_view(void)
{
	diwen_send_numascii(0x1100,(u8*)my5010_display1,strlen(my5010_display1));
	delay_ms(100);
	diwen_send_numascii(0x1127,(u8*)my5010_display2,strlen(my5010_display2));
	delay_ms(100);
	diwen_send_numascii(0x114e,(u8*)my5010_display3,strlen(my5010_display3));
}

