/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "hx711/hx711.h"


//#define ADIO GPIOB        //����ADоƬʹ�õ�I/O�˿��顣 
//#define DATA GPIO_PIN_4	  //����ADоƬ��������ʹ�õ����š�
//#define CLK  GPIO_PIN_5  //����ADоƬ��ʱ����ʹ�õ����š�
//#define ADCLK() __HAL_RCC_GPIOB_CLK_ENABLE()  //����ADоƬ��ʹ�õ�I/O�˿ڵ�ʱ�ӡ�

#define ADIO GPIOH        //����ADоƬʹ�õ�I/O�˿��顣 
#define DATA GPIO_PIN_10	  //����ADоƬ��������ʹ�õ����š�
#define CLK  GPIO_PIN_11  //����ADоƬ��ʱ����ʹ�õ����š�
#define ADCLK() __HAL_RCC_GPIOH_CLK_ENABLE()  //����ADоƬ��ʹ�õ�I/O�˿ڵ�ʱ�ӡ�

#define N 12	   //�����˲�ʱ����Ų���ֵ�Ķ��еĳ��ȡ�ʹ��ѭ�����С�
unsigned long BUF[N];  //������Ϊ���е�ȫ�����顣
unsigned long SUM=0;	//�����Ų���ֵ֮�͵�ȫ�ֱ�����
int i=0;	//����ָ����е�ǰλ�õ������±������

void hx711_delay(uint16_t time)
{
	uint8_t i;

  while(time)
  {    
    for (i = 0; i < 10; i++)
    {
      
    }
    time--;
  }
}

void ADInit(void) //��ʼ��ADоƬʹ�õ�I/O�˿ڡ�
{
	 /* ����IOӲ����ʼ���ṹ����� */
  GPIO_InitTypeDef GPIO_InitStruct;

	/*ʹ��hx711��Ӧ��IO��ʱ��*/
	ADCLK();

	GPIO_InitStruct.Pin = CLK;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //CLK���Ŷ���Ϊ�����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ADIO,&GPIO_InitStruct);

	GPIO_InitStruct.Pin = DATA;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;//DATA���Ŷ���Ϊ���롣
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ADIO,&GPIO_InitStruct);		
}

unsigned long ReadAD(void)  //��ADоƬ��������ݡ�
{
	unsigned long val = 0;
	unsigned char i = 0;
	
	HAL_GPIO_WritePin(ADIO,DATA,GPIO_PIN_SET);
	HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_RESET);
	while(HAL_GPIO_ReadPin(ADIO,DATA));
	hx711_delay(1);
	for(i=0;i<24;i++)
	{
		HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_SET);
		val=val<<1;
		hx711_delay(1); 
		HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(ADIO,DATA))
		val++;
		hx711_delay(1);
	}
	HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_SET);
	val = val^0x800000;
	hx711_delay(1);
	HAL_GPIO_WritePin(ADIO,CLK,GPIO_PIN_RESET);
	hx711_delay(1); 
	return val; 		
}

