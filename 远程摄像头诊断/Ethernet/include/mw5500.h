/**
  **************************************************************************************
  * @File      mw5500.h
  * @Author    ChuanYong Wan
  * @brief     Header file of w5500 API function.
  * @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *				     Without permission, shall not be reproduced and copied, shall not be used
  *			       for commercial purposes.
  **************************************************************************************
 **/
 
#ifndef __mw5500_H
#define __mw5500_H	

/* Include ----------------------------------------------------------------------------*/
#include "wizchip_conf.h"

/* Define -----------------------------------------------------------------------------*/
#define SOCKET0 0
#define SOCKET1 1
#define SOCKET2 2
#define SOCKET3 3
#define SOCKET4 4
#define SOCKET5 5
#define SOCKET6 6
#define SOCKET7 7

#define USE_DHCP 0

#define W5500_OK    1
#define W5500_ERROR 0
#define W5500ERR_PHYLINK (W5500_ERROR-1)
#define W5500ERR_BUFINIT (W5500_ERROR-2)
#define W5500ERR_DHCP    (W5500_ERROR-3)

/* Function Prototype -----------------------------------------------------------------*/
void SPI1_Init(void);
void SPI1_WriteByte(uint8_t TxData);
uint8_t SPI1_ReadByte(void);
void SPI1_CrisEnter(void);
void SPI1_CrisExit(void);
void SPI1_CS_Select(void);
void SPI1_CS_Deselect(void);
int8_t  W5500_Init(void);
#if USE_DHCP 
void IP_Assign(void);
void IP_Conflict(void);
#else
#endif

#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

