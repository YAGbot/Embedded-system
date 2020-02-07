#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "4G_usart.h"

//���ڷ��ͻ����� 	
u8 UART5_TX_BUF[UART5_MAX_SEND_LEN]; 	//���ͻ���,���UART5_MAX_SEND_LEN�ֽ�
u8 UART5_RX_REC_ATCOMMAD;	  
//���ڽ��ջ����� 	 			
u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 				//���ջ���,���UART5_MAX_RECV_LEN���ֽ�.//���ջ���,���UART5_MAX_RECV_LEN���ֽ�.
u16 UART5_RX_STA=0; 
 /**
  * @brief  ��ʼ��gprs usart1	PA9.PA10 
  * @param  ��
  * @retval ��
  */
void gprs_usart_init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//ʹ��UART5��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��GPIOC/Dʱ��
	
  USART_DeInit(UART5);  //��λ����1

		//USART5_TX   PB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PB10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��Pb10
  //UART5_RX	  PB11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��PB11
	
  //Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(UART5, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ���1 
}

/************************************************************************************
* �������ƣ�UART5_IRQHandler 
* ����������UART5�����жϺ���
* ��ڲ�������
* ���ڲ�������
* ˵ 	��:	ʹ��ȫ�ֱ���gu900_uart_buff��������
*************************************************************************************/
void UART5_IRQHandler(void)
{	    
	u8 res;
	if(UART5->SR&(1<<5))//���յ�����
	{	 
		res=UART5->DR;
		if(UART5_RX_STA<UART5_MAX_RECV_LEN)		//�����Խ�������
		{
		  time7_reset();		//��λ��ʱ��7��cnt 
			if(UART5_RX_STA==0)time7_start();		//������ʱ��7
			UART5_RX_BUF[UART5_RX_STA++]=res;		//��¼���յ���ֵ	
		}
		else 
 		{
 			UART5_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
 		} 
	} 
} 

/************************************************************************************
* �������ƣ�UART5_SendDat
* ����������UART5����һ���ַ�����
* ��ڲ�����-dat:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void UART5_SendDat(u8 dat)
{
	while(!(UART5->SR&(1<<6)));			//�ȴ���λ�Ĵ���Ϊ��
	UART5->DR = dat;
}

/************************************************************************************
* �������ƣ�UART5_SendString
* ����������UART5�����ַ�������
* ��ڲ�����-str:Ҫ���͵�����
* ���ڲ�������
*************************************************************************************/
void UART5_SendString(char *str)
{
	while(*str != '\0')
	{
		UART5_SendDat(*str);
		str++;
	}
	
}


//ʵ��fputc����
void gsm_fputc(char str)
{
	UART5_SendDat(str);
}

//ʵ��printf����
void gsm_printf(char *str,...)
{
	int int_d;
	double float_d;
	const char *string_s;
	char buff[20];
	int i;
	
	va_list ap;//�����ε���ʼָ��
	va_start(ap, str);//��ʼָ��ָ��str
	
	while(*str!='\0')
	{
		if(*str=='\\')
		{
			str++;
			switch(*str)
			{
				case 'r':
					UART5_SendDat('\r');
					str++;
					break;
				case 'n':
					UART5_SendDat('\r');
					str++;
					break;
				default:
					str++;
					break;
			}			
		}
		else if(*str=='%')
		{
			str++;
			switch(*str)
			{
				case 'd':
					int_d = va_arg(ap, int);
					sprintf(buff,"%d",int_d);
					for(i=0;i<(strlen(buff));i++)
						gsm_fputc(buff[i]);					
					str++;
					break;
				case 'f':
					float_d = va_arg(ap, double);
					sprintf(buff,"%f",float_d);
					for(i=0;i<(strlen(buff));i++)
						gsm_fputc(buff[i]);					
					str++;
					break;				
				case 's':
					string_s = va_arg(ap, const char *);
					while(*string_s)
					{
						gsm_fputc(*string_s++);
					}
					str++;
					break;
				default:
					str++;
					break;
			}
		}
		else
		{
			gsm_fputc(*str++);
		}
	}
}


/************************************************** *********************************
* �������ƣ�GSM_Init
* ����������gsm��ʼ������
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void GSM_Init(void)
{
	gprs_usart_init(115200);
	TIM7_Init();
}

/************************************************************************************
* �������ƣ�check_buff
* �����������������ݼ�麯��
* ��ڲ�����buff��
* ���ڲ�����0���ɹ� ��-1�����Ӵ�
*************************************************************************************/
int check_buff(u8 *buff, char *str)
{
    //�ַ�������
  if(strstr((const char *)buff,str)!=NULL)
    return 0;
  else
    return -1;
}

/************************************************************************************
* �������ƣ�gprs_check_cmd
* �����������ַ�����������
* ��ڲ�����
* ���ڲ�����
*************************************************************************************/
u8* gprs_check_cmd(u8 *str)
{
	char *strx=0;
	if(UART5_RX_STA&0X8000)		//���յ�һ��������
	{ 
		UART5_RX_BUF[UART5_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)UART5_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//��gprs��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	UART5_RX_STA=0;UART5_RX_REC_ATCOMMAD=1;
	if((u32)cmd<=0XFF)
	{  
		UART5->DR=(u32)cmd;
	}else gsm_printf("%s\r\n",cmd);//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(11);			//������ʱ������ӽ���ȥ�жϣ�
			if(UART5_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(gprs_check_cmd(ack))break;//�õ���Ч���� 
				UART5_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	UART5_RX_STA=0;UART5_RX_REC_ATCOMMAD=0;
	return res;
} 


/************************************************************************************
* �������ƣ�TIME7_Init
* ����������TIME7��ʼ���Լ���ʱ1s����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/	 
void TIM7_Init(void)
{	
	RCC->APB1ENR |= (1<<5);	//��TIME7����ʱ��
  //���ö�ʱ��
  TIM7->CR1 |= (1<<7);	//ʹ��Ӱ�ӼĴ���
  TIM7->CR1 |= (1<<3);	//�������������ֹͣ
  TIM7->CR1 |= (1<<2);	//ֻ�ܼ���������������ж�
  TIM7->CR1 &= ~(1<<1);	//ʹ�ܸ���
  NVIC_SetPriority(TIM7_IRQn,NVIC_EncodePriority(7-2,2,2));//��2��ռ���ȼ�2����Ӧ���ȼ�0
  NVIC_EnableIRQ(TIM7_IRQn);		
	TIM7->DIER |= (1<<0);//ʹ�ܸ����ж�
  TIM7->PSC = 7200-1; 	//����Ԥ��Ƶ
  TIM7->ARR = 100-1;  //�����Զ���װ�� 
  TIM7->EGR |= (1<<0); //������������¼�
}


/************************************************************************************
* �������ƣ�time7_start
* ������������ʱ����ʼ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void time7_start(void)
{
    //ʹ��
    TIM7->CNT = 0;
    TIM7->CR1 |= (1<<0);    
}

/************************************************************************************
* �������ƣ�time7_stop
* ������������ʱ��ֹͣ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void time7_stop(void)
{
    TIM7->CR1 &= ~(1<<0);    
}

/************************************************************************************
* �������ƣ�time7_set_timeout
* �������������ö�ʱ��ʱ�亯��
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void time7_set_timeout(u16 time)
{
    TIM7->ARR = 10 * time;   //ת��1ms�ĵ�λ
}

/************************************************************************************
* �������ƣ�time7_reset
* ������������ʱ����λ����ʱ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void time7_reset(void)
{
    TIM7->CNT = 0;
}

/************************************************************************************
* �������ƣ�TIM7_IRQHandler
* ������������ʱ���жϺ���
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void TIM7_IRQHandler(void)
{
	if(TIM7->SR&0X01)				//�Ǹ����ж�
	{
		TIM7->SR &= ~(1<<0);//�����־λ
		UART5_RX_STA|=1<<15;	//��ǽ������ 0x8000
		time7_stop();	//�رն�ʱ��
		USART1_Send_Data(UART5_RX_BUF,UART5_RX_STA&0x7fff);	//���LOG��Ϣ
//		if(!UART5_RX_REC_ATCOMMAD)	
//		{
//			UART5_RX_BUF[UART5_RX_STA&0X7FFF]=0;//��ӽ�����
//			UART5_RX_STA=0;	
//			if(check_buff(UART5_RX_BUF,"+CMQPUB:")==0)
//			{
//				if(check_buff(UART5_RX_BUF,g_MQTT_Msg.ID)==0)
//				{
//					if(check_buff(UART5_RX_BUF,"&&01")==0)
//					{
//						Flag_Rec_MQTT=1;
//					}
//					if(check_buff(UART5_RX_BUF,"&&02")==0)
//					{
//						rec_ip_config=1;
//					}
//				}	
//			}
//		}
	}
}


