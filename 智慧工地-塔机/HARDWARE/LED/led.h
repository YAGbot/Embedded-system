#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PAout(8)	// PA8
#define LED1 PDout(2)	// PD2	

#define FIFO1 PBout(5)	// PB5		//�̵�������
#define FIFO2 PBout(6)	// PB6
#define FIFO3 PBout(7)	// PB7
#define FIFO4 PBout(8)	// PB8

void LED_Init(void);//��ʼ��
void control_init(void);
		 				    
#endif
