/**
  ******************************************************************************
  * @file    m_ntp.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   NTP 时间同步
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "dns.h"
# include "tftp.h"
# include "socket.h"
# include "config.h"
# include "oled.h"
# include "spi.h"
# include "ntp.h"
# include "rtc.h"
# include <string.h>
# include <stdio.h>



extern uint8 ntp_msg[48];

u8 socket_buf[1024] = {0};
u16 socket_len = 0;

/* DHCP采用自动获取 */
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u8 lip[4] = {192, 168, 10, 101};/* 本地IP */


#define NTP_SERVER "ntp1.aliyun.com"
#define NTP_PORT 123


static u8 status = 0;
static char temp[100] = {0};
static u8 dns_retry_cnt = 0;


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
			"*********W5500开发板W5500 SMTP E-MAIL******\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	oled_add_str("NTP TIME SYN...");
	printf("NTP TIME SYN...\r\n");

	while(1)
	{		
		if(status == 0)/* DHCP 自动获取IP*/
		{
			switch(check_DHCP_state(SOCK_DHCP))
			{
				case DHCP_RET_NONE: 					
				break;
				case DHCP_RET_TIMEOUT:	
					print("dhcp timeout!\r\n"); 
					oled_add_str("DHCP TIMEOUT!");					
				break;
				case DHCP_RET_UPDATE:					
					print(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
					DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],
					DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
					DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
					w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
					close(SOCK_DHCP);								
					sprintf(temp, "LIP:%d.%d.%d.%d", DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3]);
					oled_add_str(temp);			
					status = 0X80;				
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP 超时定时器 */			
			delay_ms(10);/* 10ms扫描一次 */						
		}
		else if((status&0XC0) == 0X80)/* DNS */
		{						
			switch(dns_query(SOCK_DNS, NTP_SERVER))/* 发出DNS请求报文和解析DNS响应报文 */
			{
				case DNS_RET_SUCCESS:/* DNS解析域名成功 */				

					/* DNS请求报文次数置0 */
					dns_retry_cnt=0;	
					status = 0;
					printf("DNS OK!\r\nIP:%d.%d.%d.%d\r\n",DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);					
					sprintf(temp, "DNS:%d.%d.%d.%d", DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);
					oled_add_str(temp);	
					ntpclient_init();				
					status = 0XC0;
				break;
				case DNS_RET_FAIL:/* DNS解析域名失败 */
					dns_retry_cnt++;											//

					/* DNS请求报文次数加1 */
					if(dns_retry_cnt++ > DNS_RETRY)
					{
						dns_retry_cnt = 0;
						status = 0XFF;
						oled_add_str("DNS FAIL!");	
					}				
					printf("Fail! Please check your network configuration or DNS server.\r\n");
				break;
				default:
				break;
			}					
			delay_ms(1000);/* 1000ms扫描一次 */			
		}	
		else if((status&0XE0) == 0XC0)/* 发送 */
		{												
			switch(getSn_SR(SOCK_NTP))
			{
				case SOCK_UDP:
					if((socket_len = getSn_RX_RSR(SOCK_NTP)) > 0) 		
					{
						recvfrom(SOCK_NTP, socket_buf, socket_len, DNS_GET_IP, (u16 *)NTP_PORT);	

						for(u16 i=0; i<socket_len; i++)
						{
							printf("%02X ", socket_buf[i]);
						}
						printf("\r\n");					

						u32 seconds = socket_buf[40]<<24 | socket_buf[41]<<16 | socket_buf[42]<<8 | socket_buf[43];
						
						rtc_xget((seconds-TIME_1970)+ 8 * 3600, &calendar);
						rtc_set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
						

						sprintf(temp,"TIME:%d:%d:%d",  calendar.hour,calendar.min,calendar.sec);

						oled_add_str(temp);
						status = 0XFF;									
					}
					sendto(SOCK_NTP,ntp_msg, sizeof(ntp_msg), DNS_GET_IP, NTP_PORT);

				break;
				case SOCK_CLOSED:
					socket(SOCK_NTP,Sn_MR_UDP, NTP_PORT,0);
				
				break;
			}			
			delay_ms(1000);/* 1000ms扫描一次 */			
		}			
	}	 
}



