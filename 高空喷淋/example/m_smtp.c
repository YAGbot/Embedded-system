/**
  ******************************************************************************
  * @file    m_smtp.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   SMTP �ʼķ���
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

/* DHCP�����Զ���ȡ */
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u8 lip[4] = {192, 168, 10, 101};/* ����IP */


/*
smtp.126.com 25
smtp.qq.com �˿�465��587
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
	 /*clk_init();        ��ʼ��ϵͳʱ��72M �Ѿ��������ļ���
    ��ͨ�������ó�ʼ����ɲ���Ҫ�ڶ���ĳ�ʼ�� */
	delay_init();		/* ��ʱ��ʼ�� */
	uart1_init(115200);	/* UART1��ʼ�� */
	rtc_init();			/* rtcʱ�ӳ�ʼ�� */
	tmp_init();			/* �ڲ��¶ȳ�ʼ�� */
	spi1_init();		/* SPI1 ��ʼ�� */
	oled_init();		/* OLED��ʼ�� */
	w5500_init();		/* оƬ��ʼ�� */
	adc_init();			/* ADC��ʼ�� */
	tmp_init();			/* �ڲ��¶ȴ��� */
	
	
	print( 	"******************************************\r\n"
			"*************������о����*****************\r\n"
			"*********W5500������W5500 SMTP E-MAIL******\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
		
	oled_add_str("E-MAIL TEST...");
 
	
	while(1)
	{		
		if(status == 0)/* DHCP �Զ���ȡIP*/
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

			DHCP_timer_handler();/* DHCP ��ʱ��ʱ�� */			
			delay_ms(10);/* 10msɨ��һ�� */						
		}
		else if((status&0XC0) == 0X80)/* DNS */
		{						
			switch(dns_query(SOCK_DNS, SMTP_SERVER))/* ����DNS�����ĺͽ���DNS��Ӧ���� */
			{
				case DNS_RET_SUCCESS:/* DNS���������ɹ� */				

					/* DNS�����Ĵ�����0 */
					dns_retry_cnt=0;	
					status = 0;
					printf("DNS OK!\r\nIP:%d.%d.%d.%d\r\n",DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);										
					mailmessage();/* ��ʼ���ʼ���Ϣ */
					printf("Start E-MAIL Test!\r\n");	
					status = 0XC0;
				break;
				case DNS_RET_FAIL:/* DNS��������ʧ�� */
					dns_retry_cnt++;											//

					/* DNS�����Ĵ�����1 */
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
			delay_ms(1000);/* 1000msɨ��һ�� */			
		}	
		else if((status&0XE0) == 0XC0)/* ���� */
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
			delay_ms(1000);/* 1000msɨ��һ�� */			
		}			
	}	 
}



