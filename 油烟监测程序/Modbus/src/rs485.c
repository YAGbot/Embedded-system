#include "rs485.h"
#include "uart.h"
#include "usart4.h"

u8 rs485_recv_flag = 0; //485接收数据状态位

u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
u8 RS485_RX_CNT=0;			//接收到的数据长度

u8 RS485_TX_BUF[64];  	//发送缓冲
u8 RS485_TX_CNT;				//发送的数据长度


void RS485_Init(u32 pclk2,u32 baud)
{
	float USARTDIV;
	u16 DIV_Mantissa;		//整数部分
	u16 DIV_Fraction;		//小数部分
	
	pclk2 = pclk2 * 1000000;		//单位转换为Hz
	
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
	
	USARTDIV = (float)pclk2/baud/16;						//由baud=fck/(16*USARTDIV)计算
	DIV_Mantissa = USARTDIV;										//得到整数部分
	DIV_Fraction = (USARTDIV-DIV_Mantissa)*16;	//得到小数部分
	USART3->BRR = (DIV_Mantissa<<4) + DIV_Fraction;	//得到BRR的值
	
	NVIC_SetPriority(USART3_IRQn,NVIC_EncodePriority(7-2,2,2));
							//USART2中断通道使能  
	NVIC_EnableIRQ(USART3_IRQn);
	USART3->CR1|=(1<<5);							//使能接收中断 
	
	USART3->CR1 |= 1<<3;												//发送使能
	USART3->CR1 |= 1<<2;												//接收使能
}

 /**
  * @brief  初始化rs485 usart3 
  * @param  无
  * @retval 无
  */
void rs485_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 第1步： 开启GPIO和UART时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* 第4步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = bound;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	 /* 第5步： NVIC 配置 */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(USART3, ENABLE);		/* 使能串口 */
}

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
	
void USART3_IRQHandler(void)
{
	u8 res;	    
	if(USART3->SR&(1<<5))//接收到数据
	{	 
		res=USART3->DR; 	
		if(RS485_RX_CNT<64)
		{
			time6_reset();	//计数器复位
			if(rs485_recv_flag==0) time6_start();	//定时器开始
			RS485_RX_BUF[RS485_RX_CNT++]=res;		//记录接收到的值
		}
		else 
		{
			rs485_recv_flag=1;		//强制标志接收完成
		}
	}  											 
}   


/************************************************************************************
* 函数名称：USART3_SendByte
* 功能描述：USART3发送一个字符函数
* 入口参数：-dat:要发送的数据
* 出口参数：无
*************************************************************************************/
void USART3_SendByte(u8 dat)
{
	while(!(USART3->SR&(1<<6)));			//等待移位寄存器为空
	USART3->DR = dat;
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
		while((USART3->SR&0X40)==0);//等待发送结束		  
		USART3->DR=buf[i];
	}	 
}



/************************************************************************************
* 函数名称：TIME6_Init
* 功能描述：TIME6初始化以及定时1s函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void TIME6_Init(u16 arr,u16 psc)
{   
  RCC->APB1ENR |= (1<<4);	//开TIME7外设时钟
  //配置定时器
	TIM6->PSC=psc; //设置预分频系数
	TIM6->ARR=arr; //设置重装载值
	TIM6->CR1 |= 1<<7;//开启影子寄存器
	TIM6->EGR |= 1<<0; //设置UG位,软件更新
	TIM6->SR =~0X01;   //清标志
	TIM6->DIER |=1<<0; //开启定时器中断  
  NVIC_SetPriority(TIM6_IRQn,NVIC_EncodePriority(7-2,2,2));//组2抢占优先级2，响应优先级0
  NVIC_EnableIRQ(TIM6_IRQn);		
}

/************************************************************************************
* 函数名称：time6_start
* 功能描述：定时器开始函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time6_start(void)
{
    //使能
    TIM6->CNT = 0;
    TIM6->CR1 |= (1<<0);    
}

/************************************************************************************
* 函数名称：time6_stop
* 功能描述：定时器停止函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time6_stop(void)
{
    TIM6->CR1 &= ~(1<<0);    
}

/************************************************************************************
* 函数名称：time7_set_timeout
* 功能描述：设置定时器时间函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time6_set_timeout(u16 time)
{
    TIM6->ARR = 10 * time;   //转成1ms的单位
}

/************************************************************************************
* 函数名称：time7_reset
* 功能描述：定时器复位倒计时函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time6_reset(void)
{
    TIM6->CNT = 0;
}

/************************************************************************************
* 函数名称：TIM7_IRQHandler
* 功能描述：定时器中断函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void TIM6_IRQHandler(void)
{
	if(TIM6->SR&0X01)				//是更新中断
	{
		rs485_recv_flag = 1;	//标记接收完成
		RS485_RX_CNT=0;
		TIM6->SR &= ~(1<<0);//清除中断标志位
		time6_stop();				//关闭定时器
		//UART5_SendDat(RS485_RX_BUF,17);
	}
}
