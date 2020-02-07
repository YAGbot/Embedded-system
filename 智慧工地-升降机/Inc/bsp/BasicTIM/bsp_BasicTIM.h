#ifndef __BASIC_TIM_H__
#define __BASIC_TIM_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/********************通用定时器TIM参数定义，TIM2~TIM5************/

#define GENERAL_TIM5                     TIM5
#define GENERAL_TIM5_RCC_CLK_ENABLE()     __HAL_RCC_TIM5_CLK_ENABLE()
#define GENERAL_TIM5_RCC_CLK_DISABLE()    __HAL_RCC_TIM5_CLK_DISABLE()
#define GENERAL_TIM5_IRQ                  TIM5_IRQn
#define GENERAL_TIM5_INT_FUN              TIM5_IRQHandler

// 定义定时器预分频，定时器实际时钟频率为：84MHz/（GENERAL_TIM_PRESCALER+1）
#define GENERAL_TIM5_PRESCALER           83  // 实际时钟频率为：1MHz
// 定义定时器周期，当定时器开始计数到BASIC_TIMx_PERIOD值是更新定时器并生成对应事件和中断
#define GENERAL_TIM5_PERIOD              (10000-1)  // 定时器产生中断频率为：1MHz/1000=1KHz，即100ms定时周期

// 最终定时器频率计算为： 84MHz/（GENERAL_TIM_PRESCALER+1）/(GENERAL_TIM_PERIOD+1)
// 比如需要产生1ms周期定时，可以设置为： 84MHz/（83+1）/1000=1KHz，即1ms周期
// 这里设置 GENERAL_TIM_PRESCALER=83；GENERAL_TIM_PERIOD=1000-1；

/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim5;
/* 函数声明 ------------------------------------------------------------------*/

void GENERAL_TIM5_Init(void);

/* 类型定义 ---------------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/********************基本定时器TIM参数定义，只限TIM6 & TIM7************/

#define BASIC_TIM6                    TIM6
#define BASIC_TIM6_RCC_CLK_ENABLE()     __HAL_RCC_TIM6_CLK_ENABLE()
#define BASIC_TIM6_RCC_CLK_DISABLE()    __HAL_RCC_TIM6_CLK_DISABLE()
#define BASIC_TIM6_IRQ                  TIM6_DAC_IRQn
#define BASIC_TIM6_INT_FUN              TIM6_DAC_IRQHandler


// 定义定时器预分频，定时器实际时钟频率为：84MHz/（BASIC_TIMx_PRESCALER+1）
#define BASIC_TIM6_PRESCALER           83  // 实际时钟频率为：1MHz
// 定义定时器周期，当定时器开始计数到BASIC_TIMx_PERIOD值是更新定时器并生成对应事件和中断
#define BASIC_TIM6_PERIOD              (10000-1)  // 定时器产生中断频率为：1MHz/1000=1KHz，即10ms定时周期

// 最终定时器频率计算为： 84MHz/（BASIC_TIMx_PRESCALER+1）/(BASIC_TIMx_PERIOD+1)
// 比如需要产生1ms周期定时，可以设置为： 84MHz/（83+1）/1000=1kHz，即1ms周期
// 这里设置 BASIC_TIMx_PRESCALER=83；BASIC_TIMx_PERIOD=1000-1；

/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
/* 函数声明 ------------------------------------------------------------------*/

void BASIC_TIM6_Init(void);

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/********************基本定时器TIM参数定义，只限TIM6 & TIM7************/

#define BASIC_TIM7                     TIM7
#define BASIC_TIM7_RCC_CLK_ENABLE()     __HAL_RCC_TIM7_CLK_ENABLE()
#define BASIC_TIM7_RCC_CLK_DISABLE()    __HAL_RCC_TIM7_CLK_DISABLE()
#define BASIC_TIM7_IRQ                  TIM7_IRQn
#define BASIC_TIM7_INT_FUN              TIM7_IRQHandler


// 定义定时器预分频，定时器实际时钟频率为：84MHz/（BASIC_TIMx_PRESCALER+1）
#define BASIC_TIM7_PRESCALER           83  // 实际时钟频率为：1MHz
// 定义定时器周期，当定时器开始计数到BASIC_TIMx_PERIOD值是更新定时器并生成对应事件和中断
#define BASIC_TIM7_PERIOD              (10000-1)  // 定时器产生中断频率为：1MHz/1000=1KHz，即10ms定时周期

// 最终定时器频率计算为： 84MHz/（BASIC_TIMx_PRESCALER+1）/(BASIC_TIMx_PERIOD+1)
// 比如需要产生1ms周期定时，可以设置为： 84MHz/（83+1）/1000=1KHz，即1ms周期
// 这里设置 BASIC_TIMx_PRESCALER=83；BASIC_TIMx_PERIOD=1000-1；

/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim7;
/* 函数声明 ------------------------------------------------------------------*/

void BASIC_TIM7_Init(void);

#endif	/* __GENERAL_TIM_H__ */


