#include "io_check.h"
#include "uart.h"
#include "delay.h"
#include "usr_4g_usart.h"


 /**
  * @brief  初始化需要监测的IO口
  * @param  无
  * @retval 无
  */
void IO_check_init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PB口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //PB8端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC
	
	EXTIX_Init();
	
}

 /**
  * @brief  外部中断初始化函数
  * @param  无
  * @retval 无
  */
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
	
	//GPIOB.8 中断线以及中断初始化配置
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);

  EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//上升沿 与 下降沿共同触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/**
  * @brief  外部中断函数
  * @param  无
  * @retval 无
  */
void EXTI9_5_IRQHandler(void)
{			
	char buf[100];
	delay_ms(10);   //消抖			 
	if(FIFO8==0)	
	{
		printf("箱体打开\r\n");
		sprintf(buf,"{\"DataList\":[{\"stat\":\"%d\"}],\"DevID\":\"HYDL%04dEP-AC\"}\r\n",0,2);
		gsm_printf("%s",buf);
	}
	if(FIFO8==1)	
	{
		printf("箱体关闭\r\n");
		sprintf(buf,"{\"DataList\":[{\"stat\":\"%d\"}],\"DevID\":\"HYDL%04dEP-AC\"}\r\n",1,2);
		gsm_printf("%s",buf);
	}
 	 EXTI_ClearITPendingBit(EXTI_Line8);    //清除LINE8上的中断标志位  
}
