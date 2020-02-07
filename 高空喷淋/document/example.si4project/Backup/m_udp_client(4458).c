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
# include "ping.h"
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
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */
	delay_init();		/* 延时初始化 */
	uart1_init(115200);	/* UART1初始化 */
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */
	w5500_init();		/* 芯片初始化 */

	print( 	"******************************************\r\n"
			"*************杭州零芯电子*****************\r\n"
			"*********W5500开发板W5500 PING HOST********\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	

	sprintf(temp, "ping:%s", PING);
	oled_str(0,y_pos+=12,(u8*)temp, 12);
	oled_update();/* oled 显存区更新 */		

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
					oled_str(0,y_pos+=12,"dhcp timeout!", 12);
					oled_update();/* oled 显存区更新 */						
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
					oled_update();/* oled 显存区更新 */				
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP 超时定时器 */		
			
			delay_ms(100);/* 100ms扫描一次 */
		}

		if((status&0XC0) == 0X80)
		{
			switch(getSn_SR(SOCK_PING))																			/*获取socket状态*/
			{
				case SOCK_CLOSED:/*socket关闭状态*/					
					/* Create Socket */  
					IINCHIP_WRITE(Sn_PROTO(SOCK_PING), IPPROTO_ICMP);/*设置ICMP 协议*/
					if(socket(SOCK_PING,Sn_MR_IPRAW,3000,0) != 0)	/*判断ip raw模式socket是否开启*/
					{						
					}
					/* Check socket register */
					while(getSn_SR(SOCK_PING) != SOCK_IPRAW);		
					ping_status = 0;
					timeout = 0;
				break;
				case SOCK_IPRAW:/*ip raw模式*/		

					if(ping_status == 0)
					{
						ping_request(SOCK_PING, rip);/*发送Ping请求*/
						ping_status = 0X80;
						print("ping!\r\n");
					}
			
					if((socket_rxlen = getSn_RX_RSR(SOCK_PING)) > 0)
					{
						u8 res = ping_reply(SOCK_PING, rip, socket_rxlen);/*获取回复信息*/
						if (res)  
						{
							status = 0XFF;								
							oled_str(0,y_pos+=12,"ping host ok!", 12);
							oled_update();/* oled 显存区更新 */		
							close(0);											
						}
					}

					if(timeout++ > 30)
					{					
						timeout = 0;
						ping_status = 0;
						status = 0XFF;
						oled_str(0,y_pos+=12,"ping host fail!", 12);
						oled_update();/* oled 显存区更新 */		
					}

				break;	
				default:					
				break;
			}			
			

			delay_ms(1000);/* 1000ms扫描一次 */
		}	
	}
	 
}



