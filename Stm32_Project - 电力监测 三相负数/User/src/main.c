/***********************************************************************************
* @�ļ�: main.c
* @����: ̩����ҵ����򣩻����˼����о�Ժ--�����
* @ʱ��: 2017.12.18
* @����: ���������ڲɼ������
************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rs485.h"
#include "volmeter.h"
#include "gu900_uart.h"
#include "gu900.h"
#include "timer.h"
#include "modbus_timer.h"	

/**************************************������***************************************/
int main(void)
{ 
	NVIC_GroupType_Config(GroupType_M2L2);
	delay_init(72);
	TIM6_Init(7199,9999);
	TIME7_Init(99,7199);		//��ʱ10ms
	USART1_Init(72, 9600);
	RS485_Init(36,9600);
	delay_ms(5000);
	Read_Config();		//������
	printf("\"Us\":\"%d\",\"Is\":\"%d\",\"UBB\":\"%d\",\"IBB\":\"%d\"",g_ELE_PAM.U0,g_ELE_PAM.I0/10,g_ELE_PAM.UBB,g_ELE_PAM.IBB);
	Gu900_Init();
	delay_ms(2000);
	capture_time_count=CAPTURETIME;
	time6_start();			//��������ڶ�ʱ���ж�
	while(1)
	{
			;
	}
}

