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

/* 屏幕字符buf */
u8 astr[4096] = {"1\n2\n3\n4\n1111111111111111111123455677\n"};
u16 add_len = 0;

/* 遇到\n 或者大于128换行 */
void oled_add(u8 * str)
{
	u8 x = 0;
	u8 y = 0;
	u8 line_cnt = 4;
	u8 line_str = 0;
	u8 len = 0;
	u8 sbuf[25]= {0};/* 存储一行的BUF */
	oled_clear();
	
	
	len = strlen(str)-1;
	u8 * p = &str[len];
	
	while(*p)
	{
		print("%c", *p);
		
		if(*p == '\n' || line_str >= 22)
		{
			oled_str(0,line_cnt*12,(const u8*)sbuf, 12);
			
			line_str = 0;/* 行计数情况 */
			if(!(line_cnt--))
			{
				break;
			}
		}

		
		p--;
		sbuf[line_str] = *p;
		line_str++;
	}
	
	oled_update();/* oled 显存区更新 */	
}

int main(void)
{
	char temp[100];
	
	//memset(add_str, 0, sizeof(add_str));
	
	 /*clk_init();        初始化系统时钟72M 已经在启动文件里
    面通过汇编调用初始化完成不需要在额外的初始化 */
	delay_init();		/* 延时初始化 */	
	uart1_init(115200);	/* UART1初始化 */
	spi1_init();		/* SPI1 初始化 */
	oled_init();		/* OLED初始化 */
	w5500_init();		/* 芯片初始化 */
	
	print( 	"******************************************\r\n"
			"*************杭州零芯电子*****************\r\n"
			"*********W5500开发板W5500 INIT************\r\n"
			"*****https://shop114042046.taobao.com*****\r\n"
			"******************************************\r\n");	


	setSHAR(mac);
	setSUBR(sub);
	setGAR(gw);
	setSIPR(lip);

	/* Init. TX & RX Memory size of w5500 */
	sysinit(txsize, rxsize); 
	setRTR(2000);
	setRCR(3);
	oled_add(astr);

#if 0
	print("w5500 net init:\r\n");
	getSIPR (t);
	print("SET IP : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
	sprintf(temp,"IP : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
	oled_str(0,0,(const u8*)temp, 12);
	
	getSUBR(t);
	print("SET SUB : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
	sprintf(temp,"SUB: %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
	oled_str(0,12,(const u8*)temp, 12);

	getGAR(t);
	print("GW : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
	sprintf(temp,"GW : %d.%d.%d.%d\r\n", t[0],t[1],t[2],t[3]);
	oled_str(0,24,(const u8*)temp, 12);

	oled_update();/* oled 显存区更新 */

	oled_str(0,36,"WARING DHCP TIMEOUT!", 12);
	oled_str(0,48,"WARINGDHCPTIMEOUT123!", 12);
	oled_update();/* oled 显存区更新 */		
#endif

	while(1)
	{
		
	}

}




























