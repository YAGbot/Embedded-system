#ifndef __BSP_USARTX_HMI_H__
#define __BSP_USARTX_HMI_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/

/* 宏定义 --------------------------------------------------------------------*/
#define HMI_USARTx_REC_LEN  			200  	//定义最大接收字节数 200
#define HMI_USARTx_SEND_LEN		200				//最大发送缓存字节数

#define HMI_USARTx                                 USART2
#define HMI_USARTx_BAUDRATE                        115200
#define HMI_USART_RCC_CLK_ENABLE()                 __HAL_RCC_USART2_CLK_ENABLE()
#define HMI_USART_RCC_CLK_DISABLE()                __HAL_RCC_USART2_CLK_DISABLE()

#define HMI_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOD_CLK_ENABLE()
#define HMI_USARTx_PORT                            GPIOD
#define HMI_USARTx_Tx_PIN                          GPIO_PIN_5
#define HMI_USARTx_Rx_PIN                          GPIO_PIN_6

#define HMI_USARTx_IRQHANDLER                      USART2_IRQHandler
#define HMI_USARTx_IRQn                            USART2_IRQn
 
/* 扩展变量 ------------------------------------------------------------------*/
extern uint8_t hmiRxBuffer;
extern UART_HandleTypeDef husartx_HMI;
extern __IO uint8_t HMI_USARTx_TX_CNT;
extern  uint8_t  HMI_USARTx_TX_BUF[HMI_USARTx_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节	  	
extern  uint8_t  HMI_USARTx_RX_BUF[HMI_USARTx_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern __IO uint16_t HMI_USARTx_RX_STA;         		//接收状态标记	
/* 函数声明 ------------------------------------------------------------------*/
void HMI_USARTx_Init(void);
void HMI_USARTx_SendDat(uint8_t *buf,uint8_t len);
void diwen_send_data(uint16_t _reg,uint32_t _data);
void diwen_send_data8(uint16_t _reg,int8_t _data);
void diwen_send_data16(uint16_t _reg,int16_t _data);
void diwen_send_asci3(uint16_t _reg,uint16_t _data1,uint16_t _data2,uint16_t _data3);
void diwen_send_numascii(uint16_t _reg,uint8_t *buf,uint8_t len);
void check_IMI_cmd(void);
void Data_Scan(void);
#endif  /* __BSP_USARTX_HMI_H__ */


