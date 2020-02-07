#include "rs485.h"
#include "sys.h"
#include "delay.h"
#include "modbus_timer.h"
#include "string.h"

u8 rs485_recv_flag = 0; //gu900接收数据状态位

u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
u8 RS485_RX_CNT=0;			//接收到的数据长度

u8 RS485_TX_BUF[64];  	//发送缓冲
u8 RS485_TX_CNT;				//发送的数据长度
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
	
	NVIC_SetPriority(USART2_IRQn,NVIC_EncodePriority(7-2,1,2));
							//USART2中断通道使能  
	NVIC_EnableIRQ(USART2_IRQn);
	USART2->CR1|=(1<<5);							//使能接收中断 
	
	USART2->CR1 |= 1<<3;												//发送使能
	USART2->CR1 |= 1<<2;												//接收使能
}

///************************************************************************************
//* 函数名称：USART2_IRQHandler
//* 功能描述：USART2接收中断函数
//* 入口参数：无
//* 出口参数：无
//* 说 	明:	使用全局变量RS485_RX_BUF传递数据
//*************************************************************************************/
//void USART2_IRQHandler(void)
//{   
//	u8 res;
//	if(USART2->SR&(1<<5))//接收到数据
//	{	
//		res=USART2->DR; 			 
//		if(RS485_RX_CNT<64)
//		{
//			time7_start();
//			RS485_RX_BUF[RS485_RX_CNT]=res;		//记录接收到的值
//			RS485_RX_CNT++;						//接收数据增加1 
//		}
//		
//	}
//} 

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
		if(RS485_RX_CNT<64)
		{
			time7_reset();	//计数器复位
			if(rs485_recv_flag==0) time7_start();	//定时器开始
			RS485_RX_BUF[RS485_RX_CNT++]=res;		//记录接收到的值
		}
		else 
		{
			rs485_recv_flag=1;		//强制标志接收完成
		}
	}  											 
}   

void RS485_Receive_Data(u8 *buf)
{
	u8 i=0;
	for(i=0;i<RS485_RX_CNT;i++)
	{
		buf[i]=RS485_RX_BUF[i];	
	}		
}

///************************************************************************************
//* 函数名称：RS485_Receive_Data
//* 功能描述：RS485查询接收到的数据
//* 入口参数：buf:接收缓存首地址
//					 len:读到的数据长度
//* 出口参数：无
//*************************************************************************************/
//void RS485_Receive_Data(u8 *buf)
//{
//	u8 rxlen=RS485_RX_CNT;
//	u8 i=0;
//	//len=0;				//默认为0
//	//delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
//	if(rxlen==RS485_RX_CNT&&rxlen)//接收到了数据,且接收完成了
//	{
//		for(i=0;i<rxlen;i++)
//		{
//			buf[i]=RS485_RX_BUF[i];	
//		}		
//		//len=RS485_RX_CNT;	//记录本次数据长度
//		RS485_RX_CNT=0;		//清零
//	}
//}

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

