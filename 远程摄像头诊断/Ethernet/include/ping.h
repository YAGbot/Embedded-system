/**
  **************************************************************************************
  * @File      ping.h
  * @Author    ChuanYong Wan
  * @brief   	 Header files of Ping functions.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *						 Without permission, shall not be reproduced and copied, shall not be used
	*						 for commercial purposes.
  **************************************************************************************
 **/
 
#ifndef	_PING_H_
#define	_PING_H_

/* Include ----------------------------------------------------------------------------*/
#include "stdint.h"
#include "stdio.h"

/* Define -----------------------------------------------------------------------------*/
#define DATABUF_LEN 	 8
#define PING_REQUEST   8
#define PING_REPLY     0
#define CODE0 			   0
#define PING_PORT      3000
#define PING_SOCKETNUM SOCKET0

#define PING_OK          1
#define PING_ERROR       0
#define PINGERR_REQUEST (PING_ERROR-1)
#define PINGERR_REPLY   (PING_ERROR-2)
#define PINGERR_W5500   (PING_ERROR-3)
#define PINGERR_TIMEOUT (PING_ERROR-4)

/* Typedef ----------------------------------------------------------------------------*/
typedef struct PING_MSG
{
  uint8_t  Type; 		
  uint8_t  Code;		
  uint16_t Checksum;	
  uint16_t ID;	           
  uint16_t SeqNum; 	
  uint8_t  Data[DATABUF_LEN];
} PING_MSGType;

typedef struct IP_SET
{
	uint8_t Camera1[4];
	uint8_t Camera2[4];
	uint8_t Camera3[4];
	uint8_t Camera4[4];
	uint8_t Camera5[4];
	uint8_t Camera6[4];
	uint8_t Camera7[4];
	uint8_t NVR[4];
} IP_SETType;

typedef struct NET_INFO
{
	uint8_t Max_Delay;
	uint8_t Min_Delay;
	uint8_t Average_Delay;
	uint8_t Loss_Rate;
} NET_INFOType;

extern NET_INFOType Net_Info;

/* Function Prototype -----------------------------------------------------------------*/
int8_t Ping(uint8_t sn, uint16_t local_port, uint8_t *dest_ip, uint16_t dest_port, uint8_t *t);
uint8_t Ping_Multiple(IP_SETType *IP_Set);
void Ping_NetCheck(NET_INFOType *Net_Info, uint8_t *dest_ip, uint8_t time);

#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/
