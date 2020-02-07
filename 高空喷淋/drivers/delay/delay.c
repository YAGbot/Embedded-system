/**
  ******************************************************************************
  * @file    delay.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   延时驱动代码
  ******************************************************************************
  */
#include "delay.h"



static u16 x_ms = 0;	

/**
  * @brief 延时初始化10ms中断一次
  * @retval 无
  */
void delay_init(void)
{
	SysTick_Config(SystemCoreClock/100);
}

/**
  * @brief 延时ms 会引起任务调度
  * @param  nums:要延时的ms数   0~63335ms
  * @retval 无
  */
void delay_ms(u32 nms)
{				
	if(nms >= 10)					
	{ 		
		x_ms = nms / 10;	
	}
	
	while(x_ms);
}

/**
  * @brief 10MS刷新一次
  * @retval 
  */
void SysTick_Handler(void)
{			
	if(x_ms)x_ms--;
}


