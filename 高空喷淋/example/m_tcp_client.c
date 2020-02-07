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

u8 lip[4] = {192, 168, 0, 101};/* 本地IP */
u16 lport = 3000;/* 本地端口 */

//u8 rip[4] = {182, 92, 194, 8};/* 远程IP */
//u16 rport  = 1995;
u8 rip[4] = {61, 181, 255, 77};/* 远程IP */
u16 rport  = 41001;
//uint8_t domain_ip[4]={61,181,225,77};
//u16 timeout = 0;
u8 status = 0;
char temp[100] = {0};


int main(void)
{
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */  	
	delay_init();		/* 延时初始化 */
	uart2_init(115200);	/* UART1初始化 */
	uart1_init(9600);
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */ 	
	w5500_init();		/* 芯片初始化 */
	iocheck_init();	//IO初始化

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
	IWDG_Init(6,3750);    //与分频数为256,重载值为3750,溢出时间为24s
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
					/* 打开W5500 Keepalive功能 */
					setkeepalive(0);		
					
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
			/*Socket状态机，MCU通过读Sn_SR(0)的值进行判断Socket应该处于何种状态
			Sn_SR状态描述：
				0x00		SOCK_CLOSED
				0x13		SOCK_INIT
				0x14		SOCK_LISTEN
				0x17		SOCK_ESTABLISHED
				0x1C		SOCK_CLOSE_WAIT
				0x22		SOCK_UDP
			*/		
			switch(getSn_SR(0)) /* 获取socket0的状态 */ 
			{
				case SOCK_INIT: /* Socket处于初始化完成(打开)状态 */
						/* 配置Sn_CR为CONNECT，并向TCP服务器发出连接请求 */  
						connect(0, rip, rport);			
						oled_add_str("SOCKET CONNECT..."); 
				break;
				case SOCK_ESTABLISHED:/* Socket处于连接建立状态 */ 
						if(getSn_IR(0) & Sn_IR_CON) setSn_IR(0, Sn_IR_CON);/* Sn_IR的CON位置1，通知W5500连接已建立 */					
			
						/* 数据回环测试程序：数据从上位机服务器发给W5500，W5500接收到数据后再回给服务器 */ 
						socket_len = getSn_RX_RSR(0); 									// len=Socket0接收缓存中已接收和保存的数据大小
						if(socket_len > 0)
						{
							/*	W5500接收来自服务器的数据，并通过SPI发送给MCU */ 
							memset(socket_buf, 0, sizeof(socket_buf));
							//sprintf((char *)socket_buf, "%s", "REC:");
							
							/* W5500接收来自客户端的数据，并通过SPI发送给MCU */ 
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
							/* 接收到数据后再回给客户端，完成数据回环 */ 
							//send(0,(u8*)"over",4);		

							oled_add_str(socket_buf); 
						}
						get_data();
						/* W5500向客户端发送数据  间隔1S */ 
						sprintf((char *)socket_buf, Temp_str);
						socket_len = strlen((char *)socket_buf);
						
						sprintf((char *)temp, Temp_str);
						//oled_add_str("SOCKET"); 
						print("socket_buf=%s\r\n", socket_buf);
						sendto(0,socket_buf,socket_len, rip, rport);		
						IWDG_Feed();
						//printf("喂狗\r\n");
				break;
				case SOCK_CLOSE_WAIT:/* Socket处于等待关闭状态 */ 
						/* 关闭Socket0 */ 
						close(0);																						
						oled_add_str("SOCKET TCP CLOSE!"); 
				break;
				case SOCK_CLOSED:/* Socket处于关闭状态 */ 
						/* 打开Socket0，并配置为TCP无延时模式，打开一个本地端口 */ 
						socket(0,Sn_MR_TCP,lport,Sn_MR_ND);		
						oled_add_str("SOCKET TCP OPEN!"); 
				break;
			}

			delay_ms(2000);/* 1000ms扫描一次 */			
		}	
		
		
	}
	 
}



