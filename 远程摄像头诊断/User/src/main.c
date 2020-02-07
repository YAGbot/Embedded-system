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
#include "ping.h"
#include "mw5500.h"
#include "timer.h"


static	IP_SETType IP_Set = { 
												.Camera1 = {17,13,243,26},
												.Camera2 = {17,13,243,27},
												.Camera3 = {17,13,243,28},
												.Camera4 = {0,0,0,0},
												.Camera5 = {0,0,0,0},
												.Camera6 = {0,0,0,0},
												.Camera7 = {0,0,0,0},
												.NVR     = {0,0,0,0},
											};

int main(void)
{
	uint8_t Ping_Reply = 0;
	uint8_t Router_IP[4] = {17,13,243,1};
	u8 ele_sta = 0;
											
	NVIC_Configuration();	//设置为组2
	IO_check_init();			//IO口检测初始化
	delay_init();
	uart5_init(9600);		//调试串口初始化
	Control_init();			//远程断路器初始化
	GSM_Init();				//sim7020初始化
	delay_ms(3000);
	ele_sta=io_init();					//通电状态初始化
	W5500_Init();
	delay_ms(3000);
	sim7020_sta();		//sim7020状态初始化		
	sim7020_mqtt();
	TIM3_Init(7199,9999);	
	time3_start();	
	while(1)
	{
		if(Flag_Rec_MQTT==1)
		{
			time3_stop();
			Flag_Rec_MQTT=0;
			printf("重启\r\n");
			Restart_break();		//检测到远程命令
			time3_start();
		}
		
		if(reboot_mqtt==1)
		{
			time3_stop();
			reboot_mqtt=0;
			send_Msgflag=0;
			sim7020_mqtt();			//断网，重启模块
			time3_start();
		}
		
		if(worktimes_flag==1)
		{
			time3_stop();
			worktimes_flag=0;
			Ping_Reply = Ping_Multiple(&IP_Set);				//video
			if(Ping_Reply==0xff)
			{
				g_MQTT_Msg.video_sta=1;
			}
			else g_MQTT_Msg.video_sta=0;
			Ping_NetCheck(&Net_Info, Router_IP, 10);
			printf("Ping Loss Rate:%d%%\r\n", Net_Info.Loss_Rate);
			
			if(ele_sta)
			{
				io_poll();
			}
			delay_ms(50);
			
			SIM7020_Sed_CMQPUB();		//循环发布
			delay_ms(50);
			time3_start();
		}
		
	}
}


