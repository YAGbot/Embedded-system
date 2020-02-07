#ifndef __BREAK_H
#define __BREAK_H
	
#include "sys.h"

void CLR_485_Buf(void);
void Control_init(void);
void Restart_break(void);
u16 CRC16_Modbus(u8 *_pBuf, int _usLen);
static void MODH_SendAckWithCRC(void);
void MODH_Send03H(u8 _addr, u16 _reg, u16 _num);

#endif
