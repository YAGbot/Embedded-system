/*******************************************************

���ļ��У��ǲ������ش�����ר��ADоƬ HX711 �ĳ���

********************************************************/
#include "hx711.h"
#include "delay.h"


#define ADIO GPIOA        //����ADоƬʹ�õ�I/O�˿��顣 
#define DATA GPIO_Pin_12	  //����ADоƬ��������ʹ�õ����š�
#define CLK  GPIO_Pin_11  //����ADоƬ��ʱ����ʹ�õ����š�
#define ADCLK RCC_APB2Periph_GPIOA  //����ADоƬ��ʹ�õ�I/O�˿ڵ�ʱ�ӡ�

#define N 12	   //�����˲�ʱ����Ų���ֵ�Ķ��еĳ��ȡ�ʹ��ѭ�����С�
unsigned long BUF[N];  //������Ϊ���е�ȫ�����顣
unsigned long SUM=0;	//�����Ų���ֵ֮�͵�ȫ�ֱ�����
int i=0;	//����ָ����е�ǰλ�õ������±������

void ADInit(void) //��ʼ��ADоƬʹ�õ�I/O�˿ڡ�
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(ADCLK,ENABLE);

	GPIO_InitStructure.GPIO_Pin = CLK; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //CLK���Ŷ���Ϊ�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADIO,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DATA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//DATA���Ŷ���Ϊ���롣
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADIO,&GPIO_InitStructure);		
}

unsigned long ReadAD(void)  //��ADоƬ��������ݡ�
{
	unsigned long val = 0;
	unsigned char i = 0;
	
	GPIO_SetBits(ADIO,DATA);
	GPIO_ResetBits(ADIO,CLK);
	while(GPIO_ReadInputDataBit(ADIO,DATA));
	delay_us(1);
	for(i=0;i<24;i++)
	{
		GPIO_SetBits(ADIO,CLK);
		val=val<<1;
		delay_us(1); 
		GPIO_ResetBits(ADIO,CLK);
		if(GPIO_ReadInputDataBit(ADIO,DATA))
		val++;
		delay_us(1);
	}
	GPIO_SetBits(ADIO,CLK);
	val = val^0x800000;
	delay_us(1);
	GPIO_ResetBits(ADIO,CLK);
	delay_us(1); 
	return val; 		
}

