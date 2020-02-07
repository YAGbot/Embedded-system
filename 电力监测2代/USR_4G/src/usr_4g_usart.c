#include "usr_4g_timer.h"
#include "usr_4g_usart.h"
#include "usr_4g.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "uart.h"


//���ڷ��ͻ����� 	
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
	  
//���ڽ��ջ����� 	 			
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u16 USART1_RX_STA=0;  
 /**
  * @brief  ��ʼ��gprs usart1	PA9.PA10 
  * @param  ��
  * @retval ��
  */
void sim7020_usart_init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
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
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}

/************************************************************************************
* �������ƣ�USART1_IRQHandler 
* ����������USART1�����жϺ���
* ��ڲ�������
* ���ڲ�������
* ˵ 	��:	ʹ��ȫ�ֱ���gu900_uart_buff��������
*************************************************************************************/
void USART1_IRQHandler(void)
{	    
	u8 res;
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		res=USART1->DR;
		if(USART1_RX_STA<USART1_MAX_RECV_LEN)		//�����Խ�������
		{
		  time7_reset();		//��λ��ʱ��7��cnt 
			if(USART1_RX_STA==0)time7_start();		//������ʱ��7
			USART1_RX_BUF[USART1_RX_STA++]=res;		//��¼���յ���ֵ	
		}
		else 
 		{
 			USART1_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
 		} 
	} 
} 

/************************************************************************************
* �������ƣ�USART1_SendDat
* ����������USART1����һ���ַ�����
* ��ڲ�����-dat:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void USART1_SendDat(u8 dat)
{
	while(!(USART1->SR&(1<<6)));			//�ȴ���λ�Ĵ���Ϊ��
	USART1->DR = dat;
}

/************************************************************************************
* �������ƣ�USART1_SendString
* ����������USART1�����ַ�������
* ��ڲ�����-str:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void USART1_SendString(char *str)
{
	while(*str != '\0')
	{
		USART1_SendDat(*str);
		str++;
	}
	
}


//ʵ��fputc����
void gsm_fputc(char str)
{
	USART1_SendDat(str);
}

//ʵ��printf����
void gsm_printf(char *str,...)
{
	int int_d;
	double float_d;
	const char *string_s;
	char buff[20];
	int i;
	
	va_list ap;//�����ε���ʼָ��
	va_start(ap, str);//��ʼָ��ָ��str
	
	while(*str!='\0')
	{
		if(*str=='\\')
		{
			str++;
			switch(*str)
			{
				case 'r':
					USART1_SendDat('\r');
					str++;
					break;
				case 'n':
					USART1_SendDat('\r');
					str++;
					break;
				default:
					str++;
					break;
			}			
		}
		else if(*str=='%')
		{
			str++;
			switch(*str)
			{
				case '4':
					int_d = va_arg(ap, int);
					sprintf(buff,"%04d",int_d);
					for(i=0;i<(strlen(buff));i++)
						gsm_fputc(buff[i]);					
					str++;
					break;
				case 'd':
					int_d = va_arg(ap, int);
					sprintf(buff,"%d",int_d);
					for(i=0;i<(strlen(buff));i++)
						gsm_fputc(buff[i]);					
					str++;
					break;
				case 'f':
					float_d = va_arg(ap, double);
					sprintf(buff,"%.1f",float_d);
					for(i=0;i<(strlen(buff));i++)
						gsm_fputc(buff[i]);					
					str++;
					break;				
				case 's':
					string_s = va_arg(ap, const char *);
					while(*string_s)
					{
						gsm_fputc(*string_s++);
					}
					str++;
					break;
				default:
					str++;
					break;
			}
		}
		else
		{
			gsm_fputc(*str++);
		}
	}
}

