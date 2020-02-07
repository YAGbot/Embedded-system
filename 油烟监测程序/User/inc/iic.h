#ifndef __IIC__H
#define __IIC__H
#include "sys.h"

 
/*IIC�ܽŷ���������궨��*/
#define IIC_SDA_IPUT  	{GPIOB->CRL &= 0X0FFFFFFF; GPIOB->CRL |= (u32)0x8 << 4*7;}  //�����������
#define IIC_SDA_OPUT 		{GPIOB->CRL	&= 0X0FFFFFFF; GPIOB->CRL |= (u32)0x3 << 4*7;}  //�������50M	 
#define	IIC_SCL_SET			PB_SET(6)    	 		 							  //SCL���1
#define IIC_SCL_CLR    	PB_CLR(6) 	 										  //SCL���0
#define	IIC_SDA_SET			PB_SET(7)    	 		 							  //SDA���1
#define IIC_SDA_CLR    	PB_CLR(7) 	 										  //SDA���0
#define IIC_SDA_RED  		PB_RED(7)  										  //��SDA��ֵ

/*IIC����ʱ������궨��*/
#define TSTA_SU		5						//��ʼ�ź�SDA�½���֮ǰSCL�ߵ�ƽ����ʱ��(us)	  	
#define TSTA_HD		5						//��ʼ�ź�SDA�½���֮��SCL�ߵ�ƽ����ʱ��(us)
#define TSTO_SU		5						//��ֹ�ź�SDA������֮ǰSCL�ߵ�ƽ����ʱ��(us)
#define TSTO_HD		5						//��ֹ�ź�SDA������֮��SCL�ߵ�ƽ����ʱ��(us)
#define TDAT_SU		5						//����λSCL����֮ǰSDA����ʱ��(us)
#define TDAT_HD		5						//����λSCL����֮��SDA����ʱ��(us)
#define TLOW		5						//����λSCL�͵�ƽʱ��(us)
#define THIGH		5						//����λSCL�ߵ�ƽʱ��(us)
#define TCHECK		20						//���Ӧ��/��Ӧ���ź�����

/*��������*/
void IIC_init(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_TxByte(u8 data);
u8 IIC_RxByte(u8 ack);

#endif
