#include "usart4.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"	 
#include "delay.h"
#include "timer.h"

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((UART4->SR&0X40)==0);//循环发送,直到发送完毕   
    UART4->DR = (u8) ch;      
	return ch;
}
#endif 

u8 USART4_RX_BUF[USART4_REC_LEN];				 //接收缓冲,最大USART_REC_LEN个字节.
u8 USART4_TX_BUF[USART4_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
u8 USART4_TX_CNT=0;
//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
//u16 USART4_RX_STA=0;   	 
//void UART4_IRQHandler(void)
//{
//	u8 res;	    
//	if(UART4->SR&(1<<5))//接收到数据
//	{	 
//		res=UART4->DR; 			 
//		if(USART4_RX_STA<USART4_REC_LEN)		//还可以接收数据
//		{
//			TIM4->CNT=0;         					//计数器清空
//			if(USART4_RX_STA==0) time4_start();	 	//使能定时器4的中断 
//			USART4_RX_BUF[USART4_RX_STA++]=res;		//记录接收到的值	 
//		}else 
//		{
//			USART4_RX_STA|=1<<15;					//强制标记接收完成
//		} 
//	}  											 
//}   

/* 串口4 TX = PC10   RX = PC11 */
void UART4_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 第1步： 开启GPIO和UART时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
	*/
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 第4步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = bound;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	
	USART_Init(UART4, &USART_InitStructure);

	 /* 第5步： NVIC 配置 */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	USART_Cmd(UART4, ENABLE);		/* 使能串口 */
}


/************************************************************************************
* 函数名称：UART4_SendByte
* 功能描述：UART4发送一个字符函数
* 入口参数：-dat:要发送的数据
* 出口参数：无
*************************************************************************************/
void USART4_SendByte(u8 dat)
{
	while(!(UART4->SR&(1<<6)));			//等待移位寄存器为空
	UART4->DR = dat;
}

/************************************************************************************
* 函数名称：UART_SendDat
* 功能描述：UART4发送len个字符函数
* 入口参数：-dat:要发送的数据 len:要发送数据的长度
* 出口参数：无
*************************************************************************************/
void USART4_SendDat(u8 *buf,u8 len)
{
	u8 i;
  for(i=0;i<len;i++)		//循环发送数据
	{
		while((UART4->SR&0X40)==0);//等待发送结束		  
		UART4->DR=buf[i];
	}	 
}



