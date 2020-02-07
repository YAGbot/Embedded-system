#ifndef __BSP_485_USART_H__
#define __BSP_485_USART_H__

/* 包含头文件 ----------------------------------------------------------------*/

#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define RS485_USARTx                                 USART3
#define RS485_USARTx_BAUDRATE                        9600
#define RS485_USART_RCC_CLK_ENABLE()                 __HAL_RCC_USART3_CLK_ENABLE()
#define RS485_USART_RCC_CLK_DISABLE()                __HAL_RCC_USART3_CLK_DISABLE()

#define RS485_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()
#define RS485_USARTx_Tx_GPIO_PIN                     GPIO_PIN_10
#define RS485_USARTx_Tx_GPIO                         GPIOB
#define RS485_USARTx_Rx_GPIO_PIN                     GPIO_PIN_11
#define RS485_USARTx_Rx_GPIO                         GPIOB

#define RS485_USART_IRQn                             USART3_IRQn
#define RS485_USART_IRQHANDLER                       USART3_IRQHandler

/* 使用485通信的时候才会用到使能IO */
#define RS485_REDE_GPIO_ClK_ENABLE()                 __HAL_RCC_GPIOH_CLK_ENABLE()
#define RS485_REDE_PORT                              GPIOH
#define RS485_REDE_PIN                               GPIO_PIN_8
#define RS485_RX_MODE()                              HAL_GPIO_WritePin(RS485_REDE_PORT,RS485_REDE_PIN,GPIO_PIN_RESET)
#define RS485_TX_MODE()                              HAL_GPIO_WritePin(RS485_REDE_PORT,RS485_REDE_PIN,GPIO_PIN_SET)

/* RTU通信需要确定超时时间 */
#if RS485_USARTx_BAUDRATE <= 19200
  /* 1.5个字符的超时时间 T = BAUDRATE/11/1000*/
  #define OVERTIME_15CHAR             (((float)RS485_USARTx_BAUDRATE/11)*1.5f)
  /* 3个字符的超时时间 */
  #define OVERTIME_35CHAR             (((float)RS485_USARTx_BAUDRATE/11)*3.5f)
#else 
  /* 波特率超过19200bit/s的情况下建议的超时时间 */
  #define OVERTIME_15CHAR                750.0f    // 750us 
  #define OVERTIME_35CHAR               1750.0f  // 1.75ms  
  
#endif
/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef husart_485;
extern uint8_t rs485Rx_Buf[256];    // 接收缓存,最大256字节
extern uint8_t rs485Tx_Buf[256];    // 发送缓存,最大256字节
extern uint8_t rs485RxBuffer;
extern __IO uint16_t RS485_USARTx_RX_STA;

extern UART_HandleTypeDef usartx_1;	
extern uint8_t aRxBuffer;//HAL库USART接收Buffer	
extern uint8_t usart1Rx_Buf[256];    // 接收缓存,最大256字节
extern uint16_t USART1_RX_STA;         		//接收状态标记	
/* 函数声明 ------------------------------------------------------------------*/
void RS485_USART_Init(void);
void UART_Tx(uint8_t *Tx_Buf,uint16_t TxCount);
void RS485_Receive_Data(uint8_t *buf,uint8_t *len);

void MX_USARTx_Init(void);
#endif  /* __BSP_485_USART_H__ */

