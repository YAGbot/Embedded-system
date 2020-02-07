/***********************************************************************************
* @文件: at24cxx.h
* @作者: 吴永贵
* @时间: 2016.06.12
* @描述: 与AT24Cxx系列EEPROM相关   
************************************************************************************/
#ifndef __AT24CXX__H
#define __AT24CXX__H
#include "stm32f10x.h"

						
/*AT24Cxx写周期*/
#define AT24CXX_TWR		10      //写周期时间(擦除时间)ms

/*AT24CXX器件地址宏定义*/	 
#define AT24CXX_ADDR(x)   	(0xA0 | (x << 1))			//AT24C01-16:x为0~7
#define AT24CXX_SLA_W(x)  	((AT24CXX_ADDR(x)) | 0x00)	//器件地址+写控制位
#define AT24CXX_SLA_R(x)  	((AT24CXX_ADDR(x)) | 0x01)	//器件地址+读控制位

/*函数声明*/
void AT24Cxx_init(void);
void AT24Cxx_WriteByte(u8 SlaAddr, u16 ByteAddr, u8 data);
void AT24Cxx_WriteBuf(u8 SlaAddr, u16 ByteAddr, u8 *pBuf, u16 Len);
void AT24Cxx_ReadBuf(u8 SlaAddr, u16 ByteAddr, u8 *pBuf, u16 Len);
void AT24Cxx_WriteByte1(u8 SlaAddr, u16 ByteAddr,u8 *data);

#endif
