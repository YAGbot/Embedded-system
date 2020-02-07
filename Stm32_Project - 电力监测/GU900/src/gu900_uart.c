#include "gu900_uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

u8 gu900_uart_buff[250];				//gu900缓冲buff
u16 gu900_uart_len;							//缓冲区长度
/************************************************************************************
* 函数名称：USART3_Init
* 功能描述：USART3初始化函数(PB10.PB11)
* 入口参数：-baud:波特率
*           -pclk1:APB2总线时钟频率,单位MHZ
* 出口参数：无
*************************************************************************************/
void USART3_Init(u32 pclk1,u32 baud)
{
	float USARTDIV;
	u16 DIV_Mantissa;		//整数部分
	u16 DIV_Fraction;		//小数部分
	
	pclk1 = pclk1 * 1000000;		//单位转换为Hz
	
	/*USART3的Tx和Rx端口配置*/
	RCC->APB2ENR |= 1<<3;		 										//初始化PB口时钟
	GPIOB->CRH &= ~(0XF<<(4*2));								//Tx(PB10)管脚为推挽复用输出,50MHZ
	GPIOB->CRH |= (0XB<<(4*2));
	GPIOB->CRH &= ~(0xF<<(4*3));    						//Rx(PB11)管脚为上拉输入
	GPIOB->CRH |= (0x8<<(4*3));
	GPIOB->ODR |= 1<<11;
	
	RCC->APB1ENR |= 1<<18;											//使能USART3时钟
	RCC->APB1RSTR |= 1<<18;											//复位USART3,需要复位才能稳定工作
	RCC->APB1RSTR &= ~(1<<18);									//停止复位
	
	/*USART3参数配置*/
	USART3->CR1 |= 1<<13;												//UE位置1使能USART
	USART3->CR1 &= ~(1<<12);										//数据长度为8
	USART3->CR2 &= ~((1<<12)|(1<<13));					//1位停止位
	
	USARTDIV = (float)pclk1/baud/16;						//由baud=fck/(16*USARTDIV)计算
	DIV_Mantissa = USARTDIV;										//得到整数部分
	DIV_Fraction = (USARTDIV-DIV_Mantissa)*16;	//得到小数部分
	USART3->BRR = (DIV_Mantissa<<4) + DIV_Fraction;	//得到BRR的值
	
	//NVIC_SetPriority(USART3_IRQn,NVIC_EncodePriority(7-2,2,2));//组2抢占优先级1，响应优先级2
							//USART3中断通道使能  
	//NVIC_EnableIRQ(USART3_IRQn);
	//USART3->CR1|=1<<8;    	//PE中断使能
	//USART3->CR1|=(1<<5);							//使能接收中断 
	
	USART3->CR1 |= 1<<3;												//发送使能
	//USART3->CR1 |= 1<<2;												//接收使能
}


/************************************************************************************
* 函数名称：USART3_SendDat
* 功能描述：USART3发送一个字符函数
* 入口参数：-dat:要发送的数据
* 出口参数：无
*************************************************************************************/
void USART3_SendDat(u8 dat)
{
	while(!(USART3->SR&(1<<6)));			//等待移位寄存器为空
	USART3->DR = dat;
}

/************************************************************************************
* 函数名称：USART3_SendString
* 功能描述：USART3发送字符串函数
* 入口参数：-str:要发送的数据
* 出口参数：无
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
//	if(USART3->SR&(1<<5))//接收到数据
//	{	
//		res=USART3->DR; 			 
//		if(gu900_uart_len<250)
//		{
//			if(res != '\r' && res != '\n')
//			{
//				gu900_uart_buff[gu900_uart_len++]=res; //把收到的数据保存到buff中
//			}
//			if(res == '\n')   //接收完成
//			{
//				gu900_uart_buff[gu900_uart_len]='\0';  //结束符
//				gu900_uart_len=0;
//			}
//		}
//	}
//} 

//实现fputc函数
void gu900_fputc(char str)
{
	USART3_SendDat(str);
}

/************************************************************************************
* 功能：实现简易的printf函数	  
*************************************************************************************/
void gu900_printf(char *str,...)
{
	int int_d;
	double float_d;
	const char *string_s;
	char buff[100];
	int i;
	
	va_list ap;//定义变参的起始指针
	va_start(ap, str);//起始指针指向str
	
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


