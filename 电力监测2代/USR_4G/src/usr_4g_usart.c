#include "usr_4g_timer.h"
#include "usr_4g_usart.h"
#include "usr_4g.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "uart.h"


//串口发送缓存区 	
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
	  
//串口接收缓存区 	 			
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u16 USART1_RX_STA=0;  
 /**
  * @brief  初始化gprs usart1	PA9.PA10 
  * @param  无
  * @retval 无
  */
void sim7020_usart_init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
}

/************************************************************************************
* 函数名称：USART1_IRQHandler 
* 功能描述：USART1接收中断函数
* 入口参数：无
* 出口参数：无
* 说 	明:	使用全局变量gu900_uart_buff传递数据
*************************************************************************************/
void USART1_IRQHandler(void)
{	    
	u8 res;
	if(USART1->SR&(1<<5))//接收到数据
	{	 
		res=USART1->DR;
		if(USART1_RX_STA<USART1_MAX_RECV_LEN)		//还可以接收数据
		{
		  time7_reset();		//复位定时器7的cnt 
			if(USART1_RX_STA==0)time7_start();		//开启定时器7
			USART1_RX_BUF[USART1_RX_STA++]=res;		//记录接收到的值	
		}
		else 
 		{
 			USART1_RX_STA|=1<<15;					//强制标记接收完成
 		} 
	} 
} 

/************************************************************************************
* 函数名称：USART1_SendDat
* 功能描述：USART1发送一个字符函数
* 入口参数：-dat:要发送的数据
* 出口参数：无
*************************************************************************************/
void USART1_SendDat(u8 dat)
{
	while(!(USART1->SR&(1<<6)));			//等待移位寄存器为空
	USART1->DR = dat;
}

/************************************************************************************
* 函数名称：USART1_SendString
* 功能描述：USART1发送字符串函数
* 入口参数：-str:要发送的数据
* 出口参数：无
*************************************************************************************/
void USART1_SendString(char *str)
{
	while(*str != '\0')
	{
		USART1_SendDat(*str);
		str++;
	}
	
}


//实现fputc函数
void gsm_fputc(char str)
{
	USART1_SendDat(str);
}

//实现printf函数
void gsm_printf(char *str,...)
{
	int int_d;
	double float_d;
	const char *string_s;
	char buff[20];
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

