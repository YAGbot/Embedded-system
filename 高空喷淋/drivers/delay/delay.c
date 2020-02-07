/**
  ******************************************************************************
  * @file    delay.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   ��ʱ��������
  ******************************************************************************
  */
#include "delay.h"



static u16 x_ms = 0;	

/**
  * @brief ��ʱ��ʼ��10ms�ж�һ��
  * @retval ��
  */
void delay_init(void)
{
	SysTick_Config(SystemCoreClock/100);
}

/**
  * @brief ��ʱms �������������
  * @param  nums:Ҫ��ʱ��ms��   0~63335ms
  * @retval ��
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
  * @brief 10MSˢ��һ��
  * @retval 
  */
void SysTick_Handler(void)
{			
	if(x_ms)x_ms--;
}


