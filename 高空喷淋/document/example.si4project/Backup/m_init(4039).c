/**
  ******************************************************************************
  * @file    m_init.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   W5500初始化
  ******************************************************************************
  */
# include "w5500.h"
# include "uart.h"
# include "delay.h"
# include "oled.h"
# include "spi.h"
# include <string.h>

u8 txsize[MAX_SOCK_NUM] = {4,2,2,2,2,2,2,0};
u8 rxsize[MAX_SOCK_NUM] = {4,2,2,2,2,2,2,0};


u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};		
u8 lip[4] = {192, 168, 0, 101};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};
u8 dns[4] = {8, 8, 8, 8};
u8 t[4] = {0};




int main(void)
{
	char temp[100];
		
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */
	delay_init();		/* 延时初始化 */	
	uart1_init(115200);	/* UART1初始化 */
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */
	w5500_init();		/* 芯片初始化 */

#if 0
	print( 	"******************************************\r\n"
			"*************杭州零芯电子*****************\r\n"
			"*********W5500开发板W5500 INIT************\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	
#endif	

	
	setSHAR(mac);
	setSUBR(sub);
	setGAR(gw);
	setSIPR(lip);

	/* Init. TX & RX Memory size of w5500 */
	sysinit(txsize, rxsize); 
	setRTR(2000);
	setRCR(3);

	print("w5500 net init:\r\n");
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

	

	while(1)
	{
		
	}

}




























