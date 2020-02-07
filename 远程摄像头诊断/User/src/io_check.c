#include "io_check.h"
#include "uart.h"
#include "sim7020.h"

u8 sta1,sta2,sta3;

 /**
  * @brief  ��ʼ����Ҫ����IO��
  * @param  ��
  * @retval ��
  */
void IO_check_init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PC��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;				 //PC0,1,2�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC
}

 /**
  * @brief  io��״̬��⺯��
  * @param  ��
  * @retval ��
  */
u8 io_init(void)
{
	if(FIFO1==0)
	{
		printf("FIFO1ͨ��\r\n");
		sta1=1;
	}
	else sta1=0;
	if(FIFO2==0)
	{
		printf("FIFO2ͨ��\r\n");
		sta2=1;
	}
	else sta2=0;
	if(FIFO3==0)
	{
		printf("FIFO3ͨ��\r\n");
		sta3=1;
	}
	else sta3=0;
	
	if((sta1||sta2||sta3)==0)
	{
		printf("û�м��220V\r\n");
		g_MQTT_Msg.power_sta=0;
		return 0;
	} 
	return 1;
}

 /**
  * @brief  io�ڵ�ƽ��⺯��
  * @param  ��
  * @retval ��
  */
void io_poll(void)
{
	u8 power_sta=0xff;
	if(sta1)
	{
		if(FIFO1==1)
		{	
			printf("FIFO1�ϵ�\r\n");
			power_sta &= 0<<1;
		}
	}
	if(sta2)
	{
		if(FIFO2==1)
		{
			printf("FIFO2�ϵ�\r\n");
			power_sta &= 0<<2;
		}
	}
	if(sta3)
	{
		if(FIFO3==1)
		{
			printf("FIFO3�ϵ�\r\n");
			power_sta &= 0<<3;
		}
	}
	
	if(power_sta==0xff)
	{
		g_MQTT_Msg.power_sta=1;
	}
	else 
		g_MQTT_Msg.power_sta=0;
		
}
