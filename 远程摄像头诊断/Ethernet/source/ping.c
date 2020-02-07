/**
  **************************************************************************************
  * @File      ping.c
  * @Author    ChuanYong Wan
  * @brief   	 Ping functions base on ICMP protocol.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *								 Without permission, shall not be reproduced and copied, shall not be 
	*								 used for commercial purposes.
  **************************************************************************************
 **/
 
/* Include ----------------------------------------------------------------------------*/
#include "ping.h"
#include "string.h"
#include "delay.h"
#include "uart.h"
#include "wizchip_conf.h"
#include "w5500.h"
#include "socket.h"  
#include "mw5500.h"

NET_INFOType Net_Info;

/* Variable ---------------------------------------------------------------------------*/
static uint16_t RandomID     = 0x1234; 
static uint16_t RandomNum    = 0x4321;

/* Function ---------------------------------------------------------------------------*/
/**
*@brief  Convert a host mode unsigned short data(little-endian) 
*				 to a TCP/IP network byte format data(Big-endian).
*@param  host_data: Host data(little-endian) 
*@return TCP/IP network byte format data(Big-endian).  
*/ 
static uint16_t htons(uint16_t host_data)
{
	uint16_t temp = 0;
	#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )
		temp  = (host_data&0xFF)<< 8;
		temp |= ((host_data>>8)&0xFF);
		return temp;	
	#else
		return host_data;
	#endif		
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Caculate checksum
*@param  src: Pointer to stream 
*@param  len: Length of stream 
*@return Checksum(16bit) 
*/ 
static uint16_t CheckSum(uint8_t *src, 	uint16_t len)
{
	uint16_t Sum=0, Sum16=0, i=0, j=0;
	uint32_t Sum32=0;

	j = len>>1; 		 //Equal to j = j/2;
	for(i=0;i<j;i++) 
	{
		Sum16  = src[i*2]; 
		Sum16  = Sum16<<8;   //High byte
		Sum16 += src[i*2+1]; //High byte + Low byte
		Sum32 += Sum16;
	}
	if(len%2) 
	{
		Sum16  = src[i*2];
		Sum32 += (Sum16<<8);
	}
	Sum = (uint16_t)Sum32;
	Sum = ~(Sum+(Sum32>>16));
	
	return (uint16_t)Sum;	
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Send ping ip request
*@param  sn:   Socket number(0) 
*@param  ip:   Pointer variable of destination IP address.  
*@param  port: Destination port number.
*@return 1:      Pin_OK
*				 Others: Error code
*/ 
static int8_t Ping_SendRequest(uint8_t sn, uint8_t *ip, uint16_t port)
{
	int8_t ret = 0;
	uint16_t i = 0;
	PING_MSGType PING_MSG;
        											
	PING_MSG.Type    = PING_REQUEST;                  	
	PING_MSG.Code    = CODE0;	                   				
	PING_MSG.ID      = htons(RandomID++);	       						
	PING_MSG.SeqNum  = htons(RandomNum++);							
	for(i=0;i<(DATABUF_LEN);i++) PING_MSG.Data[i] = (i)%8;	//'0'~'8'

	PING_MSG.Checksum = 0; 														      //Init 0
	PING_MSG.Checksum = htons(CheckSum((uint8_t*)&PING_MSG, sizeof(PING_MSG))); 
	ret = sendto(sn, (uint8_t *)&PING_MSG, sizeof(PING_MSG), ip, port);
	if(ret<=0)	return PINGERR_REQUEST;

	return PING_OK;
} 
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Receive ping reply
*@param  sn:   Socket number(0) 
*@param  ip:   IP address 
*@param  port: Destination port number. 
*@param  len:  Length of receive data
*@return 1 :     Pin_OK
*				 others: Error code 
*/ 
static int8_t Ping_RecvReply(uint8_t sn, uint8_t *ip, uint16_t port, uint16_t len)
{
//	uint16_t Check_Sum;
	uint8_t buf[DATABUF_LEN+10];
	uint16_t Rx_Len=0;//, i=0;
//	PING_MSGType Ping_Msg;
	
	memset(buf, 0, sizeof(buf));
	Rx_Len = recvfrom(sn, (uint8_t *)buf, len, ip, &port);		
	if((buf[0]==PING_REPLY)&&(Rx_Len==DATABUF_LEN+8)) 
	{
//		for(i=0; i<Rx_Len-8 ; i++) debug("Ping: reply data: %#X\r\n", buf[i]);
//		Ping_Msg.Type 		 = buf[0];
//		Ping_Msg.Code 		 = buf[1];
//		Ping_Msg.Checksum  = (buf[3]<<8) + buf[2];
//		Ping_Msg.ID 		   = (buf[5]<<8) + buf[4];
//		Ping_Msg.SeqNum 	 = (buf[7]<<8) + buf[6];
//		
//		for(i=0; i<Rx_Len-8; i++) Ping_Msg.Data[i] = buf[8+i];
//		Check_Sum = ~CheckSum(buf,Rx_Len);								
//		if(Check_Sum) debug("Check Sum = %#x\r\n", Check_Sum);
//		else
//		{
//			debug("Ping: reply from %d.%d.%d.%d: ID=%#X, Rx Bytes=%d\r\n",
//			(ip[0]), (ip[1]), (ip[2]), (ip[3]), htons(Ping_Msg.ID), (Rx_Len+6));																		
//		}
		return PING_OK;
	}
//	else if(buf[0]==Ping_Request)
//	{
//		Ping_Msg.Code 	  = buf[1];
//		Ping_Msg.Type 	  = buf[2];
//		Ping_Msg.Checksum = (buf[3]<<8) + buf[2];
//		Ping_Msg.ID 		  = (buf[5]<<8) + buf[4];
//		Ping_Msg.SeqNum 	    = (buf[7]<<8) + buf[6];		
//		for(i=0; i<Rx_Len-8 ; i++)
//		{
//			Ping_Msg.Data[i] = buf[8+i];
//		}
//		Check_Sum = Ping_Msg.Checksum;									
//		Ping_Msg.Checksum = 0;
//		if(Check_Sum!= Ping_Msg.Checksum)
//		{
//		}
//		else
//		{		
//		}
//		printf("Request from %d.%d.%d.%d: ID=%x, SeqNum=%x, size=%d bytes\r\n",
//		(ip[0]), (ip[1]), (ip[2]), (ip[3]), (Ping_Msg.ID), (Ping_Msg.SeqNum), (Rx_Len+6));	   														
//	}

	return PINGERR_REPLY;
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Ping IP
*@param  sn:         Socket number(0) 
*@param  local_port: Local port number.
*@param  dest_ip:    Pointer variable of destination IP address.
*@param  dest_port:  Destination port number.
*@param  t:  				 Net delay(ms).
*@return 1:      Ping_OK
*				 Others: Error code
*/ 
int8_t Ping(uint8_t sn, uint16_t local_port, uint8_t *dest_ip, uint16_t dest_port, uint8_t *t)
{
	int8_t  ret = 0;
	uint8_t cnt = 3;
	int32_t len = 0;
	 
	ret = getSn_SR(sn);
	if(ret!=SOCK_IPRAW) 
	{
		do
		{
			close(sn);
			IINCHIP_WRITE(Sn_PROTO(sn), IPPROTO_ICMP);    //Set ICMP protocol
			ret = socket(sn, Sn_MR_IPRAW, local_port, 0); 
			delay_ms(50);
			cnt--;
		}while((ret!=sn)&&cnt);
		if(ret!=sn) return PINGERR_W5500;
		cnt  = 15;
		
		do																							//Waiting to enter IP Raw mode
		{		
			ret = getSn_SR(sn);
			delay_ms(10);
			cnt--;
		}while((ret!=SOCK_IPRAW)&&cnt);
		if(ret!=SOCK_IPRAW)
		{
			close(sn);
			return PINGERR_W5500;
		}
	}
	cnt = 0;
	
	if(ret==SOCK_IPRAW) 
	{	
		ret = Ping_SendRequest(sn, dest_ip, dest_port);		
		if(ret==PING_OK)
		{
			while(1)
			{	
				len = getSn_RX_RSR(sn);
				if(len>0) 
				{
					ret = Ping_RecvReply(sn, dest_ip, dest_port, len);	
					*t = cnt;
					break;					
				}
				if(cnt>200) 
				{
					*t = cnt;
					ret = PINGERR_TIMEOUT;
					break;
				}
				cnt++;
				delay_ms(1);
			}
		}
	}
	return ret;
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Ping 8 devices via socket 0 port and 3000 tcp port
*@param  IP_Set: Pointer variable of IP_Set's struct
*@return :       Result
*/
uint8_t Ping_Multiple(IP_SETType *IP_Set)
{
	uint8_t ret=0, temp=0, cnt=0, t=0;
	if(IP_Set->Camera1[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->Camera1, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<0;
	
	if(IP_Set->Camera2[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->Camera2, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<1;
	
	if(IP_Set->Camera3[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->Camera3, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<2;
	
	if(IP_Set->Camera4[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->Camera4, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<3;
	
	if(IP_Set->Camera5[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->Camera5, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<4;
	
	if(IP_Set->Camera6[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->Camera6, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<5;
	
	if(IP_Set->Camera7[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->Camera7, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<6;
	
	if(IP_Set->NVR[0]!=0)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, IP_Set->NVR, PING_PORT, &t);
		if(ret!=PING_OK) ret = 0;
	}
	else 
	{
		ret = 1;
		cnt++;
	}
	temp |= ret<<7;
	
	if(cnt==8) return 0x00;
	return temp;
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  Check packet loss rate and delay time of net.
*@param  Net_Info: Pointer variable of Net_Info's struct.
*@param  dest_ip:  Pointer variable of destination IP address.
*@param  time:     Check continued time(0.5s).
*@return None
*/
void Ping_NetCheck(NET_INFOType *Net_Info, uint8_t *dest_ip, uint8_t time)
{
	uint8_t i=0, ret=0, t=0, cnt=0, delay=0;
	for(i=0;i<time;i++)
	{
		ret  = Ping(PING_SOCKETNUM, PING_PORT, dest_ip, PING_PORT, &t);
		if(ret==PING_OK)
		{
			delay += t;
			cnt++;
		}
	}
	Net_Info->Loss_Rate     = (time-cnt)*100/time;
	Net_Info->Average_Delay = delay/cnt;
}
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/





