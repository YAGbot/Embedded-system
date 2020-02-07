#ifndef _ADC_H
#define _ADC_H


#include "stm32f10x.h"


/* ��������λ������ض˿� */
#define ADC_LIGHT_POT_BAT_PORT  GPIOB
#define ADC_LIGHT_POT_BAT_PIN   GPIO_Pin_0
#define ADC_LIGHT_POT_BAT_CH    ADC_Channel_8

/* �����˿� */
#define ADC_KEY_PORT            GPIOB
#define ADC_KEY_PIN             GPIO_Pin_1
#define ADC_KEY_CH              ADC_Channel_9


/* �ڲ��¶� */
#define ADC_ITEMP_CH              ADC_Channel_16





/* �������� */
void adc_init(void);
u16  adc_get(u32 ch); 
u16 adc_get_average(u32 ch, u8 times);



#endif










