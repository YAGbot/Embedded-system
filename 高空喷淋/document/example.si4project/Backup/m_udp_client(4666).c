/**
  ******************************************************************************
  * @file    m_dns.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   DNS 
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "socket.h"
# include "dns.h"
# include "oled.h"
# include "spi.h"
# include "rtc.h"
# include "tmp.h"
# include <string.h>
# include <stdio.h>


u8 socket_buf[1024] = {0};
u16 socket_rxlen = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 lip[4] = {192, 168, 0, 101};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

#define PING "192.168.3.14"
u8 rip[4] = {192, 168, 3, 14};


s16 y_pos = -12;
u16 timeout = 0;
u8 status = 0;
u8 ping_status = 0;
char temp[100];

int main(void)
{
	 /*clk_init();        ��ʼ��ϵͳʱ��72M �Ѿ��������ļ���
    ��ͨ�������ó�ʼ����ɲ���Ҫ�ڶ���ĳ�ʼ�� */
	delay_init();		/* ��ʱ��ʼ�� */
	uart1_init(115200);	/* UART1��ʼ�� */
	rtc_init();			/* rtcʱ�ӳ�ʼ�� */
	tmp_init();			/* �ڲ��¶ȳ�ʼ�� */
	spi1_init();		/* SPI1 ��ʼ�� */
	oled_init();		/* OLED��ʼ�� */
	w5500_init();		/* оƬ��ʼ�� */

	print( 	"******************************************\r\n"
			"*************������о����*****************\r\n"
			"*********W5500������W5500 PING HOST********\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	


//	printf("UDP Local Port: %d \r\n",mikro.udp_client.lport); 
//	printf("UDP Remote IP: %d.%d.%d.%d \r\n",mikro.udp_client.rip[0],mikro.udp_client.rip[1],mikro.udp_client.rip[2],mikro.udp_client.rip[3]);
//	printf("UDP Remote Port: %d \r\n",mikro.udp_client.rport);
//	printf("W5500 Init Complete!\r\n");
//	printf("Start UDP Test!\r\n");	


	sprintf(temp, "ping:%s", PING);
	oled_str(0,y_pos+=12,(u8*)temp, 12);
	oled_update();/* oled �Դ������� */		

	while(1)
	{		
		print("temp:%.1lf\r\n", get_tmp()/100.0);

		
		if(status == 0)/* DHCP */
		{
			switch(check_DHCP_state(SOCK_DHCP))
			{
				case DHCP_RET_NONE: 					
				break;
				case DHCP_RET_TIMEOUT:	
					print("dhcp timeout!\r\n"); 
					oled_str(0,y_pos+=12,"dhcp timeout!", 12);
					oled_update();/* oled �Դ������� */						
				break;
				case DHCP_RET_UPDATE:					
					print(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
					DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],
					DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
					DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
					w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
					close(SOCK_DHCP);			
					status = 0X80;
					oled_str(0,y_pos+=12,"dhcp ok!", 12);
					oled_update();/* oled �Դ������� */				
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP ��ʱ��ʱ�� */		
			
			delay_ms(1000);/* 100msɨ��һ�� */
		}

		if((status&0XC0) == 0X80)
		{

			delay_ms(1000);/* 1000msɨ��һ�� */
		}	
	}
	 
}



