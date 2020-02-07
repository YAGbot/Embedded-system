/**
  ******************************************************************************
  * @file    m_ping.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   PING ����
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "socket.h"
# include "oled.h"
# include "spi.h"
# include "ping.h"
# include <string.h>
# include <stdio.h>


u8 socket_buf[1024] = {0};
u16 socket_rxlen = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 lip[4] = {192, 168, 0, 101};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

#define PING "192.168.0.101"
u8 rip[4] = {192, 168, 0, 163};


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
	spi1_init();		/* SPI1 ��ʼ�� */
	oled_init();		/* OLED��ʼ�� */
	w5500_init();		/* оƬ��ʼ�� */

	print( 	"******************************************\r\n"
			"*************������о����*****************\r\n"
			"*********W5500������W5500 PING ***********\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	

	sprintf(temp, "ping:%s", PING);
	oled_add_str(temp);
	
	while(1)
	{		
		if(status == 0)/* DHCP */
		{
			switch(check_DHCP_state(SOCK_DHCP))
			{
				case DHCP_RET_NONE: 					
				break;
				case DHCP_RET_TIMEOUT:	
					print("dhcp timeout!\r\n"); 
					oled_add_str("DHCP TIMETOUT");					
				break;
				case DHCP_RET_UPDATE:					
					print(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
					DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],
					DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
					DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
					w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
					close(SOCK_DHCP);			
					status = 0X80;
					oled_add_str("DHCP OK");							
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP ��ʱ��ʱ�� */		
			
			delay_ms(10);/* 10msɨ��һ�� */
		}

		if((status&0XC0) == 0X80)
		{
			switch(getSn_SR(SOCK_PING))																			/*��ȡsocket״̬*/
			{
				case SOCK_CLOSED:/*socket�ر�״̬*/					
					/* Create Socket */  
					IINCHIP_WRITE(Sn_PROTO(SOCK_PING), IPPROTO_ICMP);/*����ICMP Э��*/
					if(socket(SOCK_PING,Sn_MR_IPRAW,3000,0) != 0)	/*�ж�ip rawģʽsocket�Ƿ���*/
					{						
					}
					/* Check socket register */
					while(getSn_SR(SOCK_PING) != SOCK_IPRAW);		
					ping_status = 0;
					timeout = 0;
				break;
				case SOCK_IPRAW:/*ip rawģʽ*/		

					if(ping_status == 0)
					{
						ping_request(SOCK_PING, rip);/*����Ping����*/
						ping_status = 0X80;
						oled_add_str("PING...");						
						print("ping!\r\n");
					}
			
					if((socket_rxlen = getSn_RX_RSR(SOCK_PING)) > 0)
					{
						u8 res = ping_reply(SOCK_PING, rip, socket_rxlen);/*��ȡ�ظ���Ϣ*/
						if (res)  
						{
							status = 0XFF;								
							oled_add_str("PING HOST OK");															
							close(0);											
						}
					}

					if(timeout++ > 30)
					{					
						timeout = 0;
						ping_status = 0;
						status = 0XFF;
						oled_add_str("PING HOST FAIL!");					
					}

				break;	
				default:					
				break;
			}			
			

			delay_ms(1000);/* 1000msɨ��һ�� */
		}	
	}
	 
}



