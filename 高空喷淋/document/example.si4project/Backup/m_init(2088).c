/**
  ******************************************************************************
  * @file    m_init.c
  * @author  BUG
  * @version V1.0.0
  * @date    2018/9/18
  * @brief   W5500��ʼ��
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

/* ��Ļ�ַ�buf */
u8 add_str[4096] = {0};
u16 add_len = 0;
/* ����\n ���ߴ���128���� */
void oled_add(u8 * str)
{
	u8 x = 0;
	u8 y = 0;
	static u8 len = 0;
	
	sprintf(&add_str[add_len], "%s", str);
	u8 * p = &add_str[add_len];
	
	len = strlen((char *)str);
	xlen += len;
	
	oled_clear();
	
	while(*p != 0)
	{
		if(x > 122)/* �ַ���ʾ����һ���� */
		{				   
			y += 12;/* �ƶ�����һ�� */
			x = 0;/* x�����ͷ��ʾ */	   
		}		
		
		if(y > 52)break;
		
		if(*p == '\n')/* ���з� */
		{         
			y += 12; /* �ƶ�����һ�� */
			x = 0;/* x�����ͷ��ʾ */	
			p++;/* �ַ���ַ��λ */ 
		}  

		oled_char(x,y,*p,12,1);/* ��ʾ�ַ�*/ 
		
		p++; 
		x += 6;
	}
	
	//x -= 6;
	
	oled_update();/* oled �Դ������� */	
}

int main(void)
{
	char temp[100];
	
	memset(add_str, 0, sizeof(add_str));
	
	 /*clk_init();        ��ʼ��ϵͳʱ��72M �Ѿ��������ļ���
    ��ͨ�������ó�ʼ����ɲ���Ҫ�ڶ���ĳ�ʼ�� */
	delay_init();		/* ��ʱ��ʼ�� */	
	uart1_init(115200);	/* UART1��ʼ�� */
	spi1_init();		/* SPI1 ��ʼ�� */
	oled_init();		/* OLED��ʼ�� */
	w5500_init();		/* оƬ��ʼ�� */
	
	print( 	"******************************************\r\n"
			"*************������о����*****************\r\n"
			"*********W5500������W5500 INIT************\r\n"
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
	oled_add("331\n4345454\n");
	oled_add("hello\n");
	oled_add("mengxianya\n");
	oled_add("123\n");
	oled_add("qwerty\n");
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

	oled_update();/* oled �Դ������� */

	oled_str(0,36,"WARING DHCP TIMEOUT!", 12);
	oled_str(0,48,"WARINGDHCPTIMEOUT123!", 12);
	oled_update();/* oled �Դ������� */		
#endif

	while(1)
	{
		
	}

}




























