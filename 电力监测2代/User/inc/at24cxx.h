/***********************************************************************************
* @�ļ�: at24cxx.h
* @����: ������
* @ʱ��: 2016.06.12
* @����: ��AT24Cxxϵ��EEPROM���   
************************************************************************************/
#ifndef __AT24CXX__H
#define __AT24CXX__H
#include "stm32f10x.h"

						
/*AT24Cxxд����*/
#define AT24CXX_TWR		10      //д����ʱ��(����ʱ��)ms

/*AT24CXX������ַ�궨��*/	 
#define AT24CXX_ADDR(x)   	(0xA0 | (x << 1))			//AT24C01-16:xΪ0~7
#define AT24CXX_SLA_W(x)  	((AT24CXX_ADDR(x)) | 0x00)	//������ַ+д����λ
#define AT24CXX_SLA_R(x)  	((AT24CXX_ADDR(x)) | 0x01)	//������ַ+������λ

/*��������*/
void AT24Cxx_init(void);
void AT24Cxx_WriteByte(u8 SlaAddr, u16 ByteAddr, u8 data);
void AT24Cxx_WriteBuf(u8 SlaAddr, u16 ByteAddr, u8 *pBuf, u16 Len);
void AT24Cxx_ReadBuf(u8 SlaAddr, u16 ByteAddr, u8 *pBuf, u16 Len);
void AT24Cxx_WriteByte1(u8 SlaAddr, u16 ByteAddr,u8 *data);

#endif
