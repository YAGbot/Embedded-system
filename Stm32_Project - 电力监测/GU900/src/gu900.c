#include "gu900.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "gu900_uart.h"
#include "delay.h"
#include "volmeter.h"


pack1 g_pack1;
/************************************************************************************
* �������ƣ�Gu900_Init
* ����������gu900��ʼ������
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void Gu900_Init(void)
{
	USART3_Init(36,115200);
}

/************************************************************************************
* �������ƣ�Get_Data01
* ������������ȡ�������ݺ���
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void Get_Data01(void)
{
	g_pack1.Ua1=(float)g_ELE_PAM.UA*g_ELE_PAM.U0*g_ELE_PAM.UBB/10000;			//A���ѹ
	g_pack1.Ub1=(float)g_ELE_PAM.UB*g_ELE_PAM.U0*g_ELE_PAM.UBB/10000;			//B���ѹ
	g_pack1.Uc1=(float)g_ELE_PAM.UC*g_ELE_PAM.U0*g_ELE_PAM.UBB/10000;			//C���ѹ
	
	g_pack1.Ia1=(float)g_ELE_PAM.IA*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;	//A�����
	g_pack1.Ib1=(float)g_ELE_PAM.IB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;	//B�����
	g_pack1.Ic1=(float)g_ELE_PAM.IC*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;	//C�����
	
	g_pack1.P1=(float)g_ELE_PAM.P*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB*3/10000;	//�ܹ���
	g_pack1.Pa1=(float)g_ELE_PAM.Pa*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;		//A�๦��
	g_pack1.Pb1=(float)g_ELE_PAM.Pb*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;		//B�๦��
	g_pack1.Pc1=(float)g_ELE_PAM.Pc*g_ELE_PAM.U0*g_ELE_PAM.UBB*g_ELE_PAM.I0/10*g_ELE_PAM.IBB/10000;		//C�๦��
	g_pack1.WH_P1=(float)g_ELE_PAM.WH_P*g_ELE_PAM.U0*g_ELE_PAM.I0/10*g_ELE_PAM.UBB*g_ELE_PAM.IBB/10800000;	//�����й�����
	g_pack1.WH_M1=(float)g_ELE_PAM.WH_M*g_ELE_PAM.U0*g_ELE_PAM.I0/10*g_ELE_PAM.UBB*g_ELE_PAM.IBB/10800000;	//�����й�����
	g_pack1.WH1=g_pack1.WH_P1-g_pack1.WH_M1;
}


//void GU900_Send_DataList(void)
//{
//	gu900_printf("{\"DataList\":[{\"Ua\":\"%fV\",\"Ub\":\"%fV\",\"Uc\":\"%fV\",\"Ia\":\"%fA\",\"Ib\":\"%fA\",\"Ic\":\"%fA\",\"P\":\"%fW\","
//	"\"Pa\":\"%fW\",\"Pb\":\"%fW\",\"Pc\":\"%fW\",\"WH\":\"%fKWh\"}],\"DevID\":\"HYHB%4EP-AC\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}\n",\
//	g_pack1.Ua1,g_pack1.Ub1,g_pack1.Uc1,g_pack1.Ia1,g_pack1.Ib1,g_pack1.Ic1,g_pack1.P1,g_pack1.Pa1,g_pack1.Pb1,g_pack1.Pc1,g_pack1.WH1,\
//	1,g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
//}

void GU900_Send_DataList(void)
{
	gu900_printf("{\"DataList\":[{\"Ua\":\"%f\",\"Ub\":\"%f\",\"Uc\":\"%f\",\"Ia\":\"%f\",\"Ib\":\"%f\",\"Ic\":\"%f\",\"P\":\"%f\","
	"\"Pa\":\"%f\",\"Pb\":\"%f\",\"Pc\":\"%f\",\"WH\":\"%f\"}],\"DevID\":\"HYHB%4EP-AC\",\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"}\n",\
	g_pack1.Ua1,g_pack1.Ub1,g_pack1.Uc1,g_pack1.Ia1,g_pack1.Ib1,g_pack1.Ic1,g_pack1.P1,g_pack1.Pa1,g_pack1.Pb1,g_pack1.Pc1,g_pack1.WH1,\
	1,g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
}
