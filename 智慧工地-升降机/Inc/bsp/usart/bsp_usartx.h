#ifndef __BSP_USARTX_H__
#define	__BSP_USARTX_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define USARTx                                 UART5
#define USARTx_BAUDRATE                        115200
#define USARTx_RCC_CLK_ENABLE()                 __HAL_RCC_UART5_CLK_ENABLE()
#define USARTx_RCC_CLK_DISABLE()               	__HAL_RCC_UART5_CLK_DISABLE()

#define USARTx_GPIO_ClK_ENABLE()               {__HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOD_CLK_ENABLE();}
#define USARTx_Tx_GPIO                         GPIOC
#define USARTx_Tx_PIN                          GPIO_PIN_12
#define USARTx_Rx_PIN                          GPIO_PIN_2
#define USARTx_Rx_GPIO                         GPIOD

/* ��չ���� ------------------------------------------------------------------*/
extern UART_HandleTypeDef husartx;

/* �������� ------------------------------------------------------------------*/
void USARTx_Init(void);
void USART_Tx(uint8_t *Tx_Buf,uint16_t TxCount);

#endif /* __BTL_USARTX_H__ */


