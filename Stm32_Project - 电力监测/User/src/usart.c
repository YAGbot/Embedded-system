#include "usart.h"

char Rx1Buf[50];
u8 Rx1Count;

/************************************************************************************
* 函数名称：USART1_Init
* 功能描述：USART1初始化函数
* 入口参数：-baud:波特率
*           -pclk1:APB2总线时钟频率,单位MHZ
* 出口参数：无
*************************************************************************************/
void USART1_Init(u32 pclk1, u32 baud)
{
	float USARTDIV;
	u16 DIV_Mantissa;		//整数部分
	u16 DIV_Fraction;		//小数部分
	
	pclk1 = pclk1 * 1000000;		//单位转换为Hz
	
	/*USART1的Tx和Rx端口配置*/
	RCC->APB2ENR |= 1<<2;		 										//初始化IO口时钟
	GPIOA->CRH &= ~(0XF<<(4*1));								//Tx(PA9)管脚为推挽复用输出,50MHZ
	GPIOA->CRH |= (0XB<<(4*1));
	GPIOA->CRH &= ~(0xF<<(4*2));    						//Rx(PA10)管脚为上拉输入
	GPIOA->CRH |= (0x8<<(4*2));				
	GPIOA->ODR |= 1<<10;
	
	RCC->APB2ENR |= 1<<14;											//使能USART1时钟
	RCC->APB2RSTR |= 1<<14;											//复位USART1,需要复位才能稳定工作
	RCC->APB2RSTR &= ~(1<<14);									//停止复位
	
	/*USART1参数配置*/
	USART1->CR1 |= 1<<13;												//UE位置1使能USART
	USART1->CR1 &= ~(1<<12);										//数据长度为8
	USART1->CR2 &= ~((1<<12)|(1<<13));					//1位停止位
	
	USARTDIV = (float)pclk1/baud/16;						//由baud=fck/(16*USARTDIV)计算
	DIV_Mantissa = USARTDIV;										//得到整数部分
	DIV_Fraction = (USARTDIV-DIV_Mantissa)*16;	//得到小数部分
	USART1->BRR = (DIV_Mantissa<<4) + DIV_Fraction;	//得到BRR的值
	
	//NVIC->IP[37]=0x40;								//抢占优先级为0,响应优先级为2
//	NVIC_SetPriority(USART1_IRQn,NVIC_EncodePriority(7-2,2,2));
//	//NVIC->ISER[1]|=(1<<5);  						//USART1中断通道使能  
//	NVIC_EnableIRQ(USART1_IRQn);
//	USART1->CR1|=(1<<5);							//使能接收中断 
	
	USART1->CR1 |= 1<<3;												//发送使能
	//USART1->CR1 |= 1<<2;												//接收使能
}

/************************************************************************************
* 函数名称：USART1_SendByte
* 功能描述：USART1发送一个字符函数
* 入口参数：-dat:要发送的数据
* 出口参数：无
*************************************************************************************/
void USART1_SendByte(u8 dat)
{
	while(!(USART1->SR&(1<<6)));			//等待移位寄存器为空
	USART1->DR = dat;
}

/************************************************************************************
* 函数名称：USART1_SendString
* 功能描述：USART1发送一个字符串函数
* 入口参数：-s:字符串的首地址
* 出口参数：无
*************************************************************************************/
void USART1_SendString(char *s)
{
	while(*s!='\0')
	{
		USART1_SendByte(*s);
		s++;
	}
}

/************************************************************************************
* 函数名称：USART1_IRQHandler
* 功能描述：USART1接收中断函数
* 入口参数：无
* 出口参数：无
* 说 	明:	使用全局变量RxBuf传递数据
*************************************************************************************/
void USART1_IRQHandler(void)
{
	if(USART1->SR&(1<<5))					//接收到数据
	{
		Rx1Buf[Rx1Count] = USART1->DR;
		Rx1Count++;
		if(Rx1Buf[Rx1Count-1]=='\n')				//检测到换行（调试时要发送新行）
		{
			Rx1Buf[Rx1Count-1] = '\0';
			Rx1Buf[Rx1Count-2] = '\0';
			Rx1Count = 0;
		}
	}
}

/************************************************************************************
* 函数名称：USART1_Send_Data
* 功能描述：USART1发送len个字节函数
* 入口参数：-buf:要发送的数据,len要发送的数据长度
* 出口参数：无
*************************************************************************************/
void USART1_Send_Data(u8 *buf,u8 len)
{
	u8 i;
  for(i=0;i<len;i++)		//循环发送数据
	{
		while((USART1->SR&0X40)==0);//等待发送结束		  
		USART1->DR=buf[i];
	}	  
}

/************************************************************************************
* 功能：加入复制粘贴以下代码,就可以支持printf函数,不需要选择use MicroLIB也可以	  
*************************************************************************************/
#ifndef MicroLIB
#pragma import(__use_no_semihosting)              //没有实现fgetc时需要声明该参数          
              
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
   
_sys_exit(int x) 	                              //定义_sys_exit避免使用半主机模式 
{ 
	x = x; 
} 
 
int fputc(int ch, FILE *f)		                  //重写fputc函数       
{
	USART1->DR=ch;
	while(!(USART1->SR&(1<<7)));
	return ch;	
}

int ferror(FILE *f) 
{   
    return EOF;  
} 

#endif 
