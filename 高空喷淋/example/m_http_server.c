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
# include "http_server.h"
# include "httputil.h"
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


u16 rport  = 80;

u16 timeout = 0;
u8 status = 0;
char temp[200] = {0};
u8 dns_retry_cnt = 0;
u8  ch = SOCK_HTTPS;




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
			"*********W5500开发板W5500 HTTP SERVER******\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client();

	printf("<提示：在浏览器的地址栏输入W5500的IP地址或“W5500/”进入内置网页>\r\n"); 
		
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
			st_http_request * http_request;	/*定义一个结构指针*/
			memset(socket_buf,0x00,sizeof(socket_buf));
			http_request = (st_http_request*)socket_buf;					 
			/* http service start */
			switch(getSn_SR(ch))/*获取socket状态*/
			{
				case SOCK_INIT:	/*socket处于初始化状态*/
					listen(ch);
					oled_add_str("SERVER LISTEN!");
					break;
				
				case SOCK_LISTEN:/*socket处于监听状态*/
					break;				
				case SOCK_ESTABLISHED:/*socket处于连接状态*/
					if(getSn_IR(ch) & Sn_IR_CON)setSn_IR(ch, Sn_IR_CON);/*清除中断标志位*/

					if ((socket_len = getSn_RX_RSR(ch)) > 0)		
					{
						socket_len = recv(ch, (uint8*)http_request, socket_len); 	/*接收http请求*/
						*(((uint8*)http_request)+socket_len) = 0;
						proc_http(ch, (uint8*)http_request);		/*接收http请求并发送http响应*/
						disconnect(ch);
					}
					break;
					
				case SOCK_CLOSE_WAIT:  /*socket处于等待关闭状态*/
					if ((socket_len = getSn_RX_RSR(ch)) > 0)
					{
						socket_len = recv(ch, (uint8*)http_request, socket_len);	/*接收http请求*/      
						*(((uint8*)http_request)+socket_len) = 0;
						proc_http(ch, (uint8*)http_request);		/*接收http请求并发送http响应*/
					}
					disconnect(ch);
					break;
					
				case SOCK_CLOSED: /*socket处于关闭状态*/
					socket(ch, Sn_MR_TCP, 80, 0x00);  /*打开socket*/
					oled_add_str("SERVER OPEN!");
					break;
				
				default:
					break;
			}		
				
			delay_ms(100);/* 100ms扫描一次 */		
		}
	}
	 
}



