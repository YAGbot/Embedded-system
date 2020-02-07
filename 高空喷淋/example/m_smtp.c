/**
  ******************************************************************************
  * @file    m_smtp.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   SMTP 邮寄发送
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
# include "adc.h"
# include "rtc.h"
# include "tmp.h"
# include "smtp.h"
# include <string.h>
# include <stdio.h>


extern uint8 SMTP_STATE;

u8 socket_buf[MAX_MTU_SIZE] = {0};
u16 socket_len = 0;

/* DHCP采用自动获取 */
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u8 lip[4] = {192, 168, 10, 101};/* 本地IP */


/*
smtp.126.com 25
smtp.qq.com 端口465或587
*/
#define SMTP_SERVER "smtp.126.com"
#define SMTP_PORT 25


static u8 status = 0;
static char temp[100] = {0};
static u8 dns_retry_cnt = 0;
static u8 res = 0;
static u8 ch = SOCK_SMTP;

int main(void)
{
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */
	delay_init();		/* 延时初始化 */
	uart1_init(115200);	/* UART1初始化 */
	rtc_init();			/* rtc时钟初始化 */
	tmp_init();			/* 内部温度初始化 */
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */
	w5500_init();		/* 芯片初始化 */
	adc_init();			/* ADC初始化 */
	tmp_init();			/* 内部温度窗户 */
	
	
	print( 	"******************************************\r\n"
			"*************杭州零芯电子*****************\r\n"
			"*********W5500开发板W5500 SMTP E-MAIL******\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
		
	oled_add_str("E-MAIL TEST...");
 
	
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
			switch(dns_query(SOCK_DNS, SMTP_SERVER))/* 发出DNS请求报文和解析DNS响应报文 */
			{
				case DNS_RET_SUCCESS:/* DNS解析域名成功 */				

					/* DNS请求报文次数置0 */
					dns_retry_cnt=0;	
					status = 0;
					printf("DNS OK!\r\nIP:%d.%d.%d.%d\r\n",DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);										
					mailmessage();/* 初始化邮寄消息 */
					printf("Start E-MAIL Test!\r\n");	
					status = 0XC0;
				break;
				case DNS_RET_FAIL:/* DNS解析域名失败 */
					dns_retry_cnt++;											//

					/* DNS请求报文次数加1 */
					if(dns_retry_cnt++ > DNS_RETRY)
					{
						dns_retry_cnt = 0;
						status = 0XFF;
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
			u16 anyport = 5500;
			memset(socket_buf,0,sizeof(socket_buf));
			switch(getSn_SR(ch))
			{
				case SOCK_INIT:					
					connect(ch, DNS_GET_IP ,SMTP_PORT);
					oled_add_str("E-MAIL SOCKET CONNECT!");
					SMTP_STATE = 0;
				break;
				case SOCK_ESTABLISHED:
					if(getSn_IR(ch) & Sn_IR_CON)setSn_IR(ch, Sn_IR_CON);

					socket_len = getSn_RX_RSR(ch);			
					if(socket_len)	   
					{															
						memset(socket_buf,0,sizeof(socket_buf));
						recv(ch, (u8*)socket_buf,socket_len);								
						printf("RXLEN:%d SMTP_STATE:%d %s \r\n",SMTP_STATE, socket_len, socket_buf);
						
						res = send_mail(ch, (char *)socket_buf);
											
						if(res == 1)
						{							
							oled_add_str("E-MAIL SEND OK!");
							status = 0XFF;
							SMTP_STATE = 0;
							disconnect(ch);
							printf("close socket!\r\n");
						}		
						if(res == 2)
						{							
							oled_add_str("E-MAIL SEND FAIL!");
							status = 0XFF;
							SMTP_STATE = 0;
							disconnect(ch);
						}							
					}
				break;
				case SOCK_CLOSE_WAIT:   
					disconnect(ch);
				break;
				case SOCK_CLOSED:				   
					socket(ch, Sn_MR_TCP,anyport++, 0x00);					
					oled_add_str("E-MAIL SOCKET OPEN!");					
				break;
				default:
				break;
			} 			
			delay_ms(1000);/* 1000ms扫描一次 */			
		}			
	}	 
}



