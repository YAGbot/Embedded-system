#ifndef __VOLMETER__H
#define __VOLMETER__H
#include "stm32f10x.h"

/*��������*/
typedef struct
{
	u16 U0;					//��ѹ����
	u16 I0;					//��������
	u16 UBB;					//��ѹ���
	u16 IBB;					//�������
	u16 UA;					//A���ѹ
	u16 UB;					//B���ѹ
	u16 UC;					//C���ѹ
	u16 IA;					//A�����
	u16 IB;					//B�����
	u16 IC;					//C�����
	int16_t P;					//�й�����
	int16_t Q;					//�޹�����
	u16 S;					//���ڹ���
	int16_t PF;					//��������
	int16_t Pa;					//A�๦��
	int16_t Pb;					//B�๦��
	int16_t Pc;					//C�๦��
	u32 WH_P;				//�����й��ܵ���
	u32 WH_M;				//�����й��ܵ���
}ELE_PAM;

typedef struct
{
	u16 U0;					//��ѹ����
	u16 I0;					//��������
	u16 UBB;					//��ѹ���
	u16 IBB;					//�������
	u16 UA;					//A���ѹ
	u16 UB;					//B���ѹ
	u16 UC;					//C���ѹ
	u16 IA;					//A�����
	u16 IB;					//B�����
	u16 IC;					//C�����
	int16_t P;					//�й�����
	int16_t Q;					//�޹�����
	u16 S;					//���ڹ���
	int16_t PF;					//��������
	int16_t Pa;					//A�๦��
	int16_t Pb;					//B�๦��
	int16_t Pc;					//C�๦��
	u32 WH_P;				//�����й��ܵ���
	u32 WH_M;				//�����й��ܵ���
}ELE_PAM2;

extern u8 SlaveAddr;									//�ӻ���ַ
extern u8 SlaveAddr2;									//�ӻ���ַ

#define WriteEleClear				0x000C		//��������������ʼ��ַ			8���Ĵ���
#define WriteConfig  				0x0005		//�����üĴ�����ʼ��ַ   2���Ĵ���
#define ReadConfig  				0x0000		//�����üĴ�����ʼ��ַ   8���Ĵ���
#define ReadParameters  		0x0040		//������������ʼ��ַ		17���Ĵ���
#define ReadElectricEnergy	0X000C    //��������ܶ�����ʼ��ַ	4���Ĵ���

extern u8 RegNum;										//�Ĵ�������
extern u8 Reg03H;										//����Ĵ�����ַ���������ݷ���

extern ELE_PAM g_ELE_PAM;
extern ELE_PAM2 g_ELE_PAM2;

/*��������*/
int DevAddRoll(void);
void Write_Data_Clear(void);
void Write_Config(void);
void Read_Config(u8 _addr);
void Read_Parameters(u8 _addr);
void Read_ElectricEnergy(u8 _addr);

u16 CRC16_Modbus(u8 *_pBuf, int _usLen);
u16 Buf2Uint16(unsigned char* _pbuf);
u32 Buf2Uint32(unsigned char* _pbuf);
static void MODH_SendAckWithCRC(void);
void MODH_Send10H(u8 _addr, u16 _reg, u8 _num, u8 *_buf);
void MODH_Send06H(u8 _addr, u16 _reg, u16 _regcnt, u16 _value);
void MODH_Send03H(u8 _addr, u16 _reg, u16 _num);
void MODH_Read_03H(void);
void MODH_Read_03H02(void);
void MODH_Read_06H(void);
void MODH_Read_10H(void);

#endif
