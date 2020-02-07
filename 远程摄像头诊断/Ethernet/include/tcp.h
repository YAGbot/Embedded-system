/**
  **************************************************************************************
  * @File      TCP.h
  * @Author    ChuanYong Wan
  * @brief   	 Header files of TCP functions.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *						 Without permission, shall not be reproduced and copied, shall not be used
	*						 for commercial purposes.
  **************************************************************************************
 **/
 
#ifndef	_TCP_H_
#define	_TCP_H_

/* Include ----------------------------------------------------------------------------*/
#include "stdint.h"

/* Define -----------------------------------------------------------------------------*/
#define TCP_BUF_LEN 10

#define TCP_OK          1
#define TCP_ERROR       0
#define TCPERR_SOCKET   (TCP_ERROR-1)
#define TCPERR_CONNECT  (TCP_ERROR-2)
#define TCPERR_SEND			(TCP_ERROR-3)
#define TCPERR_TIMEOUT  (TCP_ERROR-4)
#define TCPERR_RECEIVE	(TCP_ERROR-5)

/* Typedef ----------------------------------------------------------------------------*/
typedef struct TCP_CLIENT
{
	uint8_t  SN;									//Socket number
	
	uint8_t  Net_Sta;							//Net state
	
	uint8_t  Tx_Buf[TCP_BUF_LEN]; //Tx buffer
	
	uint8_t  Rx_Buf[TCP_BUF_LEN]; //Rx buffer
	
	uint16_t Local_Port;          //Client's port
	
	uint8_t  Dest_IP[4];	        //Destination's IP
	
	uint16_t Dest_Port;           //Destination's port
	
	uint16_t Tx_Len;							//Tx data's length
	
	uint16_t Rx_Len;							//Rx data's length
	
} TCP_ClientType;

/* Function Prototype -----------------------------------------------------------------*/
int8_t TCP_SendRequest(TCP_ClientType *TCP_Client);

#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

 
