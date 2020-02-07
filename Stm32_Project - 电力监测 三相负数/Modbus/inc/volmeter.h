#ifndef __VOLMETER__H
#define __VOLMETER__H
#include "stm32f10x.h"

/*变量声明*/
typedef struct
{
	u16 U0;					//电压量程
	u16 I0;					//电流量程
	u16 UBB;					//电压变比
	u16 IBB;					//电流变比
	u16 UA;					//A相电压
	u16 UB;					//B相电压
	u16 UC;					//C相电压
	u16 IA;					//A相电流
	u16 IB;					//B相电流
	u16 IC;					//C相电流
	int16_t P;					//有功功率
	int16_t Q;					//无功功率
	u16 S;					//视在功率
	int16_t PF;					//功率因数
	int16_t Pa;					//A相功率
	int16_t Pb;					//B相功率
	int16_t Pc;					//C相功率
	u32 WH_P;				//正向有功总电能
	u32 WH_M;				//反向有功总电能
}ELE_PAM;

extern u8 SlaveAddr;									//从机地址

#define WriteEleClear				0x000C		//电能数据清零起始地址			8个寄存器
#define ReadConfig  				0x0000		//读配置寄存器开始地址   8个寄存器
#define ReadParameters  		0x0040		//读各项电参数开始地址		17个寄存器
#define ReadElectricEnergy	0X000C    //读各项电能度数起始地址	4个寄存器

extern u8 RegNum;										//寄存器个数
extern u8 Reg03H;										//保存寄存器地址，方便数据分类

extern ELE_PAM g_ELE_PAM;

/*函数声明*/
int DevAddRoll(void);
void Write_Data_Clear(void);
void Write_Config(void);
void Read_Config(void);
void Read_Parameters(void);
void Read_ElectricEnergy(void);
u16 CRC16_Modbus(u8 *_pBuf, int _usLen);
u16 Buf2Uint16(unsigned char* _pbuf);
u32 Buf2Uint32(unsigned char* _pbuf);
static void MODH_SendAckWithCRC(void);
void MODH_Send10H(u8 _addr, u16 _reg, u8 _num, u8 *_buf);
void MODH_Send06H(u8 _addr, u16 _reg, u16 _regcnt, u16 _value);
void MODH_Send03H(u8 _addr, u16 _reg, u16 _num);
void MODH_Read_03H(void);
void MODH_Read_06H(void);
void MODH_Read_10H(void);

#endif
