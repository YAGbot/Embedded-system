#ifndef __GU900_H_
#define __GU900_H_
#include <stm32f10x.h>

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
//	float Q1;					//无功功率
//	float S1;					//视在功率
//	float PF1;					//功率因数
//	float Pa1;					//A相功率
//	float Pb1;					//B相功率
//	float Pc1;					//C相功率
//	float WH_P1;				//正向有功总电能
//	float WH_M1;				//反向有功总电能
//	float WH1;						//有功总电能
}pack1;




extern pack1 g_pack1;

/*函数声明*/
void Gu900_Init(void);
void Get_Data01(void);
void GU900_Send_DataList(void);

#endif
