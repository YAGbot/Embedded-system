/**
  ******************************************************************************
  * @file    m_upd_client.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   UDP 客户端 
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

u8 lip[4] = {192, 168, 0, 101};/* 本地IP */
u16 lport = 3000;/* 本地端口 */
u8 rip[4] = {192, 168, 0, 101};/* 远程IP */
u16 rport  = 10001;


u16 timeout = 0;
u8 status = 0;
char temp[100] = {0};


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
			"*********W5500开发板W5500 UDP CLIENT********\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	


	printf("UDP Local Port: %d \r\n",lport); 
	printf("UDP Remote IP: %d.%d.%d.%d \r\n",rip[0],rip[1],rip[2],rip[3]);
	printf("UDP Remote Port: %d \r\n",rport);
	printf("W5500 Init Complete!\r\n");
	printf("Start UDP Test!\r\n");	

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
					status = 0X80;
					oled_add_str("DHCP OK!");								
				break;
				case DHCP_RET_CONFLICT: 			
					while(1);
				default:
				break;
			}	

			DHCP_timer_handler();/* DHCP 超时定时器 */			
			delay_ms(10);/* 10ms扫描一次 */						
		}
		else if((status&0XC0) == 0X80)
		{			
			switch(getSn_SR(0)) /* 获取socket0的状态 */ 
			{
				case SOCK_UDP:	/* Socket处于初始化完成(打开)状态 */ 
					if(getSn_IR(0) & Sn_IR_RECV) 
							setSn_IR(0, Sn_IR_RECV);/* Sn_IR的RECV位置1 */	
			
					/* 数据回环测试程序：数据从上位机服务器发给W5500，W5500接收到数据后再回给服务器 */ 
					socket_len = getSn_RX_RSR(0); 									// len=Socket0接收缓存中已接收和保存的数据大小
					if(socket_len > 0)
					{				
						memset(socket_buf, 0, sizeof(socket_buf));
						/* W5500接收来自客户端的数据，并通过SPI发送给MCU */	
						recvfrom(0,socket_buf, socket_len, rip,&rport); 		
						printf("rec: %s\r\n", socket_buf);
						
						/* 接收到的数据在发送给服务器 */ 
						sendto(0,socket_buf,socket_len, rip, rport); 						
					}
			
					/* W5500向客户端发送数据  间隔1S */ 
					sprintf((char *)socket_buf, "udp time:%02d:%02d:%02d temp:%.1lf℃\n", calendar.hour,calendar.min,calendar.sec, get_tmp()/100.0);
					socket_len = strlen((char *)socket_buf);
					
					sprintf((char *)temp, "%02d:%02d:%02d tmp:%.1lf\n", calendar.hour,calendar.min,calendar.sec, get_tmp()/100.0);
					oled_add_str(temp);	
					printf("%s\n", socket_buf);
					sendto(0,socket_buf,socket_len, rip, rport);				
						
				break;
				case SOCK_CLOSED: /* Socket处于关闭状态 */																				
						/* 打开Socket0，并配置为UDP模式，打开一个本地端口 */ 
						socket(0,Sn_MR_UDP,lport,0);																	
				break;
			}	

			delay_ms(1000);/* 1000ms扫描一次 */			
		}	
		
		
	}
	 
}



