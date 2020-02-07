#include "volmeter.h"
#include "stdio.h"
#include "usart.h"
#include "delay.h"
#include "rs485.h"
#include "string.h"
#include "gu900_uart.h"

 // CRC 高位字节值表
static const unsigned char s_CRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;
// CRC 低位字节值表
const unsigned char s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

//参与轮询从机地址
const unsigned char s_AddRoll[] = {
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
	0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
	0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,
	0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A
};

ELE_PAM g_ELE_PAM;
u8 SlaveAddr=1;
u8 RegNum;										//寄存器个数
u8 Reg03H;										//保存寄存器地址，方便数据分
u8 fAck06H;										//写配置标志位				
u8 fAck10H;										//清电能标志位	

u8 Write_Clear_buff[16]={0};			//写10H多个寄存器保持值buf[],写入的值根据buf[]内容修改

/************************************************************************************
* 函数名称：DevAddRoll
* 功能描述：从机地址轮询函数
* 入口参数：无
* 出口参数：无
* 备    注：0x01-0x20 共32个地址
*************************************************************************************/
int DevAddRoll(void)
{
	int try_num=3;
	int i;
	while(try_num)
	{
		for(i=0;i<32;i++)
		{
			MODH_Send03H(s_AddRoll[i],0x0000,1);
			delay_ms(100);
			if(rs485_recv_flag==1)
			{
				if(RS485_RX_BUF[0]==s_AddRoll[i])
				{
					SlaveAddr=s_AddRoll[i];
					rs485_recv_flag=0;
					return try_num;
				}
			}
		}
		try_num--;
	}
	return try_num;
}

/************************************************************************************
* 函数名称：Write_Data_Clear
* 功能描述：配置电流电压比函数
* 入口参数：无
* 出口参数：无
* 备    注：0x0001寄存器个数为1，0x0101电压比1电流比也为1
*************************************************************************************/
void Write_Data_Clear(void)
{
	MODH_Send10H(SlaveAddr,WriteEleClear,8,Write_Clear_buff);
	delay_ms(100);
	if(rs485_recv_flag==1)
	{
		MODH_Read_10H();
		rs485_recv_flag=0;
	}
	delay_ms(50);
}

/************************************************************************************
* 函数名称：Write_Config
* 功能描述：配置电流电压比函数
* 入口参数：无
* 出口参数：无
* 备    注：0x0001寄存器个数为1，0x0101电压比1电流比也为1
*************************************************************************************/
void Write_Config(void)
{
	//MODH_Send06H(SlaveAddr,WriteConfig,0x0001,0x013C);
	delay_ms(100);
	MODH_Read_06H();
}

/************************************************************************************
* 函数名称：Read_Config
* 功能描述：读采集仪配置函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void Read_Config(void)
{
	MODH_Send03H(SlaveAddr,ReadConfig,8);
	delay_ms(100);
	if(rs485_recv_flag==1)
	{
		MODH_Read_03H();
		rs485_recv_flag=0;
	}
	delay_ms(50);
}

/************************************************************************************
* 函数名称：Read_Parameters
* 功能描述：读采集仪电参数函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void Read_Parameters(void)
{
	MODH_Send03H(SlaveAddr,ReadParameters,17);
	delay_ms(200);
	if(rs485_recv_flag==1)
	{
		MODH_Read_03H();
		rs485_recv_flag=0;
	}
//	else
//	{
//		gu900_printf("####");
//	}
	delay_ms(50);
}

/************************************************************************************
* 函数名称：Read_ElectricEnergy
* 功能描述：读采集仪电参数函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void Read_ElectricEnergy(void)
{
	MODH_Send03H(SlaveAddr,ReadElectricEnergy,4);
	delay_ms(100);
	if(rs485_recv_flag==1)
	{
		MODH_Read_03H();
		rs485_recv_flag=0;
	}
	delay_ms(50);
}


/************************************************************************************
* 函数名称：CRC16_Modbus
* 功能描述：自动CRC16校验
* 入口参数：_pBuf：参与校验数据
						_usLen：数据长度
* 出口参数：16位校验值
*************************************************************************************/
u16 CRC16_Modbus(u8 *_pBuf, int _usLen)
{
  char ucCRCHi = 0xFF; /* 高CRC字节初始化 */
	char ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
	int usIndex;  /* CRC循环中的索引 */

  while (_usLen--)
  {
		usIndex = ucCRCHi ^ *_pBuf++; /* 计算CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
  }
    return ((u16)ucCRCHi << 8 | ucCRCLo);
}

/************************************************************************************
* 函数名称：Buf2Uint16
* 功能描述：将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
* 入口参数：_pBuf : 数组
* 出口参数：16位整数值
*************************************************************************************/
u16 Buf2Uint16(unsigned char* _pbuf)
{
  return (((u16)_pbuf[0]<<8)|_pbuf[1]);
}

/************************************************************************************
* 函数名称：Buf2Uint32
* 功能描述：将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
* 入口参数：_pBuf : 数组
* 出口参数：16位整数值
*************************************************************************************/
u32 Buf2Uint32(unsigned char* _pbuf)
{
  return (((u32)_pbuf[0]<<24)|((u32)_pbuf[1])<<16|((u16)_pbuf[2]<<8)|_pbuf[3]);
}

/************************************************************************************
* 函数名称：MODH_SendAckWithCRC
* 功能描述：发送应答，自动加CRC
* 入口参数：无
* 出口参数：无
*************************************************************************************/
static void MODH_SendAckWithCRC(void)
{
	u16 crc;
	crc = CRC16_Modbus(RS485_TX_BUF, RS485_TX_CNT);
	RS485_TX_BUF[RS485_TX_CNT++] = crc >> 8;
	RS485_TX_BUF[RS485_TX_CNT++] = crc;	
	//USART1_Send_Data(RS485_TX_BUF,RS485_TX_CNT);
	RS485_Send_Data(RS485_TX_BUF, RS485_TX_CNT);
}

/************************************************************************************
* 函数名称：MODH_Send10H
* 功能描述：发送10系列指令，自动加CRC
* 入口参数：_addr：丛机地址 
						_reg：寄存器编号
				 _num: 寄存器个数
						_buf：寄存器内值
* 出口参数：无
*************************************************************************************/
void MODH_Send10H(u8 _addr, u16 _reg, u8 _num, u8 *_buf)
{
	u16 i;
	RS485_TX_CNT = 0;
	RS485_TX_BUF[RS485_TX_CNT++] = _addr;		//从站地址
	RS485_TX_BUF[RS485_TX_CNT++] = 0x10;		//功能码	
	RS485_TX_BUF[RS485_TX_CNT++] = _reg >> 8;	//寄存器编号 高字节
	RS485_TX_BUF[RS485_TX_CNT++] = _reg;		//寄存器编号 低字节
	RS485_TX_BUF[RS485_TX_CNT++] = _num >> 8;	//寄存器个数编号 高字节
	RS485_TX_BUF[RS485_TX_CNT++] = _num;		//寄存器个数 低字节
	RS485_TX_BUF[RS485_TX_CNT++] = 2*_num;		//数据字节计数
	
	for(i=0;i<2*_num;i++)
	{
		RS485_TX_BUF[RS485_TX_CNT++] = _buf[i];		//写入后面的数据
	}
	MODH_SendAckWithCRC();		//发送数据，自动加CRC
}


/************************************************************************************
* 函数名称：MODH_Send06H
* 功能描述：发送06系列指令，自动加CRC
* 入口参数：_addr：丛机地址 
						_reg：寄存器编号
				 _regcnt: 寄存器个数
						_value：寄存器个数
* 出口参数：无
*************************************************************************************/
void MODH_Send06H(u8 _addr, u16 _reg, u16 _regcnt, u16 _value)
{
	RS485_TX_CNT = 0;
	RS485_TX_BUF[RS485_TX_CNT++] = _addr;		//从站地址
	RS485_TX_BUF[RS485_TX_CNT++] = 0x06;		//功能码	
	RS485_TX_BUF[RS485_TX_CNT++] = _reg >> 8;	//寄存器编号 高字节
	RS485_TX_BUF[RS485_TX_CNT++] = _reg;		//寄存器编号 低字节
	RS485_TX_BUF[RS485_TX_CNT++] = _regcnt >> 8;	//寄存器个数编号 高字节
	RS485_TX_BUF[RS485_TX_CNT++] = _regcnt;		//寄存器个数 低字节
	RS485_TX_BUF[RS485_TX_CNT++] = 0x02;		//字节计数
	RS485_TX_BUF[RS485_TX_CNT++] = _value >> 8;	//预至数据 高字节 
	RS485_TX_BUF[RS485_TX_CNT++] = _value;		//预至数据低字节 
	
	MODH_SendAckWithCRC();		//发送数据，自动加CRC
	fAck06H = 0;							//如果主机接收到应答，该标志为1
}

/************************************************************************************
* 函数名称：MODH_Send03H
* 功能描述：发送03系列指令，自动加CRC
* 入口参数：_addr：丛机地址 
						_reg：寄存器编号
						_num：寄存器个数
* 出口参数：无
*************************************************************************************/
void MODH_Send03H(u8 _addr, u16 _reg, u16 _num)
{
	RS485_TX_CNT = 0;
	RS485_TX_BUF[RS485_TX_CNT++] = _addr;		//从站地址
	RS485_TX_BUF[RS485_TX_CNT++] = 0x03;		//功能码	
	RS485_TX_BUF[RS485_TX_CNT++] = _reg >> 8;	//寄存器编号 高字节
	RS485_TX_BUF[RS485_TX_CNT++] = _reg;		//寄存器编号 低字节
	RS485_TX_BUF[RS485_TX_CNT++] = _num >> 8;	//寄存器个数 高字节 
	RS485_TX_BUF[RS485_TX_CNT++] = _num;		//寄存器个数 低字节 
	
	MODH_SendAckWithCRC();		//发送数据，自动加CRC
	
	RegNum = _num;		//寄存器个数 
	Reg03H = _reg;		//保存03H指令中的寄存器地址，方便对应答数据进行分类
}

/********************************************************************************************************
*	函 数 名: MODH_Read_06H
*	功能说明: 分析06H指令的应答数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
void MODH_Read_06H(void)
{
	u8 rs485buf[13];
	RS485_Receive_Data(rs485buf);
	if (rs485buf[0] == SlaveAddr)		
	{
		fAck06H = 1;		/* 接收到应答 */
	}
}

/********************************************************************************************************
*	函 数 名: MODH_Read_10H
*	功能说明: 分析10H指令的应答数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************/
void MODH_Read_10H(void)
{
	if (RS485_RX_BUF[0] == SlaveAddr)		
	{
		fAck10H = 1;		/* 接收到应答 */
	}
}

/************************************************************************************
* 函数名称：MODH_Read_03H
* 功能描述：MODBUS读03线圈函数
* 入口参数：无
* 出口参数：无
*************************************************************************************/
void MODH_Read_03H(void)
{
  u8 *p;
	switch(Reg03H)
	{
		case ReadConfig:
			if(CRC16_Modbus(RS485_RX_BUF,19)==(RS485_RX_BUF[19]<<8|RS485_RX_BUF[20]))  //判断CRC
			{
				p=&RS485_RX_BUF[7]; 								//数据开始地址
				g_ELE_PAM.U0 = Buf2Uint16(p); p+=2;							//电压量程
				g_ELE_PAM.I0 = Buf2Uint16(p); p+=4; 								//电流量程
				g_ELE_PAM.UBB = Buf2Uint16(p); p+=2; 								//电压变比
				g_ELE_PAM.IBB = Buf2Uint16(p);									 //电流变比
			}
			else printf("485");
				//USART1_Send_Data(RS485_RX_BUF,21);
			break;
		case ReadParameters:
			if(CRC16_Modbus(RS485_RX_BUF,37)==(RS485_RX_BUF[37]<<8|RS485_RX_BUF[38]))  //判断CRC
			{
				p=&RS485_RX_BUF[3]; 								//数据开始地址
				g_ELE_PAM.UA = Buf2Uint16(p); p+=2;							//A相电压
				g_ELE_PAM.UB = Buf2Uint16(p); p+=2; 								//B相电压
				g_ELE_PAM.UC = Buf2Uint16(p); p+=2; 								//C相电压
				g_ELE_PAM.IA = Buf2Uint16(p);	p+=2;							 //A相电流
				g_ELE_PAM.IB = Buf2Uint16(p);	p+=2;							 //B相电流
				g_ELE_PAM.IC = Buf2Uint16(p);	p+=2;							 //C相电流
				g_ELE_PAM.P = Buf2Uint16(p);	p+=2;							 //有功功率
				g_ELE_PAM.Q = Buf2Uint16(p);	p+=2;							 //无功功率
				g_ELE_PAM.S = Buf2Uint16(p);	p+=2;							 //视在功率
				g_ELE_PAM.PF = Buf2Uint16(p);	p+=10;							 //功率因数
				g_ELE_PAM.Pa = Buf2Uint16(p);	p+=2;							 //A相功率
				g_ELE_PAM.Pb = Buf2Uint16(p);	p+=2;							 //B相功率
				g_ELE_PAM.Pc = Buf2Uint16(p);							 			//C相功率
			}
			else printf("485");
				//USART1_Send_Data(RS485_RX_BUF,39);
			break;
		case ReadElectricEnergy:
			if(CRC16_Modbus(RS485_RX_BUF,11)==(RS485_RX_BUF[11]<<8|RS485_RX_BUF[12]))  //判断CRC
			{
				p=&RS485_RX_BUF[3]; 								//数据开始地址
				g_ELE_PAM.WH_P = Buf2Uint32(p); p+=4;							//正向有功电能
				g_ELE_PAM.WH_M = Buf2Uint32(p); 						//反向有功电能
			}
			else printf("485");
			//USART1_Send_Data(RS485_RX_BUF,13);
			break;
		default: break;				
	}
	memset(RS485_RX_BUF,0,64);
} 

