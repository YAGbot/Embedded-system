/**
  **************************************************************************************
  * @File      mw5500.c
  * @Author    ChuanYong Wan
  * @brief     w5500 API function.
  * @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *				     Without permission, shall not be reproduced and copied, shall not be used
  *			       for commercial purposes.
  **************************************************************************************
 **/
 
/* Include ----------------------------------------------------------------------------*/
#include "stdint.h"
#include "delay.h"
#include "uart.h"
#include "w5500.h"
#include "socket.h"  
#include "switch.h"
#include "mw5500.h"
#include "wizchip_conf.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"  
#include "stm32f10x_gpio.h"
#if USE_DHCP                       
#include "dhcp.h"
#else
#endif

/* Variable ---------------------------------------------------------------------------*/
#if USE_DHCP                         //Use dynamic host configuration protocol	
uint8_t gDHCP_Buf[512];
#else
#endif

/* Function ---------------------------------------------------------------------------*/
void SPI1_Init(void)
{
	SPI_InitTypeDef  SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

	GPIO_InitStruct.GPIO_Pin   =  GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode              = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize          = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL              = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA              = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS               = SPI_NSS_Soft;
	SPI_InitStruct.SPI_FirstBit          = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial     = 7;
	SPI_Init(SPI1,&SPI_InitStruct);
	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
	
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5; //Reset pin		  
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		    		
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;		     
  GPIO_Init(GPIOC, &GPIO_InitStruct);		
  GPIO_SetBits(GPIOC, GPIO_Pin_5);		
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_4; //INT Pin		      
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		    	
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;				  		
  GPIO_Init(GPIOC, &GPIO_InitStruct);
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Write 1 Byte data via SPI bus
  * @param  TxData: Data to write
  * @retval None
  */
void SPI1_WriteByte(uint8_t TxData)
{				 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);	  
	SPI1->DR=TxData;	 	  									
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0);
	SPI1->DR;		
}
/*-------------------------------------------------------------------------------------*/
uint8_t SPI1_ReadByte(void)
{			 
	while((SPI1->SR&SPI_I2S_FLAG_TXE)==0);		  
	SPI1->DR=0xFF;	 	  										 
	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==0);  
	return SPI1->DR;  						    
}
/*-------------------------------------------------------------------------------------*/
void SPI1_CrisEnter(void)
{
	__set_PRIMASK(1);
}
/*-------------------------------------------------------------------------------------*/
void SPI1_CrisExit(void)
{
	__set_PRIMASK(0);
}
/*-------------------------------------------------------------------------------------*/
void SPI1_CS_Select(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}
/*-------------------------------------------------------------------------------------*/
void SPI1_CS_Deselect(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
/*-------------------------------------------------------------------------------------*/
#if USE_DHCP                       
void IP_Assign(void)
{
	wiz_NetInfo Net_Info;  
	debug("IP assign\r\n");
  getIPfromDHCP(Net_Info.ip);
  getGWfromDHCP(Net_Info.gw);
  getSNfromDHCP(Net_Info.sn);
  getDNSfromDHCP(Net_Info.dns);
  Net_Info.dhcp = NETINFO_DHCP;
  wizchip_setnetinfo(&Net_Info);
}
/*-------------------------------------------------------------------------------------*/
void IP_Conflict(void)
{
	debug("IP conflict\r\n");
  GPIO_ResetBits(GPIOB, GPIO_Pin_8);//W5500 hardware reset
	delay_us(600);  
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
	delay_us(200);
}
/*-------------------------------------------------------------------------------------*/
#else
#endif
/**
  * @brief  Init W5500
  * @param  None
  * @retval 1:      W5500_OK
	*					others: Error code
  */
int8_t W5500_Init(void)
{	
	int8_t  ret = 0;
	uint8_t Tx_Size[8] = {2,2,2,2,2,2,2,2};
  uint8_t Rx_Size[8] = {2,2,2,2,2,2,2,2};
	uint16_t i = 0;
	
	wiz_NetTimeout Net_Time = { .retry_cnt = 2,
															.time_100us = 200
														};
	wiz_NetInfo Net_Info = { .mac  = {0x00,0x90,0xF5,0xF8,0x8A,0xE5},
													 .ip   = {17,13,243,253},
													 .sn   = {255,255,255,0},
													 .gw   = {17,13,243,1},
													 .dns  = {8,8,8,8},
													 .dhcp = NETINFO_STATIC };
	 
	SPI1_Init();
	reg_wizchip_cris_cbfunc(SPI1_CrisEnter, SPI1_CrisExit);	 
	reg_wizchip_cs_cbfunc(SPI1_CS_Select, SPI1_CS_Deselect);
	reg_wizchip_spi_cbfunc(SPI1_ReadByte, SPI1_WriteByte); 
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);//W5500 hardware reset
	delay_us(600);  
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
	delay_us(200);
	
	while((wizphy_getphylink()!=PHY_LINK_ON)&&(i<150)) 
	{
		delay_ms(10);
		i++;
	}
	if(i>=150) ret = W5500ERR_PHYLINK;
	if(wizchip_init(Tx_Size, Rx_Size)!=0) ret = W5500ERR_BUFINIT;
	
	#if USE_DHCP                         //Use dynamic host configuration protocol		
		i = 0;
		setSHAR(gNet_Info.mac);
		DHCP_init(SOCKET0, gDHCP_Buf);
		reg_dhcp_cbfunc(IP_Assign, IP_Assign, IP_Conflict);
		while((DHCP_run()!=DHCP_IP_LEASED)&&(i<20))
		{
			delay_ms(100);
			i++;
		}
		if(i>=20) return W5500ERR_DHCP;
	#else 
//		Net_Info.gw[2] = Switch_Line1Read();
//		Net_Info.ip[2] = Switch_Line1Read();
//		Net_Info.ip[3] = Switch_Line2Read();
		wizchip_setnetinfo(&Net_Info); //Use static host configuration protocol
		wizchip_settimeout(&Net_Time);
	#endif
	
	if(ret!=0) return ret;
  return W5500_OK;
}
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

