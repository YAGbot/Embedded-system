#include "gu900_uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

u8 gu900_uart_buff[250];				//gu900����buff
u16 gu900_uart_len;							//����������
/************************************************************************************
* �������ƣ�USART3_Init
* ����������USART3��ʼ������(PB10.PB11)
* ��ڲ�����-baud:������
*           -pclk1:APB2����ʱ��Ƶ��,��λMHZ
* ���ڲ�������
*************************************************************************************/
void USART3_Init(u32 pclk1,u32 baud)
{
	float USARTDIV;
	u16 DIV_Mantissa;		//��������
	u16 DIV_Fraction;		//С������
	
	pclk1 = pclk1 * 1000000;		//��λת��ΪHz
	
	/*USART3��Tx��Rx�˿�����*/
	RCC->APB2ENR |= 1<<3;		 										//��ʼ��PB��ʱ��
	GPIOB->CRH &= ~(0XF<<(4*2));								//Tx(PB10)�ܽ�Ϊ���츴�����,50MHZ
	GPIOB->CRH |= (0XB<<(4*2));
	GPIOB->CRH &= ~(0xF<<(4*3));    						//Rx(PB11)�ܽ�Ϊ��������
	GPIOB->CRH |= (0x8<<(4*3));
	GPIOB->ODR |= 1<<11;
	
	RCC->APB1ENR |= 1<<18;											//ʹ��USART3ʱ��
	RCC->APB1RSTR |= 1<<18;											//��λUSART3,��Ҫ��λ�����ȶ�����
	RCC->APB1RSTR &= ~(1<<18);									//ֹͣ��λ
	
	/*USART3��������*/
	USART3->CR1 |= 1<<13;												//UEλ��1ʹ��USART
	USART3->CR1 &= ~(1<<12);										//���ݳ���Ϊ8
	USART3->CR2 &= ~((1<<12)|(1<<13));					//1λֹͣλ
	
	USARTDIV = (float)pclk1/baud/16;						//��baud=fck/(16*USARTDIV)����
	DIV_Mantissa = USARTDIV;										//�õ���������
	DIV_Fraction = (USARTDIV-DIV_Mantissa)*16;	//�õ�С������
	USART3->BRR = (DIV_Mantissa<<4) + DIV_Fraction;	//�õ�BRR��ֵ
	
	//NVIC_SetPriority(USART3_IRQn,NVIC_EncodePriority(7-2,2,2));//��2��ռ���ȼ�1����Ӧ���ȼ�2
							//USART3�ж�ͨ��ʹ��  
	//NVIC_EnableIRQ(USART3_IRQn);
	//USART3->CR1|=1<<8;    	//PE�ж�ʹ��
	//USART3->CR1|=(1<<5);							//ʹ�ܽ����ж� 
	
	USART3->CR1 |= 1<<3;												//����ʹ��
	//USART3->CR1 |= 1<<2;												//����ʹ��
}


/************************************************************************************
* �������ƣ�USART3_SendDat
* ����������USART3����һ���ַ�����
* ��ڲ�����-dat:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void USART3_SendDat(u8 dat)
{
	while(!(USART3->SR&(1<<6)));			//�ȴ���λ�Ĵ���Ϊ��
	USART3->DR = dat;
}

/************************************************************************************
* �������ƣ�USART3_SendString
* ����������USART3�����ַ�������
* ��ڲ�����-str:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void USART3_SendString(char *str)
{
	while(*str != '\0')
	{
		USART3_SendDat(*str);
		str++;
	}
}

//void USART3_IRQHandler(void)
//{   
//	u8 res;
//	if(USART3->SR&(1<<5))//���յ�����
//	{	
//		res=USART3->DR; 			 
//		if(gu900_uart_len<250)
//		{
//			if(res != '\r' && res != '\n')
//			{
//				gu900_uart_buff[gu900_uart_len++]=res; //���յ������ݱ��浽buff��
//			}
//			if(res == '\n')   //�������
//			{
//				gu900_uart_buff[gu900_uart_len]='\0';  //������
//				gu900_uart_len=0;
//			}
//		}
//	}
//} 

//ʵ��fputc����
void gu900_fputc(char str)
{
	USART3_SendDat(str);
}

/************************************************************************************
* ���ܣ�ʵ�ּ��׵�printf����	  
*************************************************************************************/
void gu900_printf(char *str,...)
{
	int int_d;
	double float_d;
	const char *string_s;
	char buff[100];
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
					USART3_SendDat('\r');
					str++;
					break;
				case 'n':
					USART3_SendDat('\r');
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
						gu900_fputc(buff[i]);					
					str++;
					break;
				case 'd':
					int_d = va_arg(ap, int);
					sprintf(buff,"%d",int_d);
					for(i=0;i<(strlen(buff));i++)
						gu900_fputc(buff[i]);					
					str++;
					break;
				case 'f':
					float_d = va_arg(ap, double);
					sprintf(buff,"%.1f",float_d);
					for(i=0;i<(strlen(buff));i++)
						gu900_fputc(buff[i]);					
					str++;
					break;				
				case 's':
					string_s = va_arg(ap, const char *);
					while(*string_s)
					{
						gu900_fputc(*string_s++);
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
			gu900_fputc(*str++);
		}
	}
}


