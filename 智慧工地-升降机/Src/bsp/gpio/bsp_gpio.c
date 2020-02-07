
/* 包含头文件 ----------------------------------------------------------------*/
#include "gpio/bsp_gpio.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 板载按键IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_key.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void INPUT_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 使能(开启)INPUT引脚对应IO端口时钟 */  
  INPUT1_RCC_CLK_ENABLE();

  
  /* 配置INPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = INPUT1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT1_GPIO, &GPIO_InitStruct);   
  
  /* 配置INPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = INPUT2_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT2_GPIO, &GPIO_InitStruct);  
  
  /* 配置INPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = INPUT3_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT3_GPIO, &GPIO_InitStruct);   
  
  /* 配置INPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = INPUT4_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INPUT4_GPIO, &GPIO_InitStruct);   
  
//  /* 配置中断优先级 */
//  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

//    /* 配置中断优先级 */
//  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 1);
//  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
  * 函数功能: 板载按键IO引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_gpio.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void OUTPUT_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* 使能(开启)光耦引脚对应IO端口时钟 */  
  OUTPUT1_RCC_CLK_ENABLE();  
  OUTPUT2_RCC_CLK_ENABLE();  
  OUTPUT3_RCC_CLK_ENABLE();  
  OUTPUT4_RCC_CLK_ENABLE();  
        
  /* 配置OUTPUT GPIO:输入上拉模式 */
  GPIO_InitStruct.Pin = OUTPUT1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT1_GPIO, &GPIO_InitStruct);   
  
  GPIO_InitStruct.Pin = OUTPUT2_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT2_GPIO, &GPIO_InitStruct);    

  GPIO_InitStruct.Pin = OUTPUT3_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT3_GPIO, &GPIO_InitStruct);   

  GPIO_InitStruct.Pin = OUTPUT4_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTPUT4_GPIO, &GPIO_InitStruct);   
}


