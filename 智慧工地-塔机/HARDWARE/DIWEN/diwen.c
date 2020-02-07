#include "diwen.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"	 
#include "delay.h"
#include "timer.h"
#include "usart.h"

/** MY-5010型号塔机力矩曲线 **/

float my5010[3][13]={{0.00,15.00,20.00,25.00,30.00,35.00,40.00,45.00,50.00},	//幅度1-13
												{3.00,3.00,3.00,3.00,2.88,2.38,2.01,1.72,1.50},							//a=2对应参数
												{6.00,6.00,4.51,3.46,2.77,2.28,1.92,1.64,1.42}};		//MY-5015型号塔机力矩曲线
const char* my5010_display1="0.00  15.00 20.00 25.00 30.00 35.00 40.00 45.00 50.00                         ";	//第一行显示
const char* my5010_display2="3.00  3.00  3.00  3.00  2.88  2.38  2.01  1.72  1.50                          ";	//第二行显示												
const char* my5010_display3="6.00  6.00  4.51  3.46  2.77  2.28  1.92  1.64  1.42                          ";	//第三行显示
												
u8 USART4_RX_BUF[USART4_REC_LEN];				 //接收缓冲,最大USART_REC_LEN个字节.
u8 USART4_TX_BUF[USART4_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
u8 USART4_TX_CNT=0;
//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART4_RX_STA=0;   	 
void UART4_IRQHandler(void)
{
	u8 res;	    
	if(UART4->SR&(1<<5))//接收到数据
	{	 
		res=UART4->DR; 			 
		if(USART4_RX_STA<USART4_REC_LEN)		//还可以接收数据
		{
			TIM4->CNT=0;         					//计数器清空
			if(USART4_RX_STA==0) time4_start();	 	//使能定时器4的中断 
			USART4_RX_BUF[USART4_RX_STA++]=res;		//记录接收到的值	 
		}else 
		{
			USART4_RX_STA|=1<<15;					//强制标记接收完成
		} 
	}  											 
}   

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
	
	TIM4_Init(99,7199);		//10ms中断
	USART4_RX_STA=0;		//清零
	time4_stop();			//关闭定时器
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


/************************************************************************************
* 函数名称：diwen_send_data16
* 功能描述：迪文屏发送实时数据		带符号
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_data16(uint16_t _reg,int16_t _data)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//数据头
	USART4_TX_BUF[USART4_TX_CNT++] = 0x05;	//发送的数据长度
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//写指令
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//寄存器编号 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = _data >> 8;	//写入数据
	USART4_TX_BUF[USART4_TX_CNT++] = _data;	//写入数据
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
	
	//USART1_Send_Data(USART4_TX_BUF,USART4_TX_CNT);
}

/************************************************************************************
* 函数名称：diwen_send_data
* 功能描述：迪文屏发送字母数字ASCII码
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_numascii(u16 _reg,u8 *buf,u8 len)
{
	u8 i;
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//数据头
	USART4_TX_BUF[USART4_TX_CNT++] = len+3;	//发送的数据长度
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//写指令
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//寄存器编号 低字节
	for(i=0;i<len;i++)
	{
		USART4_TX_BUF[USART4_TX_CNT++]=buf[i];
	}
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
	//USART1_Send_Data(USART4_TX_BUF,USART4_TX_CNT);
}

/************************************************************************************
* 函数名称：diwen_send_rectangle
* 功能描述：迪文屏画矩形程序
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_rectangle(u16 _reg,u16 x1,u16 y1,u16 x2,u16 y2)
{
	USART4_TX_CNT=0;
	USART4_TX_BUF[USART4_TX_CNT++] = 0x5A;		
	USART4_TX_BUF[USART4_TX_CNT++] = 0xA5;		//数据头
	USART4_TX_BUF[USART4_TX_CNT++] = 0x11;	//发送的数据长度
	USART4_TX_BUF[USART4_TX_CNT++] = 0x82;	//写指令
	USART4_TX_BUF[USART4_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = _reg;		//寄存器编号 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x04;	//画矩形填充指令
	USART4_TX_BUF[USART4_TX_CNT++] = 0x00;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x01;	//填充一个矩形
	USART4_TX_BUF[USART4_TX_CNT++] = x1>>8;		//x坐标 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = x1;		//x坐标 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = y1>>8;		//y坐标 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = y1;		//y坐标 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = x2>>8;		//x坐标 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = x2;		//x坐标 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = y2>>8;		//y坐标 高字节
	USART4_TX_BUF[USART4_TX_CNT++] = y2;		//y坐标 低字节
	USART4_TX_BUF[USART4_TX_CNT++] = 0x04;	
	USART4_TX_BUF[USART4_TX_CNT++] = 0x1F;	//填充颜色
	USART4_SendDat(USART4_TX_BUF, USART4_TX_CNT);
	//USART1_Send_Data(USART4_TX_BUF,USART4_TX_CNT);
}

/************************************************************************************
* 函数名称：liebiao_view
* 功能描述：迪文屏列表显示程序
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void liebiao_view(void)
{
	diwen_send_numascii(0x1100,(u8*)my5010_display1,strlen(my5010_display1));
	delay_ms(100);
	diwen_send_numascii(0x1127,(u8*)my5010_display2,strlen(my5010_display2));
	delay_ms(100);
	diwen_send_numascii(0x114e,(u8*)my5010_display3,strlen(my5010_display3));
}

