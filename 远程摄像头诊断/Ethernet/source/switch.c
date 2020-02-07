/**
  **************************************************************************************
  * @File      switch.c
  * @Author    ChuanYong Wan
  * @brief   	 Switch drive functions.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *								 Without permission, shall not be reproduced and copied, shall not be 
	*								 used for commercial purposes.
  **************************************************************************************
 **/
 
/* Include ----------------------------------------------------------------------------*/
#include "sys.h"
#include "switch.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

/* Function ---------------------------------------------------------------------------*/
void Switch_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;		
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Read the switchs of line 1 
*@param  None
*@return A decimal number(0~255)
*/ 
uint8_t Switch_Line1Read(void)
{
	return PAin(1);
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Read the switchs of line 2 
*@param  None
*@return A decimal number(0~255)
*/ 
uint8_t Switch_Line2Read(void)
{
	return PBin(1);
}
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/


