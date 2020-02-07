#ifndef __BASIC_TIM_H__
#define __BASIC_TIM_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/********************ͨ�ö�ʱ��TIM�������壬TIM2~TIM5************/

#define GENERAL_TIM5                     TIM5
#define GENERAL_TIM5_RCC_CLK_ENABLE()     __HAL_RCC_TIM5_CLK_ENABLE()
#define GENERAL_TIM5_RCC_CLK_DISABLE()    __HAL_RCC_TIM5_CLK_DISABLE()
#define GENERAL_TIM5_IRQ                  TIM5_IRQn
#define GENERAL_TIM5_INT_FUN              TIM5_IRQHandler

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/��GENERAL_TIM_PRESCALER+1��
#define GENERAL_TIM5_PRESCALER           83  // ʵ��ʱ��Ƶ��Ϊ��1MHz
// ���嶨ʱ�����ڣ�����ʱ����ʼ������BASIC_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define GENERAL_TIM5_PERIOD              (10000-1)  // ��ʱ�������ж�Ƶ��Ϊ��1MHz/1000=1KHz����100ms��ʱ����

// ���ն�ʱ��Ƶ�ʼ���Ϊ�� 84MHz/��GENERAL_TIM_PRESCALER+1��/(GENERAL_TIM_PERIOD+1)
// ������Ҫ����1ms���ڶ�ʱ����������Ϊ�� 84MHz/��83+1��/1000=1KHz����1ms����
// �������� GENERAL_TIM_PRESCALER=83��GENERAL_TIM_PERIOD=1000-1��

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim5;
/* �������� ------------------------------------------------------------------*/

void GENERAL_TIM5_Init(void);

/* ���Ͷ��� ---------------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/********************������ʱ��TIM�������壬ֻ��TIM6 & TIM7************/

#define BASIC_TIM6                    TIM6
#define BASIC_TIM6_RCC_CLK_ENABLE()     __HAL_RCC_TIM6_CLK_ENABLE()
#define BASIC_TIM6_RCC_CLK_DISABLE()    __HAL_RCC_TIM6_CLK_DISABLE()
#define BASIC_TIM6_IRQ                  TIM6_DAC_IRQn
#define BASIC_TIM6_INT_FUN              TIM6_DAC_IRQHandler


// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/��BASIC_TIMx_PRESCALER+1��
#define BASIC_TIM6_PRESCALER           83  // ʵ��ʱ��Ƶ��Ϊ��1MHz
// ���嶨ʱ�����ڣ�����ʱ����ʼ������BASIC_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define BASIC_TIM6_PERIOD              (10000-1)  // ��ʱ�������ж�Ƶ��Ϊ��1MHz/1000=1KHz����10ms��ʱ����

// ���ն�ʱ��Ƶ�ʼ���Ϊ�� 84MHz/��BASIC_TIMx_PRESCALER+1��/(BASIC_TIMx_PERIOD+1)
// ������Ҫ����1ms���ڶ�ʱ����������Ϊ�� 84MHz/��83+1��/1000=1kHz����1ms����
// �������� BASIC_TIMx_PRESCALER=83��BASIC_TIMx_PERIOD=1000-1��

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
/* �������� ------------------------------------------------------------------*/

void BASIC_TIM6_Init(void);

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/********************������ʱ��TIM�������壬ֻ��TIM6 & TIM7************/

#define BASIC_TIM7                     TIM7
#define BASIC_TIM7_RCC_CLK_ENABLE()     __HAL_RCC_TIM7_CLK_ENABLE()
#define BASIC_TIM7_RCC_CLK_DISABLE()    __HAL_RCC_TIM7_CLK_DISABLE()
#define BASIC_TIM7_IRQ                  TIM7_IRQn
#define BASIC_TIM7_INT_FUN              TIM7_IRQHandler


// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/��BASIC_TIMx_PRESCALER+1��
#define BASIC_TIM7_PRESCALER           83  // ʵ��ʱ��Ƶ��Ϊ��1MHz
// ���嶨ʱ�����ڣ�����ʱ����ʼ������BASIC_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define BASIC_TIM7_PERIOD              (10000-1)  // ��ʱ�������ж�Ƶ��Ϊ��1MHz/1000=1KHz����10ms��ʱ����

// ���ն�ʱ��Ƶ�ʼ���Ϊ�� 84MHz/��BASIC_TIMx_PRESCALER+1��/(BASIC_TIMx_PERIOD+1)
// ������Ҫ����1ms���ڶ�ʱ����������Ϊ�� 84MHz/��83+1��/1000=1KHz����1ms����
// �������� BASIC_TIMx_PRESCALER=83��BASIC_TIMx_PERIOD=1000-1��

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim7;
/* �������� ------------------------------------------------------------------*/

void BASIC_TIM7_Init(void);

#endif	/* __GENERAL_TIM_H__ */


