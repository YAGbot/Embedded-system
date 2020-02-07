
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "HMI/bsp_usartx_HMI.h"
#include "usart/bsp_usartx.h"
#include "BasicTIM/bsp_BasicTIM.h"
#include "string.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
uint8_t HMI_USARTx_RX_BUF[HMI_USARTx_REC_LEN];				 //���ջ���,���USART_REC_LEN���ֽ�.
uint8_t HMI_USARTx_TX_BUF[HMI_USARTx_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
__IO uint8_t HMI_USARTx_TX_CNT=0;
__IO uint16_t HMI_USARTx_RX_STA=0;   
UART_HandleTypeDef husartx_HMI;
uint8_t hmiRxBuffer;
const char mima[10]={0x5A,0xA5,0x07,0x82,0x00,0x84,0x5A,0x01,0x00,0x13};	//����ɹ��л�ָ��
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: NVIC����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void MX_NVIC_USARTx_Init(void)
{
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(HMI_USARTx_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(HMI_USARTx_IRQn);
  
}


/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void HMI_USARTx_Init(void)
{ 
  /* ���ô����жϲ�ʹ�ܣ���Ҫ����HAL_UART_Init������ִ���޸Ĳ���Ч */
  MX_NVIC_USARTx_Init();
  
	 /* ��������ʱ��ʹ�� */
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
	
	HMI_USARTx_RX_STA=0;  //����
}

/************************************************************************************
* �������ƣ�UART_SendDat
* ����������UART4����len���ַ�����
* ��ڲ�����-dat:Ҫ���͵����� len:Ҫ�������ݵĳ���
* ���ڲ�������
*************************************************************************************/
void HMI_USARTx_SendDat(uint8_t *buf,uint8_t len)
{
	uint8_t i;
  for(i=0;i<len;i++)		//ѭ����������
	{
		while((USART2->SR&0X40)==0);//�ȴ����ͽ���		  
		USART2->DR=buf[i];
	}	 
}


/************************************************************************************
* �������ƣ�diwen_send_data
* ��������������������ʵʱ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_data(uint16_t _reg,uint32_t _data)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//����ͷ
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x07;	//���͵����ݳ���
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//дָ��
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 24;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 16;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 8;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data;	//д������
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
}

/************************************************************************************
* �������ƣ�diwen_send_data16
* ��������������������ʵʱ����		������
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_data16(uint16_t _reg,int16_t _data)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//����ͷ
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x05;	//���͵����ݳ���
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//дָ��
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data >> 8;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data;	//д������
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
	//USART_Tx(HMI_USARTx_TX_BUF,HMI_USARTx_TX_CNT);
}

/************************************************************************************
* �������ƣ�diwen_send_data16
* ��������������������ʵʱ����		������
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_data8(uint16_t _reg,int8_t _data)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//����ͷ
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x04;	//���͵����ݳ���
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//дָ��
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data;	//д������
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
	USART_Tx(HMI_USARTx_TX_BUF,HMI_USARTx_TX_CNT);
}

/************************************************************************************
* �������ƣ�diwen_send_data16
* ��������������������ʵʱ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_asci3(uint16_t _reg,uint16_t _data1,uint16_t _data2,uint16_t _data3)
{
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//����ͷ
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x09;	//���͵����ݳ���
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//дָ��
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data1 >> 8;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data1;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data2 >> 8;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data2;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data3 >> 8;	//д������
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _data3;	//д������
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
}

/************************************************************************************
* �������ƣ�diwen_send_data
* ����������������������ĸ����ASCII��
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void diwen_send_numascii(uint16_t _reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	HMI_USARTx_TX_CNT=0;
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x5A;		
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0xA5;		//����ͷ
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = len+3;	//���͵����ݳ���
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = 0x82;	//дָ��
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg>>8;		//�Ĵ������ ���ֽ�
	HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	for(i=0;i<len;i++)
	{
		HMI_USARTx_TX_BUF[HMI_USARTx_TX_CNT++]=buf[i];
	}
	HMI_USARTx_SendDat(HMI_USARTx_TX_BUF, HMI_USARTx_TX_CNT);
}

void check_IMI_cmd(void)
{ 
	if(HMI_USARTx_RX_STA&0X8000)	//���յ�һ��������
	{
		HMI_USARTx_RX_STA=0;			//��־��ʼ��	 
		Data_Scan();
	}		
}

/************************************************************************************
* �������ƣ�Data_Scan
* ������������������������������		������24c02��ַ ��30 40 50 60 100 150(1 2 5 6 3 4) 
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void Data_Scan(void)
{
	uint8_t buf[30];
	char d_buf[50];
	uint16_t addr;
	uint8_t len;
	if(HMI_USARTx_RX_BUF[7]==0x5A)	//���յ�ASCII����
	{
		len=HMI_USARTx_RX_BUF[8];			//��λ�ô�ŵ��ǽ��յ�ASCII�����ݳ���
		printf("len=%d\r\n",len);
		addr=((uint16_t)HMI_USARTx_RX_BUF[4]<<8)|HMI_USARTx_RX_BUF[5];		//�õ���ַ
		switch(addr)
		{
			case 0x34ff:				//������1
				memset(buf,0,30);					//��ջ���
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				printf("������1=%s",buf);
				break;
			
			case 0x44ff:				//������2
				memset(buf,0,30);					//��ջ���
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				break;
			
			case 0x350f:				//��ѹ��1
				memset(buf,0,30);					//��ջ���
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				printf("��ѹ��1=%s",buf);
				break;
			
			case 0x450f:				//��ѹ��1
				memset(buf,0,30);					//��ջ���
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				printf("��ѹ��2=%s",buf);
				break;
			
			case 0x351f:				//ip1
				memset(buf,0,30);					//��ջ���
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				printf("ip1=%s",buf);
				break;
			
			case 0x35ff:				//ip2
				memset(buf,0,30);					//��ջ���
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				break;
			case 0x31ff:				//����
				memset(buf,0,30);					//��ջ���
				memcpy(buf,HMI_USARTx_RX_BUF+9,len);	//��ASCII��ֵ���뻺��
				sprintf(d_buf,"%s",buf);
				if(strcmp(d_buf,"666666")==0)
				{
					printf("����ƥ��\r\n");
					HMI_USARTx_SendDat((uint8_t*)mima,10);
					HAL_Delay(20);
					HMI_USARTx_RX_STA=0;			//��־��ʼ��
				}
				printf("����=%s\r\n",d_buf);
				break;
		default: break;				
	}
	}
}
