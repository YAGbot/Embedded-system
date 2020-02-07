# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "socket.h"
# include "dns.h"
# include "oled.h"
# include "spi.h"
# include "rs485.h"
# include "iocheck.h"
# include "iwdog.h"
# include <string.h>
# include <stdio.h>


extern u8 Temp_str[50];
u8 socket_buf[1024] = {0};
u16 socket_len = 0;
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

u8 lip[4] = {192, 168, 0, 101};/* ����IP */
u16 lport = 3000;/* ���ض˿� */

//u8 rip[4] = {182, 92, 194, 8};/* Զ��IP */
//u16 rport  = 1995;
u8 rip[4] = {61, 181, 255, 77};/* Զ��IP */
u16 rport  = 41001;
//uint8_t domain_ip[4]={61,181,225,77};
//u16 timeout = 0;
u8 status = 0;
char temp[100] = {0};


int main(void)
{
	 /*clk_init();        ��ʼ��ϵͳʱ��72M �Ѿ��������ļ���
    ��ͨ�������ó�ʼ����ɲ���Ҫ�ڶ���ĳ�ʼ�� */  	
	delay_init();		/* ��ʱ��ʼ�� */
	uart2_init(115200);	/* UART1��ʼ�� */
	uart1_init(9600);
	spi1_init();		/* SPI1 ��ʼ�� */
	oled_init();		/* OLED��ʼ�� */ 	
	w5500_init();		/* оƬ��ʼ�� */
	iocheck_init();	//IO��ʼ��

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
	IWDG_Init(6,3750);    //���Ƶ��Ϊ256,����ֵΪ3750,���ʱ��Ϊ24s
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
							//sprintf((char *)socket_buf, "%s", "REC:");
							
							/* W5500�������Կͻ��˵����ݣ���ͨ��SPI���͸�MCU */ 
							recv(0,socket_buf,socket_len);				
							if(strcmp((char *)socket_buf,"switch1on")==0)
							{
								print("111on\r\n");
								SWITCH1(OPEN);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch1off")==0) 
							{
								print("111off\r\n");
								SWITCH1(CLOSE);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch2on")==0)
							{
								print("222on\r\n");
								SWITCH2(OPEN);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch2off")==0)
							{
								print("222off\r\n"); 
								SWITCH2(CLOSE);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch3on")==0)
							{
								print("333on\r\n");
								SWITCH3(OPEN);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch3off")==0)
							{
								print("333off\r\n");
								SWITCH3(CLOSE);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch4on")==0)
							{
								print("444on\r\n");
								SWITCH4(OPEN);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch4off")==0)
							{
								print("444off\r\n");
								SWITCH4(CLOSE);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch5on")==0)
							{
								print("555on\r\n");
								SWITCH5(OPEN);
								send(0,(u8*)"over",4);	
							}
							if(strcmp((char *)socket_buf,"switch5off")==0)
							{
								print("555off\r\n");
								SWITCH5(CLOSE);
								send(0,(u8*)"over",4);	
							}
							/* ���յ����ݺ��ٻظ��ͻ��ˣ�������ݻػ� */ 
							//send(0,(u8*)"over",4);		

							oled_add_str(socket_buf); 
						}
						get_data();
						/* W5500��ͻ��˷�������  ���1S */ 
						sprintf((char *)socket_buf, Temp_str);
						socket_len = strlen((char *)socket_buf);
						
						sprintf((char *)temp, Temp_str);
						//oled_add_str("SOCKET"); 
						print("socket_buf=%s\r\n", socket_buf);
						sendto(0,socket_buf,socket_len, rip, rport);		
						IWDG_Feed();
						//printf("ι��\r\n");
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

			delay_ms(2000);/* 1000msɨ��һ�� */			
		}	
		
		
	}
	 
}



