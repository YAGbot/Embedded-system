
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "usart/bsp_usartx.h"
#include <string.h>
#include <stdio.h>

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
UART_HandleTypeDef husartx;

/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ���Դ��ڹ�������GPIO��ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* ��������ʱ��ʹ�� */
    USARTx_GPIO_ClK_ENABLE();
    
    /* �������蹦��GPIO���� */
    GPIO_InitStruct.Pin = USARTx_Tx_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(USARTx_Tx_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USARTx_Rx_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(USARTx_Rx_GPIO, &GPIO_InitStruct);
    
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void USARTx_Init(void)
{ 
  GPIO_Init();
  USARTx_RCC_CLK_ENABLE();
  husartx.Instance = USARTx;
  husartx.Init.BaudRate = USARTx_BAUDRATE;
  husartx.Init.WordLength = UART_WORDLENGTH_8B;
  husartx.Init.StopBits = UART_STOPBITS_1;
  husartx.Init.Parity = UART_PARITY_NONE;
  husartx.Init.Mode = UART_MODE_TX_RX;
  husartx.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husartx);

  
}

/**
  * ��������: �ض���c�⺯��printf��USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&husartx, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
  * ��������: �ض���c�⺯��getchar,scanf��USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&husartx,&ch, 1, 0xffff);
  return ch;
}

/**
  * ��������: ���ڷ��ͺ���
  * �������: Tx_Bul:�����ַ���,TxCount:���͵��ֽ���
  * �� �� ֵ: ��
  * ˵    ��: ���⺯�������װ.
  */
void USART_Tx(uint8_t *Tx_Buf,uint16_t TxCount)
{
  HAL_UART_Transmit(&husartx, Tx_Buf, TxCount, 0xffff);
  while(__HAL_UART_GET_FLAG(&husartx, UART_FLAG_TXE) == RESET);
}
