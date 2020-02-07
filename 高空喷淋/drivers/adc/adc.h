#ifndef _ADC_H
#define _ADC_H


#include "stm32f10x.h"


/* 光敏、电位器、电池端口 */
#define ADC_LIGHT_POT_BAT_PORT  GPIOB
#define ADC_LIGHT_POT_BAT_PIN   GPIO_Pin_0
#define ADC_LIGHT_POT_BAT_CH    ADC_Channel_8

/* 按键端口 */
#define ADC_KEY_PORT            GPIOB
#define ADC_KEY_PIN             GPIO_Pin_1
#define ADC_KEY_CH              ADC_Channel_9


/* 内部温度 */
#define ADC_ITEMP_CH              ADC_Channel_16





/* 函数声明 */
void adc_init(void);
u16  adc_get(u32 ch); 
u16 adc_get_average(u32 ch, u8 times);



#endif










