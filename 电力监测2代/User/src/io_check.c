#include "io_check.h"
#include "uart.h"
#include "delay.h"
#include "usr_4g_usart.h"


 /**
  * @brief  ��ʼ����Ҫ����IO��
  * @param  ��
  * @retval ��
  */
void IO_check_init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PB��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //PB8�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC
	
	EXTIX_Init();
	
}

 /**
  * @brief  �ⲿ�жϳ�ʼ������
  * @param  ��
  * @retval ��
  */
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��
	
	//GPIOB.8 �ж����Լ��жϳ�ʼ������
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);

  EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//������ �� �½��ع�ͬ����
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

/**
  * @brief  �ⲿ�жϺ���
  * @param  ��
  * @retval ��
  */
void EXTI9_5_IRQHandler(void)
{			
	char buf[100];
	delay_ms(10);   //����			 
	if(FIFO8==0)	
	{
		printf("�����\r\n");
		sprintf(buf,"{\"DataList\":[{\"stat\":\"%d\"}],\"DevID\":\"HYDL%04dEP-AC\"}\r\n",0,2);
		gsm_printf("%s",buf);
	}
	if(FIFO8==1)	
	{
		printf("����ر�\r\n");
		sprintf(buf,"{\"DataList\":[{\"stat\":\"%d\"}],\"DevID\":\"HYDL%04dEP-AC\"}\r\n",1,2);
		gsm_printf("%s",buf);
	}
 	 EXTI_ClearITPendingBit(EXTI_Line8);    //���LINE8�ϵ��жϱ�־λ  
}
