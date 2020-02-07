/**
  ******************************************************************************
  * ��    ��: GSM���ڵײ���������
  ******************************************************************************
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "usart/bsp_gsm_usart.h"
#include <stdarg.h>

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
UART_HandleTypeDef husartx_GSM;

/* ��չ���� ------------------------------------------------------------------*/
//�жϻ��洮������
__IO  uint16_t gsmuart_p = 0;
uint8_t   gsm_uart_buff[GSM_UART_BUFF_SIZE];
uint8_t gsmRxBuffer;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: GSMͨ�Ź�������GPIO��ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void GSM_GPIO_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;

    /* ��������ʱ��ʹ�� */
    GSM_USARTx_GPIO_ClK_ENABLE();
    
    /* �������蹦��GPIO���� */
    GPIO_InitStruct.Pin = GSM_USARTx_Tx_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GSM_USARTx_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GSM_USARTx_Rx_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GSM_USARTx_PORT, &GPIO_InitStruct);
    
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void GSM_USARTx_Init(void)
{ 
  /* GSMͨ�Ź�������GPIO��ʼ�� */
  GSM_GPIO_Init();
  GSM_USARTx_RCC_CLK_ENABLE();
  husartx_GSM.Instance = GSM_USARTx;
  husartx_GSM.Init.BaudRate = GSM_USARTx_BAUDRATE;
  husartx_GSM.Init.WordLength = UART_WORDLENGTH_8B;
  husartx_GSM.Init.StopBits = UART_STOPBITS_1;
  husartx_GSM.Init.Parity = UART_PARITY_NONE;
  husartx_GSM.Init.Mode = UART_MODE_TX_RX;
  husartx_GSM.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husartx_GSM.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&husartx_GSM);
  
  HAL_NVIC_SetPriority(UART4_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(UART4_IRQn);
  
}

/**
  * ��������: ����������ת�����ַ���
  * �������: radix =10 ��ʾ10���ƣ��������Ϊ0
  *           value Ҫת����������
  *           buf ת������ַ���
  *           radix = 10
  * �� �� ֵ: ��
  * ˵    ������
  */
static char *itoa(int value, char *string, int radix)
{
  int     i, d;
  int     flag = 0;
  char    *ptr = string;

  /* This implementation only works for decimal numbers. */
  if (radix != 10)
  {
    *ptr = 0;
    return string;
  }
  if (!value)
  {
    *ptr++ = 0x30;
    *ptr = 0;
    return string;
  }
  /* if this is a negative value insert the minus sign. */
  if (value < 0)
  {
    *ptr++ = '-';
    /* Make the value positive. */
    value *= -1;
  }
  for (i = 10000; i > 0; i /= 10)
  {
    d = value / i;
    if (d || flag)
    {
      *ptr++ = (char)(d + 0x30);
      value -= (d * i);
      flag = 1;
    }
  }
  /* Null terminate the string. */
  *ptr = 0;
  return string;
} /* NCL_Itoa */

/**
  * ��������: ���ڷ���һ���ֽ����� 
  * �������: ch���������ַ�
  * �� �� ֵ: ��
  * ˵    ������
  */
void Usart_SendByte(uint8_t ch )
{
  	while(__HAL_UART_GET_FLAG(&husartx_GSM,UART_FLAG_TXE)==0); //ѭ������,ֱ���������
	/* ����һ���ֽ����ݵ�USART2 */
	HAL_UART_Transmit(&husartx_GSM,(uint8_t *)&ch,1,0xffff);
		
}



/**
  * ��������: ��ȡ���յ������ݺͳ��� 
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
char *get_rebuff(uint8_t *len) 
{
    *len = gsmuart_p;
    return (char *)&gsm_uart_buff;
}

/**
  * ��������: ��ջ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void clean_rebuff(void)
{
  uint16_t i=256;
  
  gsmuart_p = 0;
	while(i)
		gsm_uart_buff[--i]=0;
}

/**
  * ��������: ��ʽ�������������C���е�printf��������û���õ�C��
  * �������: USARTx ����ͨ���������õ��˴���2����USART2
  *		        Data   Ҫ���͵����ڵ����ݵ�ָ��
  *			      ...    ��������
  * �� �� ֵ: ��
  * ˵    ��������Ӧ��
  *           GSM_USART_printf( USART2, "\r\n this is a demo \r\n" );
  *           GSM_USART_printf( USART2, "\r\n %d \r\n", i );
  *           GSM_USART_printf( USART2, "\r\n %s \r\n", j );
  */
void GSM_USART_printf( char *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					Usart_SendByte(0x0d);
					Data ++;
					break;
				case 'n':							          //���з�
					Usart_SendByte( 0x0a);	
					Data ++;
					break;				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						Usart_SendByte(*s);
						while( __HAL_UART_GET_FLAG(&husartx_GSM, UART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
        case 'd':										//ʮ����
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						Usart_SendByte(*s);
						while( __HAL_UART_GET_FLAG(&husartx_GSM, UART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else Usart_SendByte(*Data++);
		while( __HAL_UART_GET_FLAG(&husartx_GSM, UART_FLAG_TXE) == RESET );
	}
}


/**
  * ��������: ��GSM����һ���������֤����ֵ�Ƿ���ȷ
  * �������: cmd������
  *           replay���ڴ�����ֵ
  *           waittime���ȴ�ʱ��
  * �� �� ֵ: GSM_TRUE���ɹ�
  *           GSM_FALSE��ʧ��
  * ˵    ������
  */
uint8_t GSM_cmd(char *cmd, char *reply,uint32_t waittime )
{    
  clean_rebuff();                 //����˽��ջ���������
  GSM_USART_printf("%s",cmd);                    //��������
  if(reply == 0)                      //����Ҫ��������
  {
    return GSM_TRUE;
  }
  HAL_Delay(waittime);                 //��ʱ  
  return GSM_cmd_check(reply);    //�Խ������ݽ��д���
}

/**
  * ��������: ��֤����ķ���ֵ�Ƿ���ȷ
  * �������: replay���ڴ�����ֵ
  * �� �� ֵ: GSM_TRUE���ɹ�
  *           GSM_FALSE��ʧ��
  * ˵    ������
  */
uint8_t GSM_cmd_check(char *reply)
{
  uint8_t len;
  uint8_t n;
  uint8_t off;
  char *redata;
  
  redata = get_rebuff(&(len)); //��������
  n = 0;
  off = 0;
  while((n + off)<len)
  {
    if(reply[n] == 0) //����Ϊ�ջ��߱Ƚ����
    {
      return GSM_TRUE;
    }
    if(redata[ n + off]== reply[n])
    {
      n++; //�ƶ�����һ����������
    }
    else
    {
      off++; //������һ��ƥ��
      n=0; //����
    }
    //n++;
  }
  if(reply[n]==0) //�պ�ƥ�����
  {
     return GSM_TRUE;
  }  
  return GSM_FALSE; //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
}

/**
  * ��������: �ȴ�GSM������Ӧ��
  * �������: ��
  * �� �� ֵ: Ӧ�����ݴ�
  * ˵    ������
  */
char * GSM_waitask(uint8_t waitask_hook(void))
{
  uint8_t len=0;
  char *redata;
  do
  {    
    redata = get_rebuff(&(len));   //��������
    if(waitask_hook!=0)
    {
      if(waitask_hook()==GSM_TRUE) //���� GSM_TRUE ��ʾ��⵽�¼�����Ҫ�˳�
      {
        redata = 0;
        return redata;               
      }
    }
  }while(len==0);                 //��������Ϊ0ʱһֱ�ȴ�  
  HAL_Delay(20);              //��ʱ��ȷ���ܽ��յ�ȫ�����ݣ�115200�������£�ÿms�ܽ���11.52���ֽڣ�
  return redata;
}
