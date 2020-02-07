#ifndef __BSP_GSM_USART_H__
#define	__BSP_GSM_USART_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef enum
{
  GSM_TRUE,
  GSM_FALSE,    
}GSM_res_e;
/* 宏定义 --------------------------------------------------------------------*/
#define GSM_USARTx                                 UART4
#define GSM_USARTx_BAUDRATE                        115200
#define GSM_USARTx_RCC_CLK_ENABLE()                 __HAL_RCC_UART4_CLK_ENABLE()
#define GSM_USARTx_RCC_CLK_DISABLE()                __HAL_RCC_UART4_CLK_DISABLE()

#define GSM_USARTx_GPIO_ClK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define GSM_USARTx_PORT                            GPIOC
#define GSM_USARTx_Tx_PIN                          GPIO_PIN_10
#define GSM_USARTx_Rx_PIN                          GPIO_PIN_11

#define GSM_UART_BUFF_SIZE      255

/* 扩展变量 ------------------------------------------------------------------*/
extern UART_HandleTypeDef husartx_GSM;
extern __IO  uint16_t gsmuart_p;
extern uint8_t   gsm_uart_buff[GSM_UART_BUFF_SIZE];
extern uint8_t gsmRxBuffer;
/* 函数声明 ------------------------------------------------------------------*/
void GSM_USARTx_Init(void);
char *get_rebuff(uint8_t *len); 
void clean_rebuff(void);
void Usart_SendByte(uint8_t ch );
void GSM_USART_printf( char *Data,...);

uint8_t GSM_cmd(char *cmd,char *reply,uint32_t waittime);
uint8_t GSM_cmd_check(char *reply);
uint8_t GSM_init(void); //初始化并检测模块
#endif /* __BTL_USARTX_H__ */

