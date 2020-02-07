#include "rs485.h"
#include "sys.h"
#include "delay.h"
#include "modbus_timer.h"
#include "string.h"

u8 RS485_RX_BUF[USART2_REC_LEN];				 //接收缓冲,最大USART_REC_LEN个字节.
u8 RS485_TX_BUF[USART2_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
u8 RS485_TX_CNT=0;
u16 USART2_RX_STA=0;   
/************************************************************************************
* 函数名称：RS485_Init
* 功能描述：USART2初始化函数(PA2.PA3)
* 入口参数：-baud:波特率
*           -pclk2:APB2总线时钟频率,单位MHZ
* 出口参数：无
*************************************************************************************/
void RS485_Init(u32 pclk2,u32 baud)
{
	float USARTDIV;
	u16 DIV_Mantissa;		//整数部分
	u16 DIV_Fraction;		//小数部分
	
	pclk2 = pclk2 * 1000000;		//单位转换为Hz
	
	/*USART2的Tx和Rx端口配置*/
	RCC->APB2ENR |= 1<<2;		 										//初始化PA口时钟
	GPIOA->CRL &= ~(0XF<<(4*2));								//Tx(PA2)管脚为推挽复用输出,50MHZ
	GPIOA->CRL |= (0XB<<(4*2));
	GPIOA->CRL &= ~(0xF<<(4*3));    						//Rx(PA3)管脚为浮空输入
	GPIOA->CRL |= (0x4<<(4*3));
	//GPIOA->ODR |= 1<<3;
	
	RCC->APB1ENR |= 1<<17;											//使能USART2时钟
	RCC->APB1RSTR |= 1<<17;											//复位USART2,需要复位才能稳定工作
	RCC->APB1RSTR &= ~(1<<17);									//停止复位
	
	/*USART2参数配置*/
	USART2->CR1 |= 1<<13;												//UE位置1使能USART
	USART2->CR1 &= ~(1<<12);										//数据长度为8
	USART2->CR2 &= ~((1<<12)|(1<<13));					//1位停止位
	
	USARTDIV = (float)pclk2/baud/16;						//由baud=fck/(16*USARTDIV)计算
	DIV_Mantissa = USARTDIV;										//得到整数部分
	DIV_Fraction = (USARTDIV-DIV_Mantissa)*16;	//得到小数部分
	USART2->BRR = (DIV_Mantissa<<4) + DIV_Fraction;	//得到BRR的值
	
	NVIC_SetPriority(USART2_IRQn,NVIC_EncodePriority(7-2,2,2));
							//USART2中断通道使能  
	NVIC_EnableIRQ(USART2_IRQn);
	USART2->CR1|=(1<<5);							//使能接收中断 
	
	USART2->CR1 |= 1<<3;												//发送使能
	USART2->CR1 |= 1<<2;												//接收使能
	
	TIME7_Init(99,7199);		//10ms中断
	USART2_RX_STA=0;		//清零
	time7_stop();			//关闭定时器
}


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
	
void USART2_IRQHandler(void)
{
	u8 res;	    
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		res=USART2->DR; 			 
		if(USART2_RX_STA<USART2_REC_LEN)		//还可以接收数据
		{
			TIM7->CNT=0;         					//计数器清空
			if(USART2_RX_STA==0) time7_start();	 	//使能定时器4的中断 
			RS485_RX_BUF[USART2_RX_STA++]=res;		//记录接收到的值	 
		}else 
		{
			USART2_RX_STA|=1<<15;					//强制标记接收完成
		} 
	}  											 	 
}   


/************************************************************************************
* 函数名称：USART2_SendByte
* 功能描述：USART3发送一个字符函数
* 入口参数：-dat:要发送的数据
* 出口参数：无
*************************************************************************************/
void USART2_SendByte(u8 dat)
{
	while(!(USART2->SR&(1<<6)));			//等待移位寄存器为空
	USART2->DR = dat;
}

/************************************************************************************
* 函数名称：RS485_Send_Data
* 功能描述：USART3发送len个字节函数
* 入口参数：-buf:要发送的数据,len要发送的数据长度
* 出口参数：无
*************************************************************************************/
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 i;
  for(i=0;i<len;i++)		//循环发送数据
	{
		while((USART2->SR&0X40)==0);//等待发送结束		  
		USART2->DR=buf[i];
	}	 
}

