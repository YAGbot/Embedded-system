/**
  ******************************************************************************
  * @file    m_weather.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   WEATHER 天气获取
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
#define JSON_NAME				"name"/* 名字 */
#define JSON_COUTRY				"country"/* 城市 */
#define JSON_DATE				"date"/* 日期 */
#define JSON_TEXT				"text"/* 当前天气 */
#define JSON_TEXT_DAY			"text_day"/* 白天天气 */
#define JSON_TEXT_NIGHT			"text_night"/* 晚上天气 */
#define JSON_HIGHT				"high"/* 当天最高温 */
#define JSON_LOW				"low"/* 当天最低温 */
#define JSON_WIN_DIR			"wind_direction"/* 风向 */
#define JSON_WIND_DIR_DEGREE	"wind_direction_degree"/* 风向角度 */
#define JSON_WIND_SPEED			"wind_speed"/* 风速 */
#define JSON_WIND_SCALE			"wind_scale"/* 风力等级 */
#define JSON_TEMP				"temperature"/* 风力等级 */
#define JSON_CUR_WEATHER		"GET https://api.seniverse.com/v3/weather/now.json?key=pqe1fgv45lrdruq7&location=ip&language=en&unit=c\n\n"/* 当亲天气 */
#define JSON_3DYA_WEATHER		"GET https://api.seniverse.com/v3/weather/daily.json?key=pqe1fgv45lrdruq7&location=ip&language=en&unit=c&start=0&days=5\n\n"/* 未来三天的天气 */



extern uint8 ntp_msg[48];

u8 socket_buf[1024] = {0};
u16 socket_len = 0;

/* DHCP采用自动获取 */
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u8 lip[4] = {192, 168, 10, 101};/* 本地IP */


u8 country[20];/* 国家 */
u8 name[20];/* 城市名字 */	
u8 temperature[20];/* 当前温度 */	
u8 text[20];/* 当前天气状态 */		
u8 date[4][20];/* 日期 */
u8 text_day[4][20];/* 白天天气 */
u8 text_night[4][20];/* 晚上天气 */
u8 high[4][20];/* 当天最高位 */
u8 low[4][20];/* 当天最低温 */
u8 wind_direction[4][20];/* 风向 */
u8 wind_direction_degree[4][20];/* 风向角度 */
u8 wind_speed[4][20];/* 风速 */
u8 wind_scale[4][20];/* 风力等级 */


static u8 status = 0;
static char temp[100] = {0};
static u8 dns_retry_cnt = 0;
static u8 swth = 0;
static u16 anyport = 3000;

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
			switch(dns_query(SOCK_DNS, WEATHER_SERVER))/* 发出DNS请求报文和解析DNS响应报文 */
			{
				case DNS_RET_SUCCESS:/* DNS解析域名成功 */				

					/* DNS请求报文次数置0 */
					dns_retry_cnt=0;	
					status = 0;
					printf("DNS OK!\r\nIP:%d.%d.%d.%d\r\n",DNS_GET_IP[0],DNS_GET_IP[1],DNS_GET_IP[2],DNS_GET_IP[3]);					
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
			/*Socket状态机，MCU通过读Sn_SR(0)的值进行判断Socket应该处于何种状态
			Sn_SR状态描述：
				0x00		SOCK_CLOSED
				0x13		SOCK_INIT
				0x14		SOCK_LISTEN
				0x17		SOCK_ESTABLISHED
				0x1C		SOCK_CLOSE_WAIT
				0x22		SOCK_UDP
			*/		
			switch(getSn_SR(0))	/* 获取socket0的状态 */ 
			{
				case SOCK_INIT:	/* Socket处于初始化完成(打开)状态 */
						/* 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求 */ 
						connect(0, DNS_GET_IP, WEATHER_PORT);			
						oled_add_str("CONNECT!");					
				break;
				case SOCK_ESTABLISHED:/* Socket处于连接建立状态 */ 
						if(getSn_IR(0) & Sn_IR_CON) setSn_IR(0, Sn_IR_CON);/* Sn_IR的CON位置1，通知W5500连接已建立 */  					

						if(swth == 0)
						{
							swth = 1;

	////////////////////////////获取当前天气/////////////////////////////////////////////////////////////////////		
							/* W5500向客户端发送数据  间隔1S */ 
							memset(socket_buf, 0, sizeof(socket_buf));
							sprintf((char *)socket_buf, "%s",JSON_CUR_WEATHER);										
							socket_len = strlen((char *)socket_buf);																			
							send(0,socket_buf,socket_len);

							delay_ms(100);
							/* 数据回环测试程序：数据从上位机服务器发给W5500，W5500接收到数据后再回给服务器 */ 
							socket_len = getSn_RX_RSR(0); 									// len=Socket0接收缓存中已接收和保存的数据大小
							if(socket_len > 0)
							{
								/*	W5500接收来自服务器的数据，并通过SPI发送给MCU */
								memset(socket_buf, 0, sizeof(socket_buf));
								recv(0,socket_buf,socket_len);	/* W5500接收来自客户端的数据，并通过SPI发送给MCU */			
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
	////////////////////////////获取3天天气/////////////////////////////////////////////////////////////////////
							/* W5500向客户端发送数据  间隔1S */ 
							memset(socket_buf, 0, sizeof(socket_buf));
							sprintf((char *)socket_buf, "%s",JSON_3DYA_WEATHER);										
							socket_len = strlen((char *)socket_buf);																			
							send(0,socket_buf,socket_len);

							delay_ms(100);
							/* 数据回环测试程序：数据从上位机服务器发给W5500，W5500接收到数据后再回给服务器 */ 
							socket_len = getSn_RX_RSR(0); 									// len=Socket0接收缓存中已接收和保存的数据大小
							if(socket_len > 0)
							{
								/*	W5500接收来自服务器的数据，并通过SPI发送给MCU */
								memset(socket_buf, 0, sizeof(socket_buf));
								recv(0,socket_buf,socket_len);	/* W5500接收来自客户端的数据，并通过SPI发送给MCU */			
								printf("%s\r\n", socket_buf);	

								u8 * index = socket_buf;

								json_get_value(index, JSON_COUTRY, country);
								printf("%s:%s\r\n", JSON_COUTRY, country);
								
								json_get_value(index, JSON_NAME, name);
								printf("%s:%s\r\n", JSON_NAME, name);


								/* 解析3天的天的天气预告 */
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
				case SOCK_CLOSE_WAIT:/* Socket处于等待关闭状态 */ 
						/* 关闭Socket0 */ 
						close(0);																
						oled_add_str("SOCKET CLOSE");
				break;
				case SOCK_CLOSED:/* Socket处于关闭状态 */ 
						/* 打开Socket0，并配置为TCP无延时模式，打开一个本地端口 */ 
						socket(0,Sn_MR_TCP,anyport++, Sn_MR_ND);		
						oled_add_str("SOCKET OPEN");
						
				break;
			}	
			
			delay_ms(1000);/* 1000ms扫描一次 */			
		}			
	}	 
}



