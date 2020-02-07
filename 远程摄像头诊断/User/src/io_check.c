#include "io_check.h"
#include "uart.h"
#include "sim7020.h"

u8 sta1,sta2,sta3;

 /**
  * @brief  初始化需要监测的IO口
  * @param  无
  * @retval 无
  */
void IO_check_init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PC口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;				 //PC0,1,2端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC
}

 /**
  * @brief  io口状态检测函数
  * @param  无
  * @retval 无
  */
u8 io_init(void)
{
	if(FIFO1==0)
	{
		printf("FIFO1通电\r\n");
		sta1=1;
	}
	else sta1=0;
	if(FIFO2==0)
	{
		printf("FIFO2通电\r\n");
		sta2=1;
	}
	else sta2=0;
	if(FIFO3==0)
	{
		printf("FIFO3通电\r\n");
		sta3=1;
	}
	else sta3=0;
	
	if((sta1||sta2||sta3)==0)
	{
		printf("没有检测220V\r\n");
		g_MQTT_Msg.power_sta=0;
		return 0;
	} 
	return 1;
}

 /**
  * @brief  io口电平检测函数
  * @param  无
  * @retval 无
  */
void io_poll(void)
{
	u8 power_sta=0xff;
	if(sta1)
	{
		if(FIFO1==1)
		{	
			printf("FIFO1断电\r\n");
			power_sta &= 0<<1;
		}
	}
	if(sta2)
	{
		if(FIFO2==1)
		{
			printf("FIFO2断电\r\n");
			power_sta &= 0<<2;
		}
	}
	if(sta3)
	{
		if(FIFO3==1)
		{
			printf("FIFO3断电\r\n");
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
