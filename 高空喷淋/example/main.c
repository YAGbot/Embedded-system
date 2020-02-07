#include <stdio.h>
#include "uart.h"
#include "delay.h"
#include "spi.h"
#include "socket.h"	// Just include one header for WIZCHIP
#include "w5500.h"
#include "config.h"
//#include "timer.h"
#include "mqtt.h"
#include "dhcp.h"
//#include "dns.h"
#include "json.h"
#include <stdlib.h>
#include <string.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

// Default Network Configuration
u8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};	
u8 lip[4] = {192, 168, 0, 101};
u8 sub[4] = {255, 255, 255, 0};
u8 gw[4] = {192, 168, 0, 1};

uint8_t domain_ip[4]={61,181,255,77};
//uint8_t domain_ip[4]={115,29,225,215};
uint8_t domain_name[]="www.embed-net.com";
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void network_init(void);								// Initialize Network information and display it

/**
  * @brief  ���ڴ�ӡ���
  * @param  None
  * @retval None
  */
int main(void)
{
	char temp[100] = {0};
	u8 status = 0;
	static char meassage[200];
  int rc;
  float temperature,humidity,light,pressure;	
	
	delay_init();		/* ��ʱ��ʼ�� */
	uart1_init(115200);	/* UART1��ʼ�� */
	spi1_init();		/* SPI1 ��ʼ�� */
	w5500_init();		/* оƬ��ʼ�� */
	
	w5500_net_init(mac, sub, gw, lip);
	memcpy(DHCP_GET.mac, mac, 6);
	init_dhcp_client(); 
	print("init dhcp ok!\r\n");
	close(SOCK_DHCP);
	
	sprintf((char *)temp,"MAC:%02X-%02X-%02X-%02X-%02X-%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	 srand(0);
	
	
  while(1)
	{
		
		if(status == 0)/* DHCP */
		{
			switch(check_DHCP_state(SOCK_DHCP))
			{
				case DHCP_RET_NONE: 					
				break;
				case DHCP_RET_TIMEOUT:	
					printf("dhcp timeout!\r\n"); 
					
				break;
				case DHCP_RET_UPDATE:					
					print(temp, " DHCP OK!\r\nIP:%d.%d.%d.%d\r\nGW:%d.%d.%d.%d\r\nSUB:%d.%d.%d.%d\r\n",
					DHCP_GET.lip[0],DHCP_GET.lip[1],DHCP_GET.lip[2],DHCP_GET.lip[3],
					DHCP_GET.gw[0],DHCP_GET.gw[1],DHCP_GET.gw[2],DHCP_GET.gw[3],
					DHCP_GET.sub[0],DHCP_GET.sub[1],DHCP_GET.sub[2],DHCP_GET.sub[3]);
					w5500_net_init(DHCP_GET.mac, DHCP_GET.sub, DHCP_GET.gw, DHCP_GET.lip);
					close(SOCK_DHCP);					
					/* ��W5500 Keepalive���� */
					setkeepalive(0);		
					
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
		
		
		else if((status&0XC0) == 0X80)
		{
			
//			switch(getSn_SR(0)) /* ��ȡsocket0��״̬ */ 
//			{
//				case SOCK_INIT: /* Socket���ڳ�ʼ�����(��)״̬ */
//						/* ����Sn_CRΪCONNECT������TCP������������������ */ 
//						//connect(0, rip, rport);			
//				printf("SOCKET CONNECT...\r\n");
//					connect(0, domain_ip, 10002);	
//				break;
//				case SOCK_ESTABLISHED:/* Socket�������ӽ���״̬ */ 
//						printf("SOCKET CONNECTOK...\r\n");										
//				break;
//				case SOCK_CLOSE_WAIT:/* Socket���ڵȴ��ر�״̬ */ 
//						/* �ر�Socket0 */ 
//						close(0);																						
//						printf("SOCKET CONNECT wait...\r\n"); 
//				break;
//				case SOCK_CLOSED:/* Socket���ڹر�״̬ */ 
//						/* ��Socket0��������ΪTCP����ʱģʽ����һ�����ض˿� */ 
//						printf("SOCKET CONNECT socket...\r\n");
//					socket(0,Sn_MR_TCP,5000,0x00);
//				break;
//			}
			
    memset(meassage,0,sizeof(meassage));
    //������Ϣ
//    rc = do_mqtt_subscrib("wizled",meassage);
//    printf("rc = %d\\r\n",rc);
//    
//    delay_ms(500);
////    //��ȡ�������������ݣ���ʾ��ʹ�������
////    temperature = rand()%50;
////    humidity = rand()%100;
////    light = rand()%1000;
////    pressure = rand()%1000;
////    //�����ݺϳ�ΪJSON��ʽ����
////    sprintf(meassage,"{\"temperature\":%.1f,\"humidity\":%.1f,\"light\":%.1f,\"pressure\":%.1f}",temperature,humidity,light,pressure);
////    //�����ݷ��ͳ�ȥ
    do_mqtt_publish("Topic",meassage);
		}
		delay_ms(1000);/* 1000msɨ��һ�� */
  }
}



/*********************************END OF FILE**********************************/
