#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "4G_usart.h"

//串口发送缓存区 	
u8 UART5_TX_BUF[UART5_MAX_SEND_LEN]; 	//发送缓冲,最大UART5_MAX_SEND_LEN字节
u8 UART5_RX_REC_ATCOMMAD;	  
//串口接收缓存区 	 			
u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 				//接收缓冲,最大UART5_MAX_RECV_LEN个字节.//接收缓冲,最大UART5_MAX_RECV_LEN个字节.
u16 UART5_RX_STA=0; 
 /**
  * @brief  初始化gprs usart1	PA9.PA10 
  * @param  无
  * @retval 无
  */
void gprs_usart_init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//使能UART5，GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//使能GPIOC/D时钟
	
  USART_DeInit(UART5);  //复位串口1

		//USART5_TX   PB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PB10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化Pb10
  //UART5_RX	  PB11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOD, &GPIO_InitStructure);  //初始化PB11
	
  //Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(UART5, &USART_InitStructure); //初始化串口1
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART5, ENABLE);                    //使能串口1 
}

/************************************************************************************
* 函数名称：UART5_IRQHandler 
* 功能描述：UART5接收中断函数
* 入口参数：无
* 出口参数：无
* 说 	明:	使用全局变量gu900_uart_buff传递数据
*************************************************************************************/
void UART5_IRQHandler(void)
{	    
	u8 res;
	if(UART5->SR&(1<<5))//接收到数据
	{	 
		res=UART5->DR;
		if(UART5_RX_STA<UART5_MAX_RECV_LEN)		//还可以接收数据
		{
		  time7_reset();		//复位定时器7的cnt 
			if(UART5_RX_STA==0)time7_start();		//开启定时器7
			UART5_RX_BUF[UART5_RX_STA++]=res;		//记录接收到的值	
		}
		else 
 		{
 			UART5_RX_STA|=1<<15;					//强制标记接收完成
 		} 
	} 
} 

/************************************************************************************
* 函数名称：UART5_SendDat
* 功能描述：UART5发送一个字符函数
* 入口参数：-dat:要发送的数据
* 出口参数：无
*************************************************************************************/
void UART5_SendDat(u8 dat)
{
	while(!(UART5->SR&(1<<6)));			//等待移位寄存器为空
	UART5->DR = dat;
}

/************************************************************************************
* 函数名称：UART5_SendString
* 功能描述：UART5发送字符串函数
* 入口参数：-str:要发送的数据
* 出口参数：无
*************************************************************************************/
void UART5_SendString(char *str)
{
	while(*str != '\0')
	{
		UART5_SendDat(*str);
		str++;
	}
	
}


//实现fputc函数
void gsm_fputc(char str)
{
	UART5_SendDat(str);
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
					UART5_SendDat('\r');
					str++;
					break;
				case 'n':
					UART5_SendDat('\r');
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
				case 'd':
					int_d = va_arg(ap, int);
					sprintf(buff,"%d",int_d);
					for(i=0;i<(strlen(buff));i++)
						gsm_fputc(buff[i]);					
					str++;
					break;
				case 'f':
					float_d = va_arg(ap, double);
					sprintf(buff,"%f",float_d);
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


/************************************************** *********************************
* 函数名称：GSM_Init
* 功能描述：gsm初始化函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void GSM_Init(void)
{
	gprs_usart_init(115200);
	TIM7_Init();
}

/************************************************************************************
* 函数名称：check_buff
* 功能描述：缓存内容检查函数
* 入口参数：buff：
* 出口参数：0：成功 ；-1：非子串
*************************************************************************************/
int check_buff(u8 *buff, char *str)
{
    //字符串查找
  if(strstr((const char *)buff,str)!=NULL)
    return 0;
  else
    return -1;
}

/************************************************************************************
* 函数名称：gprs_check_cmd
* 功能描述：字符串检索函数
* 入口参数：
* 出口参数：
*************************************************************************************/
u8* gprs_check_cmd(u8 *str)
{
	char *strx=0;
	if(UART5_RX_STA&0X8000)		//接收到一次数据了
	{ 
		UART5_RX_BUF[UART5_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)UART5_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//向gprs发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	UART5_RX_STA=0;UART5_RX_REC_ATCOMMAD=1;
	if((u32)cmd<=0XFF)
	{  
		UART5->DR=(u32)cmd;
	}else gsm_printf("%s\r\n",cmd);//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(11);			//这里延时如果不加进不去中断？
			if(UART5_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(gprs_check_cmd(ack))break;//得到有效数据 
				UART5_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	UART5_RX_STA=0;UART5_RX_REC_ATCOMMAD=0;
	return res;
} 


/************************************************************************************
* 函数名称：TIME7_Init
* 功能描述：TIME7初始化以及定时1s函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/	 
void TIM7_Init(void)
{	
	RCC->APB1ENR |= (1<<5);	//开TIME7外设时钟
  //配置定时器
  TIM7->CR1 |= (1<<7);	//使能影子寄存器
  TIM7->CR1 |= (1<<3);	//计数溢出计数器停止
  TIM7->CR1 |= (1<<2);	//只能计数溢出发生更新中断
  TIM7->CR1 &= ~(1<<1);	//使能更新
  NVIC_SetPriority(TIM7_IRQn,NVIC_EncodePriority(7-2,2,2));//组2抢占优先级2，响应优先级0
  NVIC_EnableIRQ(TIM7_IRQn);		
	TIM7->DIER |= (1<<0);//使能更新中断
  TIM7->PSC = 7200-1; 	//配置预分频
  TIM7->ARR = 100-1;  //配置自动重装载 
  TIM7->EGR |= (1<<0); //软件产生更新事件
}


/************************************************************************************
* 函数名称：time7_start
* 功能描述：定时器开始函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time7_start(void)
{
    //使能
    TIM7->CNT = 0;
    TIM7->CR1 |= (1<<0);    
}

/************************************************************************************
* 函数名称：time7_stop
* 功能描述：定时器停止函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time7_stop(void)
{
    TIM7->CR1 &= ~(1<<0);    
}

/************************************************************************************
* 函数名称：time7_set_timeout
* 功能描述：设置定时器时间函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time7_set_timeout(u16 time)
{
    TIM7->ARR = 10 * time;   //转成1ms的单位
}

/************************************************************************************
* 函数名称：time7_reset
* 功能描述：定时器复位倒计时函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time7_reset(void)
{
    TIM7->CNT = 0;
}

/************************************************************************************
* 函数名称：TIM7_IRQHandler
* 功能描述：定时器中断函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void TIM7_IRQHandler(void)
{
	if(TIM7->SR&0X01)				//是更新中断
	{
		TIM7->SR &= ~(1<<0);//清除标志位
		UART5_RX_STA|=1<<15;	//标记接收完成 0x8000
		time7_stop();	//关闭定时器
		USART1_Send_Data(UART5_RX_BUF,UART5_RX_STA&0x7fff);	//输出LOG信息
//		if(!UART5_RX_REC_ATCOMMAD)	
//		{
//			UART5_RX_BUF[UART5_RX_STA&0X7FFF]=0;//添加结束符
//			UART5_RX_STA=0;	
//			if(check_buff(UART5_RX_BUF,"+CMQPUB:")==0)
//			{
//				if(check_buff(UART5_RX_BUF,g_MQTT_Msg.ID)==0)
//				{
//					if(check_buff(UART5_RX_BUF,"&&01")==0)
//					{
//						Flag_Rec_MQTT=1;
//					}
//					if(check_buff(UART5_RX_BUF,"&&02")==0)
//					{
//						rec_ip_config=1;
//					}
//				}	
//			}
//		}
	}
}


