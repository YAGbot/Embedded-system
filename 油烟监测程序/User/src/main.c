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
	NVIC_Configuration();	//����Ϊ��2
	IO_check_init();			//IO�ڼ���ʼ��
	delay_init();
	uart5_init(9600);		//���Դ��ڳ�ʼ��
	GSM_Init();				//gsm��ʼ��
	delay_ms(3000);
	printf("restart work!!!!!!!!!!!!!!\r\n");
	sim7020_sta();		//sim7020״̬��ʼ��		
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
			__set_FAULTMASK(1);//�ر����ж�
			NVIC_SystemReset();//����Ƭ������
		}
	}
}


