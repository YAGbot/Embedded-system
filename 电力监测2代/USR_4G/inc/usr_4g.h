#ifndef __USR_4G_H
#define __USR_4G_H 			   
#include "sys.h"

/*变量声明*/
typedef struct
{
	float Ua1;					//A相电压
	float Ub1;					//B相电压
	float Uc1;					//C相电压
	float Ia1;					//A相电流
	float Ib1;					//B相电流
	float Ic1;					//C相电流
	float P1;					//有功功率
	float Q1;					//无功功率
	float S1;					//视在功率
	float PF1;					//功率因数
	float Pa1;					//A相功率
	float Pb1;					//B相功率
	float Pc1;					//C相功率
	float WH_P1;				//正向有功总电能
	float WH_M1;				//反向有功总电能
	float WH1;						//有功总电能
}pack1;

typedef struct
{
	float Ua1;					//A相电压
	float Ub1;					//B相电压
	float Uc1;					//C相电压
	float Ia1;					//A相电流
	float Ib1;					//B相电流
	float Ic1;					//C相电流
	float P1;					//有功功率
	float Q1;					//无功功率
	float S1;					//视在功率
	float PF1;					//功率因数
	float Pa1;					//A相功率
	float Pb1;					//B相功率
	float Pc1;					//C相功率
	float WH_P1;				//正向有功总电能
	float WH_M1;				//反向有功总电能
	float WH1;						//有功总电能
}pack2;

extern pack1 g_pack1;
extern pack2 g_pack2;
/*函数声明*/
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

