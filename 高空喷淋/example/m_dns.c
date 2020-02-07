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
# include <string.h>


u8 socket_buf[1024] = {0};
u16 socket_rxlen = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 lip[4] = {192, 168, 0, 101};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

#define WWW "www.baidu.com"

s16 y_pos = -12;

int main(void)
{
	u8 status = 0;
	u8 dns_retry_cnt = 0;
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
			"*********W5500开发板W5500 DNS************\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	

	sprintf(temp, "get:%s", WWW);
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
					oled_add_str("<!DHCP TIMETOUT>");				
				break;
				case DHCP_RET_UPDATE:					
					print(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
					DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],
					DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
					DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
					w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
					close(SOCK_DHCP);
					close(SOCK_DNS);				
					status = 0X80;
					oled_add_str("DHCP OK");				
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP 超时定时器 */		
			
			delay_ms(10);/* 10ms扫描一次 */
		}

		if((status&0XC0) == 0X80)
		{
			switch(dns_query(SOCK_DNS, WWW))/* 发出DNS请求报文和解析DNS响应报文 */
			{
				case DNS_RET_SUCCESS:/* DNS解析域名成功 */				

					/* DNS请求报文次数置0 */
					dns_retry_cnt = 0;	
					print(" DNS OK! RIP:%d.%d.%d.%d",
					DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);
					status = 0xff;		
					sprintf(temp, "dns:%d.%d.%d.%d",DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);
					oled_add_str(temp);
				break;
				case DNS_RET_FAIL:/* DNS解析域名失败 */
					/* DNS请求报文次数加1 */
					if(dns_retry_cnt++ > DNS_RETRY)
					{
						print("dns fail!\r\n");
						oled_add_str("DNS FAIL!");
						status = 0;
					}				
					print("Fail! Please check your network configuration or DNS server.\r\n");
				break;
				default:
				break;
			}

			delay_ms(1000);/* 1000ms扫描一次 */
		}	
	}
	 
}



