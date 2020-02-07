#ifndef __ADC_H__
#define	__ADC_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "sys.h" 

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
// ע�⣺����ADC�ɼ���IO����û�и��ã�����ɼ���ѹ����Ӱ��
/********************ADC����ͨ�������ţ�����**************************/
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd

#define    ADCx                          ADC1
#define    ADC_CLK                       RCC_APB2Periph_ADC1
#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA1
#define    ADC_DMA_CHANNEL               DMA1_Channel1

//#define    ADCx                          ADC3
//#define    ADC_CLK                       RCC_APB2Periph_ADC3
//#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA2
//#define    ADC_DMA_CHANNEL               DMA2_Channel5

#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADC_PORT                      GPIOC

#define    ADC_PIN1                      GPIO_Pin_0      
#define    ADC_CHANNEL1                  ADC_Channel_10    
#define    ADC_PIN2                      GPIO_Pin_1      
#define    ADC_CHANNEL2                  ADC_Channel_11    
#define    ADC_PIN3                      GPIO_Pin_2       
#define    ADC_CHANNEL3                  ADC_Channel_12   
//#define    ADC_PIN4                      GPIO_Pin_3       
//#define    ADC_CHANNEL4                  ADC_Channel_13    

#define    ADC_NUMOFCHANNEL              3

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void ADCx_Init(void);

#endif /* __ADC_H__ */
