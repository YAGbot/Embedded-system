/**
  ******************************************************************************
  485���ڳ��� USART3
  ******************************************************************************
  */
#include "usart/bsp_485_usart.h"
#include "HMI/bsp_usartx_HMI.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
UART_HandleTypeDef husart_485;
UART_HandleTypeDef usartx_1;
uint8_t rs485Rx_Buf[256];            // ���ջ���,���256�ֽ�
uint8_t rs485Tx_Buf[256];            // ���ͻ���,���256�ֽ�
uint8_t rs485RxBuffer;
__IO uint16_t RS485_USARTx_RX_STA=0;

uint16_t USART1_RX_STA=0;
uint8_t aRxBuffer;
uint8_t usart1Rx_Buf[256];
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ����Ӳ����ʼ������
  * �������: huart�����ھ������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  if(huart->Instance==RS485_USARTx)
  {
    /* ��������ʱ��ʹ�� */
    RS485_USARTx_GPIO_ClK_ENABLE();
    RS485_USART_RCC_CLK_ENABLE();
    RS485_REDE_GPIO_ClK_ENABLE();
    /* �������蹦��GPIO���� */
    GPIO_InitStruct.Pin = RS485_USARTx_Tx_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(RS485_USARTx_Tx_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = RS485_USARTx_Rx_GPIO_PIN;
    HAL_GPIO_Init(RS485_USARTx_Rx_GPIO, &GPIO_InitStruct);
       
    /* SP3485E��������ʹ�ܿ������ų�ʼ�� */
    HAL_GPIO_WritePin(RS485_REDE_PORT,RS485_REDE_PIN,GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = RS485_REDE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RS485_REDE_PORT, &GPIO_InitStruct);
    
  }  
	else if(huart->Instance==HMI_USARTx)
	{
	 GPIO_InitTypeDef GPIO_InitStruct;
    /* ʹ�ܴ��ڹ�������GPIOʱ�� */
   HMI_USARTx_GPIO_ClK_ENABLE();
  
   /* �������蹦��GPIO���� */
   GPIO_InitStruct.Pin = HMI_USARTx_Tx_PIN|HMI_USARTx_Rx_PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
   HAL_GPIO_Init(HMI_USARTx_PORT, &GPIO_InitStruct);
    
   GPIO_InitStruct.Pin = HMI_USARTx_Rx_PIN;
   HAL_GPIO_Init(HMI_USARTx_PORT, &GPIO_InitStruct);  
	}
	else if(huart->Instance==USART1)
  {
    /* ʹ�ܴ��ڹ�������GPIOʱ�� */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /* �������蹦��GPIO���� */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
	
}

/**
  * ��������: ����Ӳ������ʼ������
  * �������: huart�����ھ������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==RS485_USARTx)
  {
    /* ��������ʱ�ӽ��� */
    RS485_USART_RCC_CLK_DISABLE();
  
    /* �������蹦��GPIO���� */
    HAL_GPIO_DeInit(RS485_USARTx_Tx_GPIO, RS485_USARTx_Tx_GPIO_PIN);
    HAL_GPIO_DeInit(RS485_USARTx_Rx_GPIO, RS485_USARTx_Rx_GPIO_PIN);
    
    /* �����жϽ��� */
    HAL_NVIC_DisableIRQ(RS485_USART_IRQn);
  }
	else if(huart->Instance==USART1)
  {
    /* ��������ʱ�ӽ��� */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /* �������蹦��GPIO���� */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    
    /* �����жϽ��� */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void RS485_USART_Init(void)
{

  husart_485.Instance = RS485_USARTx;
  husart_485.Init.BaudRate = RS485_USARTx_BAUDRATE;
  husart_485.Init.WordLength = UART_WORDLENGTH_9B;
  husart_485.Init.StopBits = UART_STOPBITS_1;
  husart_485.Init.Parity = UART_PARITY_EVEN;
  husart_485.Init.Mode = UART_MODE_TX_RX;
  husart_485.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husart_485.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husart_485);
  
  HAL_NVIC_SetPriority(RS485_USART_IRQn,1,0);
  HAL_NVIC_EnableIRQ(RS485_USART_IRQn);
  
  /* �����ж�,�洢����ʱ������ */ 
  HAL_UART_Receive_IT(&husart_485,&rs485RxBuffer,1);
}
/**
  * ��������: ���ڷ��ͺ���
  * �������: Tx_Bul:�����ַ���,TxCount:���͵��ֽ���
  * �� �� ֵ: ��
  * ˵    ��: ���⺯�������װ.
  */
void UART_Tx(uint8_t *Tx_Buf,uint16_t TxCount)
{
  RS485_TX_MODE();
  HAL_UART_Transmit(&husart_485, Tx_Buf, TxCount, 0xffff);
  while(__HAL_UART_GET_FLAG(&husart_485, UART_FLAG_TXE) == RESET);
  RS485_RX_MODE();
}

//RS485��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void RS485_Receive_Data(uint8_t *buf,uint8_t *len)
{
	uint8_t rxlen=RS485_USARTx_RX_STA;
	uint8_t i=0;
	*len=0;				//Ĭ��Ϊ0
  HAL_Delay(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==RS485_USARTx_RX_STA&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=rs485Rx_Buf[i];	
		}		
		*len=RS485_USARTx_RX_STA;	//��¼�������ݳ���
		RS485_USARTx_RX_STA=0;		//����
	}
} 

/**
  * ��������: NVIC����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void MX_NVIC_USARTx_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void MX_USARTx_Init(void)
{
  /* ��������ʱ��ʹ�� */
  __HAL_RCC_USART1_CLK_ENABLE();
  
  usartx_1.Instance = USART1;
  usartx_1.Init.BaudRate = 115200;
  usartx_1.Init.WordLength = UART_WORDLENGTH_8B;
  usartx_1.Init.StopBits = UART_STOPBITS_1;
  usartx_1.Init.Parity = UART_PARITY_NONE;
  usartx_1.Init.Mode = UART_MODE_TX_RX;
  usartx_1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  usartx_1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&usartx_1);
  
  /* ���ô����жϲ�ʹ�ܣ���Ҫ����HAL_UART_Init������ִ���޸Ĳ���Ч */
  MX_NVIC_USARTx_Init();
}

