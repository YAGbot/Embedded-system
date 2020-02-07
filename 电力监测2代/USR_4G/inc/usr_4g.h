#ifndef __USR_4G_H
#define __USR_4G_H 			   
#include "sys.h"

/*��������*/
typedef struct
{
	float Ua1;					//A���ѹ
	float Ub1;					//B���ѹ
	float Uc1;					//C���ѹ
	float Ia1;					//A�����
	float Ib1;					//B�����
	float Ic1;					//C�����
	float P1;					//�й�����
	float Q1;					//�޹�����
	float S1;					//���ڹ���
	float PF1;					//��������
	float Pa1;					//A�๦��
	float Pb1;					//B�๦��
	float Pc1;					//C�๦��
	float WH_P1;				//�����й��ܵ���
	float WH_M1;				//�����й��ܵ���
	float WH1;						//�й��ܵ���
}pack1;

typedef struct
{
	float Ua1;					//A���ѹ
	float Ub1;					//B���ѹ
	float Uc1;					//C���ѹ
	float Ia1;					//A�����
	float Ib1;					//B�����
	float Ic1;					//C�����
	float P1;					//�й�����
	float Q1;					//�޹�����
	float S1;					//���ڹ���
	float PF1;					//��������
	float Pa1;					//A�๦��
	float Pb1;					//B�๦��
	float Pc1;					//C�๦��
	float WH_P1;				//�����й��ܵ���
	float WH_M1;				//�����й��ܵ���
	float WH1;						//�й��ܵ���
}pack2;

extern pack1 g_pack1;
extern pack2 g_pack2;
/*��������*/
void Get_Data01(void);
void Get_Data02(void);
void GU900_Send_DataList(void);
void GU900_Send_DataList02(void);

void sim7020_sta(void);
void GSM_Init(void);
int check_buff(u8 *buff, char *str);
u8* gprs_check_cmd(u8 *str);
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

void strToHexStr(char *src, char *str);

int USR4G_Enter_CMDMode(void);
void USR4G_DTUConfig(void);
u8 SIM7020_Snd_GSN(void);

void itoi(char *str, unsigned long num);
char itoc(unsigned char ichar);

#endif

