
/* 包含头文件 ----------------------------------------------------------------*/
#include "HMI/bsp_usartx_HMI.h"
#include "usart/bsp_usartx.h"
#include "BasicTIM/bsp_BasicTIM.h"
#include "string.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
uint8_t HMI_USARTx_RX_BUF[HMI_USARTx_REC_LEN];				 //接收缓冲,最大USART_REC_LEN个字节.
uint8_t HMI_USARTx_TX_BUF[HMI_USARTx_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
__IO uint8_t HMI_USARTx_TX_CNT=0;
__IO uint16_t HMI_USARTx_RX_STA=0;   
UART_HandleTypeDef husartx_HMI;
uint8_t hmiRxBuffer;
const char mima[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x13};	//密码成功切换指令
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: NVIC配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static void MX_NVIC_USARTx_Init(void)
{
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(HMI_USARTx_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(HMI_USARTx_IRQn);
  
}


/**
  * 函数功能: 串口参数配置.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void HMI_USARTx_Init(void)
{ 
  /* 配置串口中断并使能，需要放在HAL_UART_Init函数后执行修改才有效 */
  MX_NVIC_USARTx_Init();
  
	 /* 串口外设时钟使能 */
  HMI_USART_RCC_CLK_ENABLE();
  
  husartx_HMI.Instance = HMI_USARTx;
  husartx_HMI.Init.BaudRate = HMI_USARTx_BAUDRATE;
  husartx_HMI.Init.WordLength = UART_WORDLENGTH_8B;
  husartx_HMI.Init.StopBits = UART_STOPBITS_1;
  husartx_HMI.Init.Parity = UART_PARITY_NONE;
  husartx_HMI.Init.Mode = UART_MODE_TX_RX;
  husartx_HMI.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx_HMI.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husartx_HMI);
	
	HMI_USARTx_RX_STA=0;  //清零
}

/************************************************************************************
* 函数名称：UART_SendDat
* 功能描述：UART4发送len个字符函数
* 入口参数：-dat:要发送的数据 len:要发送数据的长度
* 出口参数：无
*************************************************************************************/
void HMI_USARTx_SendDat(uint8_t *buf,uint8_t len)
{
	uint8_t i;
  for(i=0;i<len;i++)		//循环发送数据
	{
		while((USART2->SR&0X40)==0);//等待发送结束		  
		USART2->DR=buf[i];
	}	 
}


/************************************************************************************
* 函数名称：diwen_send_data
* 功能描述：迪文屏发送实时数据
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_data(uint16_t _reg,uint32_t _data)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//数据头
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x07;	//发送的数据长度
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//写指令
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//寄存器编号 低字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 24;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 16;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 8;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data;	//写入数据
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
}

/************************************************************************************
* 函数名称：diwen_send_data16
* 功能描述：迪文屏发送实时数据		带符号
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_data16(uint16_t _reg,int16_t _data)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//数据头
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x05;	//发送的数据长度
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//写指令
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//寄存器编号 低字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 8;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data;	//写入数据
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
	//USART_Tx(HMI_USARTx_TX_BUF,HMI_USARTx_TX_CNT);
}

/************************************************************************************
* 函数名称：diwen_send_data16
* 功能描述：迪文屏发送实时数据		带符号
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_data8(uint16_t _reg,int8_t _data)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//数据头
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x04;	//发送的数据长度
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//写指令
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//寄存器编号 低字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data;	//写入数据
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
	USART_Tx(HMI_USARTx_TX_BUF,HMI_USARTx_TX_CNT);
}

/************************************************************************************
* 函数名称：diwen_send_data16
* 功能描述：迪文屏发送实时数据
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_asci3(uint16_t _reg,uint16_t _data1,uint16_t _data2,uint16_t _data3)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//数据头
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x09;	//发送的数据长度
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//写指令
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//寄存器编号 低字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data1 >> 8;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data1;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data2 >> 8;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data2;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data3 >> 8;	//写入数据
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data3;	//写入数据
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
}

/************************************************************************************
* 函数名称：diwen_send_data
* 功能描述：迪文屏发送字母数字ASCII码
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void diwen_send_numascii(uint16_t _reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//数据头
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = len+3;	//发送的数据长度
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//写指令
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//寄存器编号 高字节
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//寄存器编号 低字节
	for(i=0;i<len;i++)
	{
		HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++]=buf[i];
	}
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
}

void check_IMI_cmd(void)
{ 
	if(HMI_USARTx_RX_STA&0X8000)	//接收到一次数据了
	{
		HMI_USARTx_RX_STA=0;			//标志初始化	 
		Data_Scan();
	}		
}

/************************************************************************************
* 函数名称：Data_Scan
* 功能描述：迪文屏触摸参数处理函数		保存至24c02地址 ：30 40 50 60 100 150(1 2 5 6 3 4) 
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void Data_Scan(void)
{
	uint8_t buf[30];
	char d_buf[50];
	uint16_t addr;
	uint8_t len;
	if(HMI_USARTx_RX_BUF[7]==0x5A)	//接收到ASCII码了
	{
		len=HMI_USARTx_RX_BUF[8];			//该位置存放的是接收的ASCII码数据长度
		printf("len=%d\r\n",len);
		addr=((uint16_t)HMI_USARTx_RX_BUF[4]<<8)|HMI_USARTx_RX_BUF[5];		//得到地址
		switch(addr)
		{
			case 0x34ff:				//电流比1
				memset(buf,0,30);					//清空缓存
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//将ASCII码值存入缓存
				printf("电流比1=%s",buf);
				break;
			
			case 0x44ff:				//电流比2
				memset(buf,0,30);					//清空缓存
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//将ASCII码值存入缓存
				break;
			
			case 0x350f:				//电压比1
				memset(buf,0,30);					//清空缓存
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//将ASCII码值存入缓存
				printf("电压比1=%s",buf);
				break;
			
			case 0x450f:				//电压比1
				memset(buf,0,30);					//清空缓存
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//将ASCII码值存入缓存
				printf("电压比2=%s",buf);
				break;
			
			case 0x351f:				//ip1
				memset(buf,0,30);					//清空缓存
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//将ASCII码值存入缓存
				printf("ip1=%s",buf);
				break;
			
			case 0x35ff:				//ip2
				memset(buf,0,30);					//清空缓存
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//将ASCII码值存入缓存
				break;
			case 0x31ff:				//密码
				memset(buf,0,30);					//清空缓存
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//将ASCII码值存入缓存
				sprintf(d_buf,"%s",buf);
				if(strcmp(d_buf,"666666")==0)
				{
					printf("密码匹配\r\n");
					HMI_USARTx_SendDat((uint8_t*)mima,10);
					HAL_Delay(20);
					HMI_USARTx_RX_STA=0;			//标志初始化
				}
				printf("密码=%s\r\n",d_buf);
				break;
		default: break;				
	}
	}
}
