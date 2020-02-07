/**
  ******************************************************************************
  485串口程序 USART3
  ******************************************************************************
  */
#include "usart/bsp_485_usart.h"
#include "HMI/bsp_usartx_HMI.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
UART_HandleTypeDef husart_485;
UART_HandleTypeDef usartx_1;
uint8_t rs485Rx_Buf[256];            // 接收缓存,最大256字节
uint8_t rs485Tx_Buf[256];            // 发送缓存,最大256字节
uint8_t rs485RxBuffer;
__IO uint16_t RS485_USARTx_RX_STA=0;

uint16_t USART1_RX_STA=0;
uint8_t aRxBuffer;
uint8_t usart1Rx_Buf[256];
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 串口硬件初始化配置
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  if(huart->Instance==RS485_USARTx)
  {
    /* 串口外设时钟使能 */
    RS485_USARTx_GPIO_ClK_ENABLE();
    RS485_USART_RCC_CLK_ENABLE();
    RS485_REDE_GPIO_ClK_ENABLE();
    /* 串口外设功能GPIO配置 */
    GPIO_InitStruct.Pin = RS485_USARTx_Tx_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(RS485_USARTx_Tx_GPIO, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = RS485_USARTx_Rx_GPIO_PIN;
    HAL_GPIO_Init(RS485_USARTx_Rx_GPIO, &GPIO_InitStruct);
       
    /* SP3485E发送数据使能控制引脚初始化 */
    HAL_GPIO_WritePin(RS485_REDE_PORT,RS485_REDE_PIN,GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = RS485_REDE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RS485_REDE_PORT, &GPIO_InitStruct);
    
  }  
	else if(huart->Instance==HMI_USARTx)
	{
	 GPIO_InitTypeDef GPIO_InitStruct;
    /* 使能串口功能引脚GPIO时钟 */
   HMI_USARTx_GPIO_ClK_ENABLE();
  
   /* 串口外设功能GPIO配置 */
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
    /* 使能串口功能引脚GPIO时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /* 串口外设功能GPIO配置 */
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
  * 函数功能: 串口硬件反初始化配置
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==RS485_USARTx)
  {
    /* 串口外设时钟禁用 */
    RS485_USART_RCC_CLK_DISABLE();
  
    /* 串口外设功能GPIO配置 */
    HAL_GPIO_DeInit(RS485_USARTx_Tx_GPIO, RS485_USARTx_Tx_GPIO_PIN);
    HAL_GPIO_DeInit(RS485_USARTx_Rx_GPIO, RS485_USARTx_Rx_GPIO_PIN);
    
    /* 串口中断禁用 */
    HAL_NVIC_DisableIRQ(RS485_USART_IRQn);
  }
	else if(huart->Instance==USART1)
  {
    /* 串口外设时钟禁用 */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /* 串口外设功能GPIO配置 */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    
    /* 串口中断禁用 */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
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
  
  /* 开启中断,存储在临时缓存中 */ 
  HAL_UART_Receive_IT(&husart_485,&rs485RxBuffer,1);
}
/**
  * 函数功能: 串口发送函数
  * 输入参数: Tx_Bul:发送字符串,TxCount:发送的字节数
  * 返 回 值: 无
  * 说    明: 将库函数对外封装.
  */
void UART_Tx(uint8_t *Tx_Buf,uint16_t TxCount)
{
  RS485_TX_MODE();
  HAL_UART_Transmit(&husart_485, Tx_Buf, TxCount, 0xffff);
  while(__HAL_UART_GET_FLAG(&husart_485, UART_FLAG_TXE) == RESET);
  RS485_RX_MODE();
}

//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(uint8_t *buf,uint8_t *len)
{
	uint8_t rxlen=RS485_USARTx_RX_STA;
	uint8_t i=0;
	*len=0;				//默认为0
  HAL_Delay(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RS485_USARTx_RX_STA&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=rs485Rx_Buf[i];	
		}		
		*len=RS485_USARTx_RX_STA;	//记录本次数据长度
		RS485_USARTx_RX_STA=0;		//清零
	}
} 

/**
  * 函数功能: NVIC配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void MX_NVIC_USARTx_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void MX_USARTx_Init(void)
{
  /* 串口外设时钟使能 */
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
  
  /* 配置串口中断并使能，需要放在HAL_UART_Init函数后执行修改才有效 */
  MX_NVIC_USARTx_Init();
}

