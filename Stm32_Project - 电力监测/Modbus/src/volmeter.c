#include "volmeter.h"
#include "stdio.h"
#include "usart.h"
#include "delay.h"
#include "rs485.h"
#include "string.h"
#include "gu900_uart.h"

 // CRC ��λ�ֽ�ֵ��
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
// CRC ��λ�ֽ�ֵ��
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

//������ѯ�ӻ���ַ
const unsigned char s_AddRoll[] = {
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
	0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
	0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,
	0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A
};

ELE_PAM g_ELE_PAM;
u8 SlaveAddr=1;
u8 RegNum;										//�Ĵ�������
u8 Reg03H;										//����Ĵ�����ַ���������ݷ�
u8 fAck06H;										//д���ñ�־λ				
u8 fAck10H;										//����ܱ�־λ	

u8 Write_Clear_buff[16]={0};			//д10H����Ĵ�������ֵbuf[],д���ֵ����buf[]�����޸�

/************************************************************************************
* �������ƣ�DevAddRoll
* �����������ӻ���ַ��ѯ����
* ��ڲ�������
* ���ڲ�������
* ��    ע��0x01-0x20 ��32����ַ
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
* �������ƣ�Write_Data_Clear
* �������������õ�����ѹ�Ⱥ���
* ��ڲ�������
* ���ڲ�������
* ��    ע��0x0001�Ĵ�������Ϊ1��0x0101��ѹ��1������ҲΪ1
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
* �������ƣ�Write_Config
* �������������õ�����ѹ�Ⱥ���
* ��ڲ�������
* ���ڲ�������
* ��    ע��0x0001�Ĵ�������Ϊ1��0x0101��ѹ��1������ҲΪ1
*************************************************************************************/
void Write_Config(void)
{
	//MODH_Send06H(SlaveAddr,WriteConfig,0x0001,0x013C);
	delay_ms(100);
	MODH_Read_06H();
}

/************************************************************************************
* �������ƣ�Read_Config
* �������������ɼ������ú���
* ��ڲ�������
* ���ڲ�������
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
* �������ƣ�Read_Parameters
* �������������ɼ��ǵ��������
* ��ڲ�������
* ���ڲ�������
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
* �������ƣ�Read_ElectricEnergy
* �������������ɼ��ǵ��������
* ��ڲ�������
* ���ڲ�������
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
* �������ƣ�CRC16_Modbus
* �����������Զ�CRC16У��
* ��ڲ�����_pBuf������У������
						_usLen�����ݳ���
* ���ڲ�����16λУ��ֵ
*************************************************************************************/
u16 CRC16_Modbus(u8 *_pBuf, int _usLen)
{
  char ucCRCHi = 0xFF; /* ��CRC�ֽڳ�ʼ�� */
	char ucCRCLo = 0xFF; /* ��CRC �ֽڳ�ʼ�� */
	int usIndex;  /* CRCѭ���е����� */

  while (_usLen--)
  {
		usIndex = ucCRCHi ^ *_pBuf++; /* ����CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
  }
    return ((u16)ucCRCHi << 8 | ucCRCLo);
}

/************************************************************************************
* �������ƣ�Buf2Uint16
* ������������2�ֽ�����(���Big Endian���򣬸��ֽ���ǰ)ת��Ϊ16λ����
* ��ڲ�����_pBuf : ����
* ���ڲ�����16λ����ֵ
*************************************************************************************/
u16 Buf2Uint16(unsigned char* _pbuf)
{
  return (((u16)_pbuf[0]<<8)|_pbuf[1]);
}

/************************************************************************************
* �������ƣ�Buf2Uint32
* ������������2�ֽ�����(���Big Endian���򣬸��ֽ���ǰ)ת��Ϊ16λ����
* ��ڲ�����_pBuf : ����
* ���ڲ�����16λ����ֵ
*************************************************************************************/
u32 Buf2Uint32(unsigned char* _pbuf)
{
  return (((u32)_pbuf[0]<<24)|((u32)_pbuf[1])<<16|((u16)_pbuf[2]<<8)|_pbuf[3]);
}

/************************************************************************************
* �������ƣ�MODH_SendAckWithCRC
* ��������������Ӧ���Զ���CRC
* ��ڲ�������
* ���ڲ�������
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
* �������ƣ�MODH_Send10H
* ��������������10ϵ��ָ��Զ���CRC
* ��ڲ�����_addr���Ի���ַ 
						_reg���Ĵ������
				 _num: �Ĵ�������
						_buf���Ĵ�����ֵ
* ���ڲ�������
*************************************************************************************/
void MODH_Send10H(u8 _addr, u16 _reg, u8 _num, u8 *_buf)
{
	u16 i;
	RS485_TX_CNT = 0;
	RS485_TX_BUF[RS485_TX_CNT++] = _addr;		//��վ��ַ
	RS485_TX_BUF[RS485_TX_CNT++] = 0x10;		//������	
	RS485_TX_BUF[RS485_TX_CNT++] = _reg >> 8;	//�Ĵ������ ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _num >> 8;	//�Ĵ���������� ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _num;		//�Ĵ������� ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = 2*_num;		//�����ֽڼ���
	
	for(i=0;i<2*_num;i++)
	{
		RS485_TX_BUF[RS485_TX_CNT++] = _buf[i];		//д����������
	}
	MODH_SendAckWithCRC();		//�������ݣ��Զ���CRC
}


/************************************************************************************
* �������ƣ�MODH_Send06H
* ��������������06ϵ��ָ��Զ���CRC
* ��ڲ�����_addr���Ի���ַ 
						_reg���Ĵ������
				 _regcnt: �Ĵ�������
						_value���Ĵ�������
* ���ڲ�������
*************************************************************************************/
void MODH_Send06H(u8 _addr, u16 _reg, u16 _regcnt, u16 _value)
{
	RS485_TX_CNT = 0;
	RS485_TX_BUF[RS485_TX_CNT++] = _addr;		//��վ��ַ
	RS485_TX_BUF[RS485_TX_CNT++] = 0x06;		//������	
	RS485_TX_BUF[RS485_TX_CNT++] = _reg >> 8;	//�Ĵ������ ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _regcnt >> 8;	//�Ĵ���������� ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _regcnt;		//�Ĵ������� ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = 0x02;		//�ֽڼ���
	RS485_TX_BUF[RS485_TX_CNT++] = _value >> 8;	//Ԥ������ ���ֽ� 
	RS485_TX_BUF[RS485_TX_CNT++] = _value;		//Ԥ�����ݵ��ֽ� 
	
	MODH_SendAckWithCRC();		//�������ݣ��Զ���CRC
	fAck06H = 0;							//����������յ�Ӧ�𣬸ñ�־Ϊ1
}

/************************************************************************************
* �������ƣ�MODH_Send03H
* ��������������03ϵ��ָ��Զ���CRC
* ��ڲ�����_addr���Ի���ַ 
						_reg���Ĵ������
						_num���Ĵ�������
* ���ڲ�������
*************************************************************************************/
void MODH_Send03H(u8 _addr, u16 _reg, u16 _num)
{
	RS485_TX_CNT = 0;
	RS485_TX_BUF[RS485_TX_CNT++] = _addr;		//��վ��ַ
	RS485_TX_BUF[RS485_TX_CNT++] = 0x03;		//������	
	RS485_TX_BUF[RS485_TX_CNT++] = _reg >> 8;	//�Ĵ������ ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _reg;		//�Ĵ������ ���ֽ�
	RS485_TX_BUF[RS485_TX_CNT++] = _num >> 8;	//�Ĵ������� ���ֽ� 
	RS485_TX_BUF[RS485_TX_CNT++] = _num;		//�Ĵ������� ���ֽ� 
	
	MODH_SendAckWithCRC();		//�������ݣ��Զ���CRC
	
	RegNum = _num;		//�Ĵ������� 
	Reg03H = _reg;		//����03Hָ���еļĴ�����ַ�������Ӧ�����ݽ��з���
}

/********************************************************************************************************
*	�� �� ��: MODH_Read_06H
*	����˵��: ����06Hָ���Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
void MODH_Read_06H(void)
{
	u8 rs485buf[13];
	RS485_Receive_Data(rs485buf);
	if (rs485buf[0] == SlaveAddr)		
	{
		fAck06H = 1;		/* ���յ�Ӧ�� */
	}
}

/********************************************************************************************************
*	�� �� ��: MODH_Read_10H
*	����˵��: ����10Hָ���Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************/
void MODH_Read_10H(void)
{
	if (RS485_RX_BUF[0] == SlaveAddr)		
	{
		fAck10H = 1;		/* ���յ�Ӧ�� */
	}
}

/************************************************************************************
* �������ƣ�MODH_Read_03H
* ����������MODBUS��03��Ȧ����
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void MODH_Read_03H(void)
{
  u8 *p;
	switch(Reg03H)
	{
		case ReadConfig:
			if(CRC16_Modbus(RS485_RX_BUF,19)==(RS485_RX_BUF[19]<<8|RS485_RX_BUF[20]))  //�ж�CRC
			{
				p=&RS485_RX_BUF[7]; 								//���ݿ�ʼ��ַ
				g_ELE_PAM.U0 = Buf2Uint16(p); p+=2;							//��ѹ����
				g_ELE_PAM.I0 = Buf2Uint16(p); p+=4; 								//��������
				g_ELE_PAM.UBB = Buf2Uint16(p); p+=2; 								//��ѹ���
				g_ELE_PAM.IBB = Buf2Uint16(p);									 //�������
			}
			else printf("485");
				//USART1_Send_Data(RS485_RX_BUF,21);
			break;
		case ReadParameters:
			if(CRC16_Modbus(RS485_RX_BUF,37)==(RS485_RX_BUF[37]<<8|RS485_RX_BUF[38]))  //�ж�CRC
			{
				p=&RS485_RX_BUF[3]; 								//���ݿ�ʼ��ַ
				g_ELE_PAM.UA = Buf2Uint16(p); p+=2;							//A���ѹ
				g_ELE_PAM.UB = Buf2Uint16(p); p+=2; 								//B���ѹ
				g_ELE_PAM.UC = Buf2Uint16(p); p+=2; 								//C���ѹ
				g_ELE_PAM.IA = Buf2Uint16(p);	p+=2;							 //A�����
				g_ELE_PAM.IB = Buf2Uint16(p);	p+=2;							 //B�����
				g_ELE_PAM.IC = Buf2Uint16(p);	p+=2;							 //C�����
				g_ELE_PAM.P = Buf2Uint16(p);	p+=2;							 //�й�����
				g_ELE_PAM.Q = Buf2Uint16(p);	p+=2;							 //�޹�����
				g_ELE_PAM.S = Buf2Uint16(p);	p+=2;							 //���ڹ���
				g_ELE_PAM.PF = Buf2Uint16(p);	p+=10;							 //��������
				g_ELE_PAM.Pa = Buf2Uint16(p);	p+=2;							 //A�๦��
				g_ELE_PAM.Pb = Buf2Uint16(p);	p+=2;							 //B�๦��
				g_ELE_PAM.Pc = Buf2Uint16(p);							 			//C�๦��
			}
			else printf("485");
				//USART1_Send_Data(RS485_RX_BUF,39);
			break;
		case ReadElectricEnergy:
			if(CRC16_Modbus(RS485_RX_BUF,11)==(RS485_RX_BUF[11]<<8|RS485_RX_BUF[12]))  //�ж�CRC
			{
				p=&RS485_RX_BUF[3]; 								//���ݿ�ʼ��ַ
				g_ELE_PAM.WH_P = Buf2Uint32(p); p+=4;							//�����й�����
				g_ELE_PAM.WH_M = Buf2Uint32(p); 						//�����й�����
			}
			else printf("485");
			//USART1_Send_Data(RS485_RX_BUF,13);
			break;
		default: break;				
	}
	memset(RS485_RX_BUF,0,64);
} 

