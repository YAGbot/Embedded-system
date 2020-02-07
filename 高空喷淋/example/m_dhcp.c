/**
  ******************************************************************************
  * @file    m_dhcp.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   DHCP自动获取IP
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "socket.h"
# include "oled.h"
# include "spi.h"
# include "config.h"
# include <string.h>


u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};		
u8 lip[4] = {192, 168, 0, 101};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

int main(void)
{
	char temp[100] = {0};
	u8 t[8] = {0};
	
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */
	delay_init();		/* 延时初始化 */
	uart1_init(115200);	/* UART1初始化 */
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */
	w5500_init();		/* W5500芯片初始化 */

	print( 	"******************************************\r\n"
			"*************杭州零芯电子*****************\r\n"
			"*********W5500开发板W5500 DHCP************\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");	
	sprintf((char *)temp,"MAC:%02X-%02X-%02X-%02X-%02X-%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	oled_add_str(temp);	
	oled_add_str("DHCP START...");
	
	while(1)
	{		
		switch(check_DHCP_state(SOCK_DHCP))/* 检测DHCP状态 */
		{
			case DHCP_RET_NONE: 					
			break;
			case DHCP_RET_TIMEOUT:	
				print("dhcp timeout!\r\n");
				oled_add_str("!WARING TIMEOUT");		
			break;
			case DHCP_RET_UPDATE:					
				sprintf(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
				DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],			
				DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
				DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
			
				w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
			
				getSIPR (t);
				print("SET IP : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
				sprintf(temp,"IP : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
				oled_add_str(temp);

				getSUBR(t);
				print("SET SUB : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
				sprintf(temp,"SUB: %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
				oled_add_str(temp);
			
				getGAR(t);
				print("GW : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
				sprintf(temp,"GW : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
				oled_add_str(temp);			
				
				sprintf((char *)temp,"MAC:%02X-%02X-%02X-%02X-%02X-%02X\r\n", DHCP_GET.mac[0],DHCP_GET.mac[1],DHCP_GET.mac[2],DHCP_GET.mac[3],DHCP_GET.mac[4],DHCP_GET.mac[5]);
				oled_add_str(temp);
	
				close(0);								
			break;
			case DHCP_RET_CONFLICT: 			
				while(1);
			default:
			break;
		}	

		DHCP_timer_handler();/* DHCP 超时定时器 */		

		delay_ms(10);/* 延时10ms */		
	}

	
	
	//return 0;
}




























