/**
  ******************************************************************************
  * @file    m_arp.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   ARP 获取物理MAC
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "socket.h"
# include "arp.h"
# include "oled.h"
# include "spi.h"
# include <string.h>


extern ARPMSG * aARPMSG;

u8 socket_buf[1024] = {0};
u16 socket_rxlen = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 rip[4]= {192,168,0,101};/* 远程IP */
u16 rport = 10086;/* 远程端口 */

u8 lip[4] = {192, 168, 0, 103};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

int main(void)
{
	u8 status = 0;
	u8 res = 0;	
	u8 tx_arp = 0;
	
	char temp[100];
	
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */
	delay_init();		/* 延时初始化 */
	uart1_init(115200);	/* UART1初始化 */
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */
	w5500_init();		/* 芯片初始化 */

	print( 	"******************************************\r\n"
			"*************杭州零芯电子*****************\r\n"
			"*********W5500开发板W5500 ARP************\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	oled_add_str("ARP START...");
		
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
					oled_add_str("<!DHCP TIMEOUT>");
				break;
				case DHCP_RET_UPDATE:					
					print(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
					DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],
					DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
					DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
					w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
					close(0);	
					status = 0X80;				
					oled_add_str("DHCP OK...");
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP 超时定时器 */		
			
			delay_ms(10);/* 10扫描一次 */
		}

		if((status&0XC0) == 0X80)
		{
			switch(getSn_SR(SOCK_ARP))
			{	
				case SOCK_MACRAW:
					if((tx_arp++ % 10) == 0)/* 1000ms刷新一次 */
					{
						arp_request(SOCK_ARP, rport, DHCP_GET.lip, DHCP_GET.mac, rip);
						oled_add_str("ARP REQUEST...");
					}
					
					if((socket_rxlen = getSn_RX_RSR(SOCK_ARP)) > 0)
					{
						res = arp_reply(SOCK_ARP, socket_rxlen);
						if(res == 1)
						{
							close(SOCK_ARP);
							print(temp,"MAC:%.2x.%.2x.%.2x.%.2x.%.2x.%.2x",
									 (aARPMSG->sender_mac[0]),	(aARPMSG->sender_mac[1]),  (aARPMSG->sender_mac[2]), 
									 (aARPMSG->sender_mac[3]), (aARPMSG->sender_mac[4]),  (aARPMSG->sender_mac[5])) ;	

							sprintf(temp,"MAC:%.2x.%.2x.%.2x.%.2x.%.2x.%.2x",
									(aARPMSG->sender_mac[0]),	(aARPMSG->sender_mac[1]),  (aARPMSG->sender_mac[2]), 
									(aARPMSG->sender_mac[3]), (aARPMSG->sender_mac[4]),  (aARPMSG->sender_mac[5])) ;												
							
							oled_add_str(temp);
							status = 0;
						}
											
						printf("arp reply!\r\n");
					}
					else if(res == 2)
					{
						close(SOCK_ARP);	
						oled_add_str("<!ARP FAIL!>");
						print("arp fail!\r\n");
					}
				break;
				case SOCK_CLOSED:					
					close(SOCK_ARP);	/* close the SOCKET */ 	  					
					socket(SOCK_ARP, Sn_MR_MACRAW, rport, 0);	 /* open the SOCKET with MACRAW mode */  
					oled_add_str("CRATE ARP SOCKET!");
				break; 
				default: break;
			}

			delay_ms(100);/* 100ms扫描一次 */
		}	
	}
}














