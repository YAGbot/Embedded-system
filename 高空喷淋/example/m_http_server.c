/**
  ******************************************************************************
  * @file    m_http_client.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   HTTP �ͻ��� 
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
# include "http_server.h"
# include "httputil.h"
# include <string.h>
# include <stdio.h>


u8 socket_buf[2048] = {0};
u16 socket_len = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	

/* ����DHCP�ķ�ʽ�Զ���ȡIP����ɲ��� */
u8 lip[4] = {192, 168, 0, 101};/* ����IP */
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u16 lport = 3000;/* ���ض˿� */


u16 rport  = 80;

u16 timeout = 0;
u8 status = 0;
char temp[200] = {0};
u8 dns_retry_cnt = 0;
u8  ch = SOCK_HTTPS;




int main(void)
{
	 /*clk_init();        ��ʼ��ϵͳʱ��72M �Ѿ��������ļ���
    ��ͨ�������ó�ʼ����ɲ���Ҫ�ڶ���ĳ�ʼ�� */
	delay_init();		/* ��ʱ��ʼ�� */
	uart1_init(115200);	/* UART1��ʼ�� */
	rtc_init();			/* rtcʱ�ӳ�ʼ�� */
	spi1_init();		/* SPI1 ��ʼ�� */
	oled_init();		/* OLED��ʼ�� */
	w5500_init();		/* оƬ��ʼ�� */
	adc_init();			/* ADC��ʼ�� */
	tmp_init();			/* �ڲ��¶ȴ��� */
	
	
	print( 	"******************************************\r\n"
			"*************������о����*****************\r\n"
			"*********W5500������W5500 HTTP SERVER******\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client();

	printf("<��ʾ����������ĵ�ַ������W5500��IP��ַ��W5500/������������ҳ>\r\n"); 
		
	while(1)
	{		
		if(status == 0)/* DHCP�Զ���ȡIP */
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

			DHCP_timer_handler();/* DHCP ��ʱ��ʱ�� */			
			delay_ms(10);/* 10msɨ��һ�� */						
		}
		else if((status&0XC0) == 0X80)/* DNS */
		{			
			st_http_request * http_request;	/*����һ���ṹָ��*/
			memset(socket_buf,0x00,sizeof(socket_buf));
			http_request = (st_http_request*)socket_buf;					 
			/* http service start */
			switch(getSn_SR(ch))/*��ȡsocket״̬*/
			{
				case SOCK_INIT:	/*socket���ڳ�ʼ��״̬*/
					listen(ch);
					oled_add_str("SERVER LISTEN!");
					break;
				
				case SOCK_LISTEN:/*socket���ڼ���״̬*/
					break;				
				case SOCK_ESTABLISHED:/*socket��������״̬*/
					if(getSn_IR(ch) & Sn_IR_CON)setSn_IR(ch, Sn_IR_CON);/*����жϱ�־λ*/

					if ((socket_len = getSn_RX_RSR(ch)) > 0)		
					{
						socket_len = recv(ch, (uint8*)http_request, socket_len); 	/*����http����*/
						*(((uint8*)http_request)+socket_len) = 0;
						proc_http(ch, (uint8*)http_request);		/*����http���󲢷���http��Ӧ*/
						disconnect(ch);
					}
					break;
					
				case SOCK_CLOSE_WAIT:  /*socket���ڵȴ��ر�״̬*/
					if ((socket_len = getSn_RX_RSR(ch)) > 0)
					{
						socket_len = recv(ch, (uint8*)http_request, socket_len);	/*����http����*/      
						*(((uint8*)http_request)+socket_len) = 0;
						proc_http(ch, (uint8*)http_request);		/*����http���󲢷���http��Ӧ*/
					}
					disconnect(ch);
					break;
					
				case SOCK_CLOSED: /*socket���ڹر�״̬*/
					socket(ch, Sn_MR_TCP, 80, 0x00);  /*��socket*/
					oled_add_str("SERVER OPEN!");
					break;
				
				default:
					break;
			}		
				
			delay_ms(100);/* 100msɨ��һ�� */		
		}
	}
	 
}



