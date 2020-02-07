#ifndef __BSP_ADC_H__
#define	__BSP_ADC_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
// ע�⣺����ADC�ɼ���IO����û�и��ã�����ɼ���ѹ����Ӱ��
/********************ADC����ͨ�������ţ�����**************************/
#define ADCx_RCC_CLK_ENABLE()            __HAL_RCC_ADC1_CLK_ENABLE()
#define ADCx_RCC_CLK_DISABLE()           __HAL_RCC_ADC1_CLK_DISABLE()
#define DMAx_RCC_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
#define ADCx                             ADC1
#define ADCx_DMA_IRQx                    DMA2_Stream0_IRQn
#define ADCx_DMA_IRQx_Handler            DMA2_Stream0_IRQHandler
#define DMAx__Stream_x                   DMA2_Stream0
#define DMAx__CHANNEL_x                  DMA_CHANNEL_0

//#define ADCx_RCC_CLK_ENABLE()            __HAL_RCC_ADC2_CLK_ENABLE()
//#define ADCx_RCC_CLK_DISABLE()           __HAL_RCC_ADC2_CLK_DISABLE()
//#define DMAx_RCC_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
//#define ADCx                             ADC2
//#define ADCx_DMA_IRQx                    DMA2_Stream2_IRQn
//#define ADCx_DMA_IRQx_Handler            DMA2_Stream2_IRQHandler
//#define DMAx__Stream_x                   DMA2_Stream2
//#define DMAx__CHANNEL_x                  DMA_CHANNEL_1

//#define ADCx_RCC_CLK_ENABLE()            __HAL_RCC_ADC3_CLK_ENABLE()
//#define ADCx_RCC_CLK_DISABLE()           __HAL_RCC_ADC3_CLK_DISABLE()
//#define DMAx_RCC_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
//#define ADCx                             ADC3
//#define ADCx_DMA_IRQx                    DMA2_Stream0_IRQn
//#define ADCx_DMA_IRQx_Handler            DMA2_Stream0_IRQHandler
//#define DMAx__Stream_x                   DMA2_Stream0
//#define DMAx__CHANNEL_x                  DMA_CHANNEL_2


#define ADC_GPIO_ClK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define ADC_GPIO                         GPIOA
#define ADC_GPIO_PIN                     GPIO_PIN_4        
#define ADC_CHANNEL                      ADC_CHANNEL_4    
                                 
/* ��չ���� ------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadcx;
extern DMA_HandleTypeDef hdma_adcx;

/* �������� ------------------------------------------------------------------*/
void MX_ADCx_Init(void);
void MX_DMA_Init(void) ;
#endif /* __ADC_H__ */


