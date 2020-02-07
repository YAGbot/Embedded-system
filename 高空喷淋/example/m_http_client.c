/**
  ******************************************************************************
  * @file    m_http_client.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   HTTP 客户端 
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
# include "adc.h"
# include "rtc.h"
# include "tmp.h"
# include <string.h>
# include <stdio.h>


u8 socket_buf[2048] = {0};
u16 socket_len = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	

/* 采用DHCP的方式自动获取IP这里可不管 */
u8 lip[4] = {192, 168, 0, 101};/* 本地IP */
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u16 lport = 3000;/* 本地端口 */

//u8 rip[4] = {116,62,81,138};/* 远程IP */
u16 rport  = 80;

u16 timeout = 0;
u8 status = 0;
char temp[200] = {0};
u8 dns_retry_cnt = 0;
u8  ch = SOCK_HTTPS;

#define WWW 			"api.heclouds.com"/* 域名 */
#define DEV_ID 			"45741876"/* 设备ID */
#define API_KEY	 		"pE=92f9qVUMtBqaap5igrqNiSw0="/* 秘钥 */
#define DAT_STREAM 		"http_test_dat"/* 数据流 */


#if 0/* 发送的原型 */
char http_post[] = 
{
	"POST /devices/45741876/datapoints?type=3 HTTP/1.1\r\n"
	"api-key:pE=92f9qVUMtBqaap5igrqNiSw0=\r\n"
	"Host:api.heclouds.com\r\n"
	"Connection:close\r\n"
	"Content-Length:10\r\n"
	"\r\n"
	"{\"dat\":50}\r\n"
};
#endif



int main(void)
{
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */
	delay_init();		/* 延时初始化 */
	uart1_init(115200);	/* UART1初始化 */
	rtc_init();			/* rtc时钟初始化 */
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */
	w5500_init();		/* 芯片初始化 */
	adc_init();			/* ADC初始化 */
	tmp_init();			/* 内部温度窗户 */
	
	
	print( 	"******************************************\r\n"
			"*************杭州零芯电子*****************\r\n"
			"*********W5500开发板W5500 HTTP CLIENT********\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	
	sprintf(temp, "get %s", WWW);
	oled_add_str(temp);

	while(1)
	{		
		if(status == 0)/* DHCP自动获取IP */
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
					status = 0X80;
					sprintf(temp, "LIP:%d.%d.%d.%d", DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3]);
					oled_add_str(temp);								
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
			switch(dns_query(SOCK_DNS, WWW))/* 发出DNS请求报文和解析DNS响应报文 */
			{
				case DNS_RET_SUCCESS:/* DNS解析域名成功 */				

					/* DNS请求报文次数置0 */
					dns_retry_cnt = 0;	
					print(" DNS OK! RIP:%d.%d.%d.%d",
					DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);					
					sprintf(temp, "DNS:%d.%d.%d.%d",DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);
					oled_add_str(temp);
					status = 0XC0;			
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
		else if((status&0XE0) == 0XC0)
		{						
			switch(getSn_SR(ch))
			{
				case SOCK_INIT:
						connect(ch, DNS_GET_IP , rport);
				break;
				case SOCK_ESTABLISHED:
						if(getSn_IR(ch) & Sn_IR_CON)setSn_IR(ch, Sn_IR_CON);

						memset((char *)socket_buf, 0, sizeof(socket_buf));
						sprintf(temp, "POST /devices/%s/datapoints?type=3 HTTP/1.1\r\n", DEV_ID);
						strcat((char *)socket_buf, temp);
						sprintf(temp, "api-key:%s\r\n", API_KEY);
						strcat((char *)socket_buf, temp);
						sprintf(temp, "Host:api.heclouds.com\r\n");
						strcat((char *)socket_buf, temp);
						sprintf(temp, "Connection:close\r\n");
						strcat((char *)socket_buf, temp);
						sprintf(temp, "Content-Length:%d\r\n", 20);
						strcat((char *)socket_buf, temp);
						sprintf(temp, "\r\n");
						strcat((char *)socket_buf, temp);		
						sprintf(temp, "{\"%s\":%d}\r\n", DAT_STREAM, (int)(get_tmp()/100.0));
						strcat((char *)socket_buf, temp);
				
						socket_len = strlen((char *)socket_buf);
						send(ch,(const uint8 *)socket_buf, socket_len);
						
						close(ch);
						sprintf((char *)temp, "%02d:%02d:%02d tmp:%.1lf\n", calendar.hour,calendar.min,calendar.sec, get_tmp()/100.0);
						oled_add_str(temp); 
						printf("%s\n", socket_buf);
						
				break;
				case SOCK_CLOSE_WAIT: 

				break;
				case SOCK_CLOSED:  					            
						socket(ch, Sn_MR_TCP, lport++ , 0x00);   
						oled_add_str("SOCKET CLOSE!");
				break;
				default:
				break;
			}  		

			delay_ms(10000);/* 1000ms扫描一次 */			
		}	
		
		
	}
	 
}



