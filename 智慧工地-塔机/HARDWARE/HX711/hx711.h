#ifndef __HX711_H__
#define	__HX711_H__

#include "sys.h"

void ADInit(void); //��ʼ��ADоƬ������ʹ�õ�I/O�˿ڡ�
unsigned long ReadAD(void);  //��ADоƬ��������ݡ�

/*���������ͨ�ţ��ڴ��ڵ�����������ʾADģ�鷵�ص����ݣ��Ե���ADģ��ĺ�����*/

void UsartDebugAD(void);
void UsartDebugAD_F(void);
void BUFInit(void);   //��ʼ����������BUF��SUM�������������ȴ���N������ֵ��SUMΪN������ֵ�ĺ͡�
long double filter(void);



/***********************************
�������õ���һЩ���ڲ���������������
************************************/

void UsartConf(unsigned long baud); //�������ã�ʹ�ܺ������β�baud�ǲ����ʣ����磬9600��
void USendStr(u8* str);  //����USendStr()ͨ�����ڷ���һ���ַ�����

#endif
