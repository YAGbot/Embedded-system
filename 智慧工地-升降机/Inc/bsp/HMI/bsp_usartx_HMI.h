#ifndef __BSP_USARTX_HMI_H__
#define __BSP_USARTX_HMI_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/

/* �궨�� --------------------------------------------------------------------*/
#define HMI_USARTx_REC_LEN  			200  	//�����������ֽ��� 200
#define HMI_USARTx_SEND_LEN		200				//����ͻ����ֽ���

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
 
/* ��չ���� ------------------------------------------------------------------*/
extern uint8_t hmiRxBuffer;
extern UART_HandleTypeDef husartx_HMI;
extern __IO uint8_t HMI_USARTx_TX_CNT;
extern  uint8_t  HMI_USARTx_TX_BUF[HMI_USARTx_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�	  	
extern  uint8_t  HMI_USARTx_RX_BUF[HMI_USARTx_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern __IO uint16_t HMI_USARTx_RX_STA;         		//����״̬���	
/* �������� ------------------------------------------------------------------*/
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


