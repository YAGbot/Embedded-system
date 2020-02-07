/**
  ******************************************************************************
  * @file    m_weather.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   WEATHER ������ȡ
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "dns.h"
# include "socket.h"
# include "config.h"
# include "oled.h"
# include "spi.h"
# include "json.h"
# include <string.h>
# include <stdio.h>

#define WEATHER_SERVER "api.seniverse.com"
#define WEATHER_PORT 80
#define JSON_NAME				"name"/* ���� */
#define JSON_COUTRY				"country"/* ���� */
#define JSON_DATE				"date"/* ���� */
#define JSON_TEXT				"text"/* ��ǰ���� */
#define JSON_TEXT_DAY			"text_day"/* �������� */
#define JSON_TEXT_NIGHT			"text_night"/* �������� */
#define JSON_HIGHT				"high"/* ��������� */
#define JSON_LOW				"low"/* ��������� */
#define JSON_WIN_DIR			"wind_direction"/* ���� */
#define JSON_WIND_DIR_DEGREE	"wind_direction_degree"/* ����Ƕ� */
#define JSON_WIND_SPEED			"wind_speed"/* ���� */
#define JSON_WIND_SCALE			"wind_scale"/* �����ȼ� */
#define JSON_TEMP				"temperature"/* �����ȼ� */
#define JSON_CUR_WEATHER		"GET https://api.seniverse.com/v3/weather/now.json?key=pqe1fgv45lrdruq7&location=ip&language=en&unit=c\n\n"/* �������� */
#define JSON_3DYA_WEATHER		"GET https://api.seniverse.com/v3/weather/daily.json?key=pqe1fgv45lrdruq7&location=ip&language=en&unit=c&start=0&days=5\n\n"/* δ����������� */



extern uint8 ntp_msg[48];

u8 socket_buf[1024] = {0};
u16 socket_len = 0;

/* DHCP�����Զ���ȡ */
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u8 lip[4] = {192, 168, 10, 101};/* ����IP */


u8 country[20];/* ���� */
u8 name[20];/* �������� */	
u8 temperature[20];/* ��ǰ�¶� */	
u8 text[20];/* ��ǰ����״̬ */		
u8 date[4][20];/* ���� */
u8 text_day[4][20];/* �������� */
u8 text_night[4][20];/* �������� */
u8 high[4][20];/* �������λ */
u8 low[4][20];/* ��������� */
u8 wind_direction[4][20];/* ���� */
u8 wind_direction_degree[4][20];/* ����Ƕ� */
u8 wind_speed[4][20];/* ���� */
u8 wind_scale[4][20];/* �����ȼ� */


static u8 status = 0;
static char temp[100] = {0};
static u8 dns_retry_cnt = 0;
static u8 swth = 0;
static u16 anyport = 3000;

int main(void)
{
	 /*clk_init();        ��ʼ��ϵͳʱ��72M �Ѿ��������ļ���
    ��ͨ�������ó�ʼ����ɲ���Ҫ�ڶ���ĳ�ʼ�� */
	delay_init();		/* ��ʱ��ʼ�� */
	uart1_init(115200);	/* UART1��ʼ�� */
	spi1_init();		/* SPI1 ��ʼ�� */
	oled_init();		/* OLED��ʼ�� */
	w5500_init();		/* оƬ��ʼ�� */

	print( 	"******************************************\r\n"
			"*************������о����*****************\r\n"
			"*********W5500������W5500 SMTP E-MAIL******\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	oled_add_str("NTP TIME SYN...");
	
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
			switch(dns_query(SOCK_DNS, WEATHER_SERVER))/* ����DNS�����ĺͽ���DNS��Ӧ���� */
			{
				case DNS_RET_SUCCESS:/* DNS���������ɹ� */				

					/* DNS�����Ĵ�����0 */
					dns_retry_cnt=0;	
					status = 0;
					printf("DNS OK!\r\nIP:%d.%d.%d.%d\r\n",DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);					
					status = 0XC0;
				break;
				case DNS_RET_FAIL:/* DNS��������ʧ�� */
					dns_retry_cnt++;											//

					/* DNS�����Ĵ�����1 */
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
			
			delay_ms(1000);/* 1000msɨ��һ�� */			
		}	
		else if((status&0XE0) == 0XC0)/* ���� */
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
			switch(getSn_SR(0))	/* ��ȡsocket0��״̬ */ 
			{
				case SOCK_INIT:	/* Socket���ڳ�ʼ�����(��)״̬ */
						/* ����Sn_CRΪCONNECT������TCP������������������ */ 
						connect(0, DNS_GET_IP, WEATHER_PORT);			
						oled_add_str("CONNECT!");					
				break;
				case SOCK_ESTABLISHED:/* Socket�������ӽ���״̬ */ 
						if(getSn_IR(0) & Sn_IR_CON) setSn_IR(0, Sn_IR_CON);/* Sn_IR��CONλ��1��֪ͨW5500�����ѽ��� */  					

						if(swth == 0)
						{
							swth = 1;

	////////////////////////////��ȡ��ǰ����/////////////////////////////////////////////////////////////////////		
							/* W5500��ͻ��˷�������  ���1S */ 
							memset(socket_buf, 0, sizeof(socket_buf));
							sprintf((char *)socket_buf, "%s",JSON_CUR_WEATHER);										
							socket_len = strlen((char *)socket_buf);																			
							send(0,socket_buf,socket_len);

							delay_ms(100);
							/* ���ݻػ����Գ������ݴ���λ������������W5500��W5500���յ����ݺ��ٻظ������� */ 
							socket_len = getSn_RX_RSR(0); 									// len=Socket0���ջ������ѽ��պͱ�������ݴ�С
							if(socket_len > 0)
							{
								/*	W5500�������Է����������ݣ���ͨ��SPI���͸�MCU */
								memset(socket_buf, 0, sizeof(socket_buf));
								recv(0,socket_buf,socket_len);	/* W5500�������Կͻ��˵����ݣ���ͨ��SPI���͸�MCU */			
								printf("%s\r\n", socket_buf); 

								u8 * index = socket_buf;
						
								json_get_value(index, JSON_TEXT, text);
								printf("%s:%s\r\n", JSON_TEXT, text);

								json_get_value(index, JSON_TEMP, temperature);
								printf("%s:%s\r\n", JSON_TEMP, temperature);

								oled_add_str("WEATHER GET OK!");
								
							}							
							else
							{
								printf("weather get cur weather fail!\r\n");	
								oled_add_str("WEATHER GET FAIL!");
								
								close(0);	
								swth = 0;
								status = 0XFF;								
							}
						}
						else if(swth == 1)
						{
							swth = 0;
	////////////////////////////��ȡ3������/////////////////////////////////////////////////////////////////////
							/* W5500��ͻ��˷�������  ���1S */ 
							memset(socket_buf, 0, sizeof(socket_buf));
							sprintf((char *)socket_buf, "%s",JSON_3DYA_WEATHER);										
							socket_len = strlen((char *)socket_buf);																			
							send(0,socket_buf,socket_len);

							delay_ms(100);
							/* ���ݻػ����Գ������ݴ���λ������������W5500��W5500���յ����ݺ��ٻظ������� */ 
							socket_len = getSn_RX_RSR(0); 									// len=Socket0���ջ������ѽ��պͱ�������ݴ�С
							if(socket_len > 0)
							{
								/*	W5500�������Է����������ݣ���ͨ��SPI���͸�MCU */
								memset(socket_buf, 0, sizeof(socket_buf));
								recv(0,socket_buf,socket_len);	/* W5500�������Կͻ��˵����ݣ���ͨ��SPI���͸�MCU */			
								printf("%s\r\n", socket_buf);	

								u8 * index = socket_buf;

								json_get_value(index, JSON_COUTRY, country);
								printf("%s:%s\r\n", JSON_COUTRY, country);
								
								json_get_value(index, JSON_NAME, name);
								printf("%s:%s\r\n", JSON_NAME, name);


								/* ����3����������Ԥ�� */
								for(u8 i=0; i<3; i++)
								{
									index =  json_get_value(index, JSON_DATE, &date[i][0]) + 5;

									printf("%s:%s\r\n", JSON_DATE, date[i]);
									
									printf("\r\n");
									json_get_value(index, JSON_TEXT_DAY, text_day[i]);
									printf("%s:%s\r\n", JSON_TEXT_DAY, text_day[i]);

									json_get_value(index, JSON_TEXT_NIGHT, text_night[i]);
									printf("%s:%s\r\n", JSON_TEXT_NIGHT, text_night[i]);

									json_get_value(index, JSON_HIGHT, high[i]);
									printf("%s:%s\r\n", JSON_HIGHT, high[i]);

									json_get_value(index, JSON_LOW, low[i]);
									printf("%s:%s\r\n", JSON_LOW, low[i]);

									json_get_value(index, JSON_WIN_DIR, wind_direction[i]);
									printf("%s:%s\r\n", JSON_WIN_DIR, wind_direction[i]);

									json_get_value(index, JSON_WIND_DIR_DEGREE, wind_direction_degree[i]);
									printf("%s:%s\r\n", JSON_WIND_DIR_DEGREE, wind_direction_degree[i]);

									json_get_value(index, JSON_WIND_SPEED, wind_speed[i]);
									printf("%s:%s\r\n", JSON_WIND_SPEED, wind_speed[i]);

									json_get_value(index, JSON_WIND_SCALE, wind_scale[i]);
									printf("%s:%s\r\n", JSON_WIND_SCALE, wind_scale[i]);								
								}
														
								oled_add_str("WEATHER GET 3DAY OK!");
							}							
							else
							{
								printf("weather get 3day weather fail!\r\n");	
								oled_add_str("WEATHER GET 3DAY FAIL!");
							}

							close(0);	
							status = 0;							
						}

				break;
				case SOCK_CLOSE_WAIT:/* Socket���ڵȴ��ر�״̬ */ 
						/* �ر�Socket0 */ 
						close(0);																
						oled_add_str("SOCKET CLOSE");
				break;
				case SOCK_CLOSED:/* Socket���ڹر�״̬ */ 
						/* ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿� */ 
						socket(0,Sn_MR_TCP,anyport++, Sn_MR_ND);		
						oled_add_str("SOCKET OPEN");
						
				break;
			}	
			
			delay_ms(1000);/* 1000msɨ��һ�� */			
		}			
	}	 
}



