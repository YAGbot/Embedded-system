/**
  ******************************************************************************
  * @file    m_tcp_client.c
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
# include "adc.h"
# include "rtc.h"
# include "tmp.h"
# include <string.h>
# include <stdio.h>


u8 socket_buf[1024] = {0};
u16 socket_len = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

u8 lip[4] = {192, 168, 0, 101};/* ����IP */
u16 lport = 3000;/* ���ض˿� */
u8 rip[4] = {192, 168, 0, 101};/* Զ��IP */
u16 rport  = 10001;


u16 timeout = 0;
u8 status = 0;
char temp[100] = {0};


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
			"*********W5500������W5500 UDP CLIENT********\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	

	printf("TCP Server IP: %d.%d.%d.%d \r\n",rip[0],rip[1],rip[2],rip[3]);
	printf("TCP Server Port: %d \r\n",rport); 

	sprintf(temp, "RIP:%d.%d.%d.%d", rip[0], rip[1], rip[2], rip[3]);
	oled_add_str(temp);
	sprintf(temp, "RPORT:%d", rport);
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
					oled_add_str("DHCP TIMEOUT!");
					
				break;
				case DHCP_RET_UPDATE:					
					print(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
					DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],
					DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
					DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
					w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
					close(SOCK_DHCP);					
					/* ��W5500 Keepalive���� */
					setkeepalive(0);		
					
					status = 0X80;
					oled_add_str("DHCP OK!");								
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP ��ʱ��ʱ�� */			
			delay_ms(10);/* 10msɨ��һ�� */						
		}
		else if((status&0XC0) == 0X80)
		{	
			/*Socket״̬����MCUͨ����Sn_SR(0)��ֵ�����ж�SocketӦ�ô��ں���״̬
			Sn_SR״̬������
				0x00		SOCK_CLOSED
				0x13		SOCK_INIT
				0x14		SOCK_LISTEN
				0x17		SOCK_ESTABLISHED
				0x1C		SOCK_CLOSE_WAIT
				0x22		SOCK_UDP
			*/		
			switch(getSn_SR(0)) /* ��ȡsocket0��״̬ */ 
			{
				case SOCK_INIT: /* Socket���ڳ�ʼ�����(��)״̬ */
						/* ����Sn_CRΪCONNECT������TCP������������������ */ 
						connect(0, rip, rport);			
						oled_add_str("SOCKET CONNECT..."); 
				break;
				case SOCK_ESTABLISHED:/* Socket�������ӽ���״̬ */ 
						if(getSn_IR(0) & Sn_IR_CON) setSn_IR(0, Sn_IR_CON);/* Sn_IR��CONλ��1��֪ͨW5500�����ѽ��� */					
			
						/* ���ݻػ����Գ������ݴ���λ������������W5500��W5500���յ����ݺ��ٻظ������� */ 
						socket_len = getSn_RX_RSR(0); 									// len=Socket0���ջ������ѽ��պͱ�������ݴ�С
						if(socket_len > 0)
						{
							/*	W5500�������Է����������ݣ���ͨ��SPI���͸�MCU */
							memset(socket_buf, 0, sizeof(socket_buf));
							sprintf((char *)socket_buf, "%s", "REC:");
			
							/* W5500�������Կͻ��˵����ݣ���ͨ��SPI���͸�MCU */ 
							recv(0,socket_buf+4,socket_len);				
	
							/* ���յ����ݺ��ٻظ��ͻ��ˣ�������ݻػ� */ 
							send(0,socket_buf+4,socket_len);		

							oled_add_str(socket_buf); 
						}
			
						/* W5500��ͻ��˷�������  ���1S */ 
						sprintf((char *)socket_buf, "TCP CLIENT:%02d:%02d:%02d\r\n", calendar.hour,calendar.min,calendar.sec);
						socket_len = strlen((char *)socket_buf);
						send(0,socket_buf,socket_len);		

						sprintf((char *)temp, "%02d:%02d:%02d tmp:%.1lf\n", calendar.hour,calendar.min,calendar.sec, get_tmp()/100.0);
						oled_add_str(temp); 
						printf("%s\n", socket_buf);
						sendto(0,socket_buf,socket_len, rip, rport);	

						
				break;
				case SOCK_CLOSE_WAIT:/* Socket���ڵȴ��ر�״̬ */ 
						/* �ر�Socket0 */ 
						close(0);																						
						oled_add_str("SOCKET TCP CLOSE!"); 
				break;
				case SOCK_CLOSED:/* Socket���ڹر�״̬ */ 
						/* ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿� */ 
						socket(0,Sn_MR_TCP,lport,Sn_MR_ND);		
						oled_add_str("SOCKET TCP OPEN!"); 
				break;
			}


			delay_ms(1000);/* 1000msɨ��һ�� */			
		}	
		
		
	}
	 
}



