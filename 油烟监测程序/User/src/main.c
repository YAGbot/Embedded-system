#include "sys.h"   
#include "string.h"
#include "io_check.h"
#include "uart.h"
#include "delay.h"
#include "rs485.h"
#include "sim7020_usart.h"
#include "sim7020_timer.h"
#include "sim7020.h"
#include "break.h"
#include "timer.h"
//#include "at24cxx.h"


int main(void)
{									
	NVIC_Configuration();	//设置为组2
	IO_check_init();			//IO口检测初始化
	delay_init();
	uart5_init(9600);		//调试串口初始化
	GSM_Init();				//gsm初始化
	delay_ms(3000);
	printf("restart work!!!!!!!!!!!!!!\r\n");
	sim7020_sta();		//sim7020状态初始化		
//	sim7020_mqtt();
	delay_ms(20000);
	TIM3_Init(7199,9999);	
	time3_start();
	while(1)
	{		
		if(worktimes_flag==1)
		{
			time3_stop();
			worktimes_flag=0;
			SIM7020_PUBLISH();
			time3_start();
		}
		if(reset_flag==1)
		{
			__set_FAULTMASK(1);//关闭总中断
			NVIC_SystemReset();//请求单片机重启
		}
	}
}


