#ifndef __GU900_H_
#define __GU900_H_
#include <stm32f10x.h>

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
//	float Q1;					//�޹�����
//	float S1;					//���ڹ���
//	float PF1;					//��������
//	float Pa1;					//A�๦��
//	float Pb1;					//B�๦��
//	float Pc1;					//C�๦��
//	float WH_P1;				//�����й��ܵ���
//	float WH_M1;				//�����й��ܵ���
//	float WH1;						//�й��ܵ���
}pack1;




extern pack1 g_pack1;

/*��������*/
void Gu900_Init(void);
void Get_Data01(void);
void GU900_Send_DataList(void);

#endif
