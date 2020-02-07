#include "timer.h"
#include "delay.h"
#include "usart4.h"

u8 worktimes_flag;
u16 worktime=0;
/************************************************************************************
* 函数名称：TIME3_Init
* 功能描述：TIME3初始化以及定时1s函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void TIM3_Init(u16 psc,u16 arr)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能 
	TIM3->PSC=psc; //设置预分频系数
	TIM3->ARR=arr; //设置重装载值
	TIM3->CR1 |= 1<<7;//开启影子寄存器
	TIM3->EGR |= 1<<0; //设置UG位,软件更新
	TIM3->SR =~0X01;   //清标志
	TIM3->DIER |=1<<0; //开启定时器中断
	//设置NVIC
	NVIC_SetPriorityGrouping(7-2); //设置为分组2
	NVIC_SetPriority(TIM3_IRQn,NVIC_EncodePriority(7-2,2,2));//抢占优先级设置为,响应优先级为2
	NVIC_EnableIRQ(TIM3_IRQn);
}

/************************************************************************************
* 函数名称：time6_start
* 功能描述：定时器开始函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time3_start(void)
{
    //使能
    TIM3->CNT = 0;
    TIM3->CR1 |= (1<<0);    
}

/************************************************************************************
* 函数名称：time6_stop
* 功能描述 ：定时器停止函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time3_stop(void)
{
    TIM3->CR1 &= ~(1<<0);    
}


/************************************************************************************
* 函数名称：TIM3_IRQHandler
* 功能描述：TIME3更新中断函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void TIM3_IRQHandler(void)
{
	if(TIM3->SR & 1<<0) //定时器3中断
	{
		TIM3->SR &= ~(1<<0); //清标志
		worktime++;
		if(worktime>55)	//定时10s 等待
		{
			worktime=0;
			worktimes_flag=1;
		}
	}
}

/************************************************************************************
* 函数名称：TIME4_Init
* 功能描述：TIME4初始化以及定时1s函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void TIM4_Init(u16 psc,u16 arr)
{
	RCC->APB1ENR|=1<<2;	//TIM3时钟使能 
	TIM4->PSC=psc; //设置预分频系数
	TIM4->ARR=arr; //设置重装载值
	TIM4->CR1 |= 1<<7;//开启影子寄存器
	TIM4->EGR |= 1<<0; //设置UG位,软件更新
	TIM4->SR =~0X01;   //清标志
	TIM4->DIER |=1<<0; //开启定时器中断
	//设置NVIC
	NVIC_SetPriorityGrouping(7-2); //设置为分组2
	NVIC_SetPriority(TIM4_IRQn,NVIC_EncodePriority(7-2,2,2));//抢占优先级设置为,响应优先级为2
	NVIC_EnableIRQ(TIM4_IRQn);
}

/************************************************************************************
* 函数名称：time4_start
* 功能描述：定时器开始函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time4_start(void)
{
    //使能
    TIM4->CNT = 0;
    TIM4->CR1 |= (1<<0);    
}

/************************************************************************************
* 函数名称：time4_stop
* 功能描述 ：定时器停止函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void time4_stop(void)
{
    TIM4->CR1 &= ~(1<<0);    
}


/************************************************************************************
* 函数名称：TIM4_IRQHandler
* 功能描述：TIME4更新中断函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void TIM4_IRQHandler(void)
{
	if(TIM4->SR & 1<<0) //定时器3中断
	{
		USART4_RX_STA|=1<<15;	//标记接收完成
		TIM4->SR &= ~(1<<0); //清标志
		time4_stop();				//关闭定时器
	}
}

