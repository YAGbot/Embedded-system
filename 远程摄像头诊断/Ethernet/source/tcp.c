/**
  **************************************************************************************
  * @File      tcp.c
  * @Author    ChuanYong Wan
  * @brief   	 TCP Client functions base on TCP protocol.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *								 Without permission, shall not be reproduced and copied, shall not be 
	*								 used for commercial purposes.
  **************************************************************************************
 **/
 
/* Include ----------------------------------------------------------------------------*/
#include "string.h"
#include "delay.h"
#include "uart.h"
#include "wizchip_conf.h"
#include "w5500.h"
#include "socket.h"  
#include "mw5500.h"
#include "tcp.h"

/* Function ---------------------------------------------------------------------------*/
static uint16_t Get_BufLen(uint8_t buf[])
{
	uint16_t i = 0;
	while(buf[i]!='\0')
	{
		i++;
	}
	return i;
}
/*-------------------------------------------------------------------------------------*/
/**
*@brief  TCP client send a request to server
*@param  TCP_Client: Pointer variable of TCP client's struct
*@return 1:      TCP_OK
*				 Others: Error code
*/
int8_t TCP_SendRequest(TCP_ClientType *TCP_Client)
{
	uint8_t cnt = 3;
	uint8_t ret = 0;

	memset(TCP_Client->Rx_Buf, 0, sizeof(TCP_Client->Rx_Buf));
	TCP_Client->Tx_Len = Get_BufLen(TCP_Client->Tx_Buf);
	do													//Open a socket port
	{
		close(TCP_Client->SN);
		ret = socket(TCP_Client->SN, Sn_MR_TCP, TCP_Client->Local_Port, 0);
		delay_ms(50);
		cnt--;
	}while((ret!=TCP_Client->SN)&&cnt);
	if(ret!=TCP_Client->SN) 
	{
		close(TCP_Client->SN);
		return TCPERR_SOCKET;     //Socket open failed
	}
	delay_ms(300);              //Waiting for the completion of the socket init
	cnt = 20;
	debug("TCP: Socket open.....!\r\n");
	
	do                          //Waiting for the completion of the socket init
	{
		ret = getSn_SR(TCP_Client->SN);
		cnt--;
		delay_ms(50);
	}while((ret!=SOCK_INIT)&&cnt);
	if(ret!=SOCK_INIT) 
	{
		close(TCP_Client->SN);
		return TCPERR_SOCKET;     //Socket init failed
	}
	debug("TCP: Socket init success!\r\n");
	
	ret = connect(TCP_Client->SN, TCP_Client->Dest_IP, TCP_Client->Dest_Port);
	if(ret!=SOCK_OK) 
	{
		close(TCP_Client->SN);
		return TCPERR_CONNECT;   //Connect failed
	}
	delay_ms(300); 
	cnt = 20;
	debug("TCP: Connecting....!\r\n");
	
	do                         //Waiting for the completion of the connect
	{
		ret = getSn_SR(TCP_Client->SN);
		cnt--;
		delay_ms(50);
	}while((ret!=SOCK_ESTABLISHED)&&cnt);
	if(getSn_IR(TCP_Client->SN)&Sn_IR_CON) setSn_IR(TCP_Client->SN, Sn_IR_CON); 
	if(ret!=SOCK_ESTABLISHED) 
	{
		disconnect(TCP_Client->SN);
		close(TCP_Client->SN);
		return TCPERR_CONNECT;   //Connect failed
	}
	debug("TCP: Connect success!\r\n");
	
	ret = send(TCP_Client->SN, TCP_Client->Tx_Buf, TCP_Client->Tx_Len);
	if(ret<=0)
	{
		disconnect(TCP_Client->SN);
		close(TCP_Client->SN);
		return TCPERR_SEND;     //Send failed
	}
	cnt = 60;
	debug("TCP: Socket send:%s\r\n", TCP_Client->Tx_Buf);	
	
	do
	{
		delay_ms(50);	
		TCP_Client->Rx_Len = getSn_RX_RSR(TCP_Client->SN);
		cnt--;
	}while((TCP_Client->Rx_Len==0)&&cnt);	
	if(TCP_Client->Rx_Len>0)
	{
		recv(TCP_Client->SN, TCP_Client->Rx_Buf, TCP_Client->Rx_Len); 
		debug("TCP: Socket receive:%s\r\n", TCP_Client->Rx_Buf);			
	}
	else return TCPERR_TIMEOUT; //Time out error
	
	debug("TCP: Disconnect!\r\n");
	disconnect(TCP_Client->SN);
	close(TCP_Client->SN);
	if(TCP_Client->Rx_Buf[0]!='0') return TCPERR_RECEIVE; //Receive error
	return TCP_OK;
}
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

