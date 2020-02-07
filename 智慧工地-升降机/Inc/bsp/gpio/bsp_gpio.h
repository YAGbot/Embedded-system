#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 --------------------------------------------------------------*/
typedef enum
{
  KEY_UP   = 0,
  KEY_DOWN = 1,
}KEYState_TypeDef;

/* 宏定义 --------------------------------------------------------------------*/
/********************输入定义***************************/
#define INPUT1_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT1_GPIO_PIN                 GPIO_PIN_8
#define INPUT1_GPIO                     GPIOG
#define INPUT1_DOWN_LEVEL               1  /* 根据原理图设计，INPUT引脚为高电平时表示检测到输入，所以这里设置为1 */

#define INPUT2_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT2_GPIO_PIN                 GPIO_PIN_9
#define INPUT2_GPIO                     GPIOG
#define INPUT2_DOWN_LEVEL               1  /* 根据原理图设计，INPUT引脚为高电平时表示检测到输入，所以这里设置为1 */

#define INPUT3_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT3_GPIO_PIN                 GPIO_PIN_10
#define INPUT3_GPIO                     GPIOG
#define INPUT3_DOWN_LEVEL               1  /* 根据原理图设计，INPUT引脚为高电平时表示检测到输入，所以这里设置为1 */

#define INPUT4_RCC_CLK_ENABLE           __HAL_RCC_GPIOG_CLK_ENABLE
#define INPUT4_GPIO_PIN                 GPIO_PIN_15
#define INPUT4_GPIO                     GPIOG
#define INPUT4_DOWN_LEVEL               1  /* 根据原理图设计，INPUT引脚为高电平时表示检测到输入，所以这里设置为1 */

/*******************输出定义**********************************/
#define OUTPUT1_RCC_CLK_ENABLE           __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT1_GPIO_PIN                 GPIO_PIN_12
#define OUTPUT1_GPIO                     GPIOF

#define OUTPUT2_RCC_CLK_ENABLE           __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT2_GPIO_PIN                 GPIO_PIN_13
#define OUTPUT2_GPIO                     GPIOF

#define OUTPUT3_RCC_CLK_ENABLE           __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT3_GPIO_PIN                 GPIO_PIN_14
#define OUTPUT3_GPIO                     GPIOF

#define OUTPUT4_RCC_CLK_ENABLE           __HAL_RCC_GPIOF_CLK_ENABLE
#define OUTPUT4_GPIO_PIN                 GPIO_PIN_15
#define OUTPUT4_GPIO                     GPIOF

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void INPUT_GPIO_Init(void);
void OUTPUT_GPIO_Init(void);
#endif  // __BSP_GPIO_H__


