/**
  ******************************************************************************
  * @file    m_ftp_client.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   FTP 客户端
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "dhcp.h"
# include "tftp.h"
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

/* DHCP采用自动获取 */
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u8 lip[4] = {192, 168, 10, 101};/* 本地IP */
u16 lport = 3000;/* 本地端口 */


u8 rip[4] = {192, 168, 10, 101};/* 远程IP */
u16 rport  = 69;

#define FILENAME "zerochip.txt"

u16 timeout = 0;
u8 status = 0;
u32 tftp_server = 0;/* 服务器IP缓冲区 */
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
			"*********W5500开发板W5500 FTP CLIENT********\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	

	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);	
	oled_add_str("TFTP CLIENT...");

	
	print("tftp client test!\r\n");
	TFTP_init(SOCK_TFTP,socket_buf);	
	tftp_server = (rip[0] << 24) | (rip[1] << 16) | (rip[2]<<8) | (rip[3]);
	print(" TFTP START %X\r\n", tftp_server);
	TFTP_read_request(tftp_server, FILENAME);
		
	sprintf(temp, "RIP:%d.%d.%d.%d", rip[0],rip[1],rip[2],rip[3]);
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
					sprintf(temp, "LIP:%d.%d.%d.%d", DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3]);
					oled_add_str(temp);									
					sprintf(temp, "LPORT:%d", lport);
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
		else if((status&0XC0) == 0X80)
		{						
			tftp_timeout_handler();
		
			u8 res = TFTP_run();	
			if(res == TFTP_FAIL)
			{
				print("TFTP FAIL\r\n");	
				close(SOCK_TFTP);
				status = 0;
				oled_add_str("TFTP FAIL!");	
			}
			if(res == TFTP_SUCCESS)
			{
				print("TFTP SUCCEED\r\n");	
				close(SOCK_TFTP);
				status = 0;
				oled_add_str("TFTP OK!");	
			}		
			delay_ms(100);/* 1000ms扫描一次 */			
		}	
	}	 
}



