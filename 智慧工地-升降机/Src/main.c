/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stdio.h"
#include <math.h>
#include "stm32f4xx_hal.h"
#include "string.h" 	 
#include "led/bsp_led.h"
#include "BasicTIM/bsp_BasicTIM.h" 
#include "spiflash/bsp_spiweight.h"
#include "spiflash/bsp_spiflash.h"
#include "usart/bsp_usartx.h"
#include "usart/bsp_485_usart.h"
#include "HMI/bsp_usartx_HMI.h"
#include "usart/bsp_gsm_usart.h"
#include "adc/bsp_adc.h"
#include "gpio/bsp_gpio.h"
#include "hx711/hx711.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ��ȡ�������ĳ��� */
#define  FLASH_WriteAddress     0x0000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
#define  N 											40


/* ˽�б��� ------------------------------------------------------------------*/
uint8_t qingjiao[7]={0x5A,0xA5,0x20,0xf0,0x01,0x03,0x00};	//��ȡ��Ǵ�����ָ��
uint8_t shijian[7]={0x5A,0xA5,0x04,0x83,0x00,0x10,0x04};	//��ȡ��ǰʱ��
uint8_t wendu[8]={0x05,0x03,0x00,0x00,0x00,0x02,0xC5,0x8F};
uint8_t  ID_buff[15]={0};
uint32_t DeviceID = 0;
uint32_t FlashID = 0;

uint8_t name[10];
uint8_t NameID[20];
char EndStr[300];

uint8_t USART1_RX_flg=0;
uint8_t register_time_flag=0;
uint8_t check_ID_flag=1;
uint8_t ALARM_FLAG=0;
uint8_t ALARM_FLAG_WEIGHT=0;	//����������־����ֹ��α���������0:�������������ɴ������� 1���Ѵ���������ȴ�����״̬�����ɴ�������
uint8_t ALARM_FLAG_HIGH=0;
uint8_t ALARM_FLAG_ANGLEX=0;
uint8_t ALARM_FLAG_ANGLEY=0;
uint8_t angle_warning=0;
uint8_t weight_warning=0;
uint8_t high_warning=0;
uint8_t IO1,IO2;	//�ż��״̬
int32_t Result_data=0;
uint8_t write_flag=0;		//д���־����һ���仯��Ҫ����д��flash
uint16_t count=0;

uint8_t Tx_Buffer[29] = {0};		//spiͨ���ڴ棬���ֽڶ�д
uint8_t Rx_Buffer[29] = {0};

__IO uint8_t  weight_Zero_IsInit=1; //0: У׼ֵδ��ȡ  1���ѻ�ȡУ׼ֵ

float weight_proportion=0; //��������ϵ��
float high_proportion=0; //�߶ȱ���ϵ��
__IO int32_t weight_Zero_Data=8388608;  //��ֵ	-- ��ѹΪ0ʱ��ʵ�ʲ���ֵ 
__IO int16_t weight_current;    //��ǰ��������ֵ
__IO int32_t weight_initial_value;	//��ʼֵ,�൱�ڹ�ʽ�е�b
__IO uint16_t high_current;    //��ǰ�߶Ȳ���ֵ
__IO int32_t high_initial_value;	//��ʼֵ,�൱�ڹ�ʽ�е�b


__IO uint16_t previous_weight_count;	//��¼�ϴ�����

__IO int16_t weight_change=0;	//��¼���ι���ʱ��������
__IO uint16_t weight_close=0;	//��¼ÿ�ι�������ʱ����ֵ
__IO uint16_t weight_count=0;	//��ǰ����ֵ
__IO uint16_t weight_limit=0;	//������ֵ
__IO uint16_t weight_angle=0;	//�����ٷֱ� ��ǰ����/������ֵ  weight_count/weight_limit

float previous_high_count;	//��¼�ϴθ߶�
float high_count=0;	//��ǰ�߶�ֵ
__IO uint16_t high_limit=0;	//�߶���ֵ
float high_speed=0;	//�ٶ�  ==����ǰ�߶�-�ϴθ߶ȣ�  /2.3  m/s
float net_high_temp;	//Զ�̴���߶���ʱֵ
/*********����ֵ�궨����**********/
__IO uint16_t weight_zero;	//���ز���ֵ
__IO uint16_t weight_add;		//���غ����ֵ
__IO uint16_t weight_zero_value;	//��������ֵ
__IO uint16_t weight_add_value;		//���غ�����ֵ

/*********�߶ȴ������궨����**********/ 
__IO uint16_t high_zero;	//�����õײ���ֵ
__IO uint16_t high_top;		//�����ö�����ֵ
__IO uint16_t high_zero_value;	//�����õ�ʵ�������߶�
__IO uint16_t high_top_value;		//�����ö�ʵ�������߶�

float angle_limit=0;	//�����ֵ
__IO uint16_t worktime_limit;	//Ĭ�Ϲ���ʱ������
__IO uint16_t alreadyworktime;	//����ʱ��
/*************ǰ���ű궨����*******************/
__IO uint16_t back_door;		//1:��  0����
__IO uint16_t front_door;
__IO uint16_t work_load_cnt;		//װ�ش���
__IO uint16_t illegal_operation_cnt;	//Υ�����
//__IO uint8_t workcycle_flag;//����ѭ���ж� 1����Դ 0������
__IO uint8_t work_register_sta=0;	//��Աע��״̬ 1;������  0���������� �ɿ��ڻ��ж�

__IO uint16_t timer_count=0;	//����ʱ��ѯ����״̬����
__IO uint16_t Work_Count=0;		//�����ɼ�ʱ�����
__IO uint16_t Count=0;

uint8_t io1_temp,io2_temp;

// ���ڱ���ת�������ĵ�ѹֵ	 
float ADC_ConvertedValueLocal;
// ADת�����ֵ
uint16_t ADC_ConvertedValue[40];
uint32_t DMA_Transfer_Complete_Count=0;

__IO uint32_t ad7190_data[4];	//ad7190�ɼ�ֵ

/***********�Ƕȱ���***************/
float x_angle,y_angle;

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
void SystemClock_Config(void);
void calibrate_parameter(void);
int32_t int_abs(int32_t value1,int32_t value2);
uint16_t filter(void);
void io_check(void);
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{
  /* ��λ�������裬��ʼ��Flash�ӿں�ϵͳ�δ�ʱ�� */
  HAL_Init();
  /* ����ϵͳʱ�� */
  SystemClock_Config();
  
  /* ����LED��ʼ�� */
  LED_GPIO_Init();
	/* �������ʼ�� */
  INPUT_GPIO_Init();
	  /* ����˿ڳ�ʼ�� */
  OUTPUT_GPIO_Init();
	HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_RESET);	//Ĭ��ֵ�����ͨ��OUTx�����Ϊ��,�е�ѹ
	HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);	//220��ѹ��·�رգ�״̬�������෴����ͨ�޵�ѹ
	HAL_GPIO_WritePin(OUTPUT3_GPIO, OUTPUT3_GPIO_PIN, GPIO_PIN_SET);  
//	HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);  //����ߣ�����ر��޵�ѹ 
	//HAL_GPIO_WritePin(OUTPUT2_GPIO, OUTPUT2_GPIO_PIN, GPIO_PIN_RESET);  //���ͨ��OUTx�����Ϊ��,�е�ѹ
	
  /* ��ʼ��������Դ */
	USARTx_Init();				//���Դ���
	HMI_USARTx_Init();		//������
	GSM_USARTx_Init();		//gsmģ��
	RS485_USART_Init();		//485
  MX_USARTx_Init();			//����1�ӿ��ڻ�
//	/* ��ʼ��DMA */
//	MX_DMA_Init();

	/*HX711��ʼ��*/
	ADInit();
	
//  /* ADC ��ʼ�� */
//  MX_ADCx_Init();
///* ����ADת����ʹ��DMA������ж� */
//  HAL_ADC_Start_DMA(&hadcx,(uint32_t *)ADC_ConvertedValue,40);  
	
	 /* ��ʼ��SPI */
  MX_SPIFlash_Init();
	 /* Get SPI Flash Device ID */
	DeviceID = SPI_FLASH_ReadDeviceID();
  
  HAL_Delay(2000);
	
	FlashID = SPI_FLASH_ReadID(); 
  printf("FlashID is 0x%X,  Manufacturer Device ID is 0x%X\n", FlashID, DeviceID);
	if (FlashID == SPI_FLASH_ID)  /* #define  sFLASH_ID  0XEF4018 */
	{	
		uint8_t i;
		printf("��⵽�����flash W25Q128 !\n");
    SPI_FLASH_BufferRead(Rx_Buffer,FLASH_ReadAddress,sizeof(Rx_Buffer));
		if(Rx_Buffer[28]==0x5a)		//�����ݸĶ��ˣ�ֱ�Ӷ�ȡ��ʾ
		{
			/**********��rx���ݸ��µ�tx��ֹ�����buf���ݱ����д0��flash*******************/
			for(i=0;i<29;i++)
			{
				Tx_Buffer[i]=Rx_Buffer[i];
			}
			weight_zero_value=Rx_Buffer[0]*256+Rx_Buffer[1];
			diwen_send_data16(0x12b0,(uint16_t)weight_zero_value);	
			printf("weight_zero_value=%d\r\n",weight_zero_value);
			
			weight_zero=Rx_Buffer[2]*256+Rx_Buffer[3];
			diwen_send_data16(0x12c0,(uint16_t)weight_zero);	

			weight_add_value=Rx_Buffer[4]*256+Rx_Buffer[5];
			diwen_send_data16(0x12f0,(uint16_t)weight_add_value);	

			weight_add=Rx_Buffer[6]*256+Rx_Buffer[7];
			diwen_send_data16(0x1300,(uint16_t)weight_add);	

			weight_limit=Rx_Buffer[8]*256+Rx_Buffer[9];
			diwen_send_data16(0x1340,weight_limit);	

			back_door=Rx_Buffer[10]*256+Rx_Buffer[11];
			diwen_send_data16(0x1320,back_door);

			front_door=Rx_Buffer[12]*256+Rx_Buffer[13];
			diwen_send_data16(0x1310,front_door);	
		
			high_zero_value=(Rx_Buffer[14]*256+Rx_Buffer[15])/10;
			diwen_send_data16(0x1290,high_zero_value*10);	
			
			high_zero=Rx_Buffer[16]*256+Rx_Buffer[17];
			diwen_send_data16(0x12a0,high_zero);	
			
			high_top_value=(Rx_Buffer[18]*256+Rx_Buffer[19])/10;
			diwen_send_data16(0x12d0,high_top_value*10);	
			
			high_top=Rx_Buffer[20]*256+Rx_Buffer[21];
			diwen_send_data16(0x12e0,high_top);	
			
			high_limit=(Rx_Buffer[22]*256+Rx_Buffer[23])/10;
			diwen_send_data16(0x1330,high_limit*10);	
			
			angle_limit=(Rx_Buffer[24]*256+Rx_Buffer[25])/100;
			diwen_send_data16(0x1350,angle_limit*100);	
			
			worktime_limit=Rx_Buffer[26]*256+Rx_Buffer[27];
			diwen_send_data16(0x1360,(uint16_t)worktime_limit);	
			printf("worktime_limit=%d\r\n",worktime_limit);
    }
  }  	  
	
	
	calibrate_parameter();	//��������
	
	
   if(AD7190_Init()==0)
	{
    printf("��ȡ���� AD7190 !\n");
    while(1)
    {
      HAL_Delay(1000);
      if(AD7190_Init())
        break;
    }
  }
  printf("��⵽  AD7190 !\n");
  ad7190_conf(); 
  /* ������ʱ����ʼ����10ms�ж�һ�� */
	GENERAL_TIM5_Init();	//485
  BASIC_TIM6_Init();		//��������ʱ��
	BASIC_TIM7_Init();	//������
	
  /* ���ж�ģʽ��������ʱ�� */
	HAL_TIM_Base_Start_IT(&htim5);	

	//ʹ�ܽ��գ������жϻص�����
	HAL_UART_Receive_IT(&husartx_HMI,&hmiRxBuffer,1);
	HAL_UART_Receive_IT(&husartx_GSM,&gsmRxBuffer,1);
  HAL_UART_Receive_IT(&usartx_1,&aRxBuffer,1);
//	//��ǰ���ų�ʼ״̬
	IO1=HAL_GPIO_ReadPin(INPUT1_GPIO,INPUT1_GPIO_PIN);
	IO2=HAL_GPIO_ReadPin(INPUT2_GPIO,INPUT2_GPIO_PIN);
	printf("IO1:%d,IO2:%d\r\n",IO1,IO2);
	if(front_door==1)
	{
		IO1=!IO1;
	}
	if(back_door==1)
	{
		IO2=!IO2;
	}
	io1_temp=IO1;
	io2_temp=IO2;
	io_check();
	
  LED2_ON;
	//HAL_Delay(10000);
	if(GSM_cmd("usr.cnAT+IMEI\r\n","+IMEI:",100)==GSM_TRUE)
	{
		uint8_t i;
		uint8_t tmpbuff[7];
		for(i=0;i<6;i++)
		{
			tmpbuff[i]=gsm_uart_buff[18+i];//�޻��ԣ��У�24
		}
		tmpbuff[6]='\0';
		printf("%s\r\n",gsm_uart_buff);
		sprintf((char *)ID_buff,"THHY%s",tmpbuff);
		printf("%s",ID_buff);
		check_ID_flag=0;
	}
	diwen_send_numascii(0x1130,ID_buff,10);	//����ID������ʾ��
	
	diwen_send_numascii(0x1070,name,8);		
	diwen_send_numascii(0x1080,NameID,8);
	
	diwen_send_data16(0x10f0,0x00);
	diwen_send_data16(0x1150,0x00);
	//���װ�ؼ���
	diwen_send_data16(0x1160,work_load_cnt);
	diwen_send_data16(0x1170,illegal_operation_cnt);
	
	HAL_Delay(100);
	
  /* ����ѭ�� */
  while (1)
  {
		
		/***************�жϹ�������ʱ��************************/
		if((work_register_sta==1)&&(Count>=6000))
		{ 
			Count=0;
			worktime_limit--;
			alreadyworktime++;
			
			diwen_send_data16(0x10f0,worktime_limit);
			diwen_send_data16(0x1150,alreadyworktime);
			if(worktime_limit==0)		//���ι���ʱ����� 
			{
				//�رյ�Դ
				HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_RESET);  //�򿪿��ڻ�
				HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);  //�رտ���ϵͳ
				worktime_limit=Rx_Buffer[26]*256+Rx_Buffer[27];	//��λ����ʱ��
				diwen_send_data16(0x10f0,0x00);
				diwen_send_data16(0x1150,0x00);
				//���װ�ؼ���
				diwen_send_data16(0x1160,work_load_cnt);
				diwen_send_data16(0x1170,illegal_operation_cnt);
				alreadyworktime=0;
				work_register_sta=0;
				work_load_cnt=0;
				illegal_operation_cnt=0;
			}
		}
		/*******************��ȡ��ǰע��ʱ��*********************************/
		if(register_time_flag==1)
		{
			register_time_flag=0;
		//��ȡ��ǰע��ʱ��
			HMI_USARTx_RX_STA=0;
			memset(HMI_USARTx_RX_BUF,0,200);
			HMI_USARTx_SendDat(shijian,7);
			HAL_Delay(50);
			uint8_t year,month,day,hour,minute,sec;
			if(HMI_USARTx_RX_STA&0X8000)	//���������յ�һ��������
			{
				USART_Tx(HMI_USARTx_RX_BUF,20);
				HMI_USARTx_RX_STA=0;			//��־��ʼ��	 
				if(HMI_USARTx_RX_BUF[3+6]==0x83)	//���յ������޸���
				{
					year=HMI_USARTx_RX_BUF[7+6];
					month=HMI_USARTx_RX_BUF[8+6];
					day=HMI_USARTx_RX_BUF[9+6];
					hour=HMI_USARTx_RX_BUF[11+6];
					minute=HMI_USARTx_RX_BUF[12+6];
					sec=HMI_USARTx_RX_BUF[13+6];
					printf("%d.%d.%d.%d.%d.%d\r\n",2000+year,month,day,hour,minute,sec);
					diwen_send_data16(0x1090,2000+year);
					diwen_send_data16(0x10a0,month);
					diwen_send_data16(0x10b0,day);
					diwen_send_data16(0x10c0,hour);
					diwen_send_data16(0x10d0,minute);
					diwen_send_data16(0x10e0,sec);
				}
		}
	}
		/***********************��ѯ��������״̬&&���ݷ���************************************/
    if(timer_count>=3000)		//30s��ʱ ��ѯ����&&������������
    {
			if(check_ID_flag==1)
			{
				if(GSM_cmd("usr.cnAT+IMEI\r\n","+IMEI:",100)==GSM_TRUE)
				{
					uint8_t i;
					uint8_t tmpbuff[7];
					for(i=0;i<6;i++)
					{
						tmpbuff[i]=gsm_uart_buff[18+i];//�޻��ԣ��У�24
					}
					tmpbuff[6]='\0';
					printf("%s\r\n",gsm_uart_buff);
					sprintf((char *)ID_buff,"THHY%s",tmpbuff);
					printf("%s",ID_buff);
					check_ID_flag=0;
				}
	
				diwen_send_numascii(0x1130,ID_buff,10);	//����ID������ʾ��
			}
			HAL_Delay(100);
			/*4Gģ���ѯ����״̬*/
			if(GSM_cmd("usr.cnAT+SOCKALK\r\n","+SOCKALK:ON",100)==GSM_TRUE)
			{
				printf("%s",gsm_uart_buff);
				printf("�����ɹ�\r\n");
				clean_rebuff();                 //����˽��ջ���������,�Եȴ�������Զ�̽���
				diwen_send_asci3(0x1140,0xd2d1,0xc1ac,0xbdd3);	//����������״̬
			}
			else 
			{
				printf("%s",gsm_uart_buff);
				diwen_send_asci3(0x1140,0xceb4,0xc1ac,0xbdd3);	//����δ����״̬
			}
			sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"jump\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
			"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,\
			high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
			printf("%d\r\n",strlen(EndStr));
			printf("%s",EndStr);
			GSM_USART_printf("%s",EndStr);
			
      timer_count=0;
      LED1_TOGGLE;
      LED2_TOGGLE;
      LED3_TOGGLE;
    }
		/*********************���ڹ����ɼ�*********************************************/
		if(Work_Count>=100)		//1s��ʱ
		{
			HAL_TIM_Base_Stop_IT(&htim5);
			
			//	//��ǰ���ų�ʼ״̬
			IO1=HAL_GPIO_ReadPin(INPUT1_GPIO,INPUT1_GPIO_PIN);
			IO2=HAL_GPIO_ReadPin(INPUT2_GPIO,INPUT2_GPIO_PIN);
			//printf("IO1:%d,IO2:%d\r\n",IO1,IO2);
			if(front_door==1)
			{
				IO1=!IO1;
			}
			if(back_door==1)
			{
				IO2=!IO2;
			}
			if((IO1!=io1_temp)||(IO2!=io2_temp))	//����״̬������
			{
				io_check();
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
	"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
	//printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
			}
			
			io1_temp=IO1;
			io2_temp=IO2;		//��¼������״̬
			
			
			unsigned long ad;
				/**************�����������ɼ�***************/ 
			ad=ReadAD()/838.8608;
			//printf("ad=%d\r\n",ad);  
			weight_current=ad-10000;	//��ǰ��������ֵ
			if(weight_current<0)
			{
				weight_current=0; 
			}
			diwen_send_data16(0x1280,weight_current);
			weight_count=(weight_current-weight_zero)*weight_proportion;	//��ǰ����ֵ
			if((ALARM_FLAG_WEIGHT==0)&&(weight_count>=weight_limit))		//������ֵ
			{
				weight_warning=1;
				ALARM_FLAG_WEIGHT=1;	//������Ϊ1 waring =0
			}

			if((ALARM_FLAG_WEIGHT==1)&&(weight_count<weight_limit))
			{
				ALARM_FLAG_WEIGHT=0;
				ALARM_FLAG=1;
			}

			previous_weight_count=weight_count;		//��¼Ϊ�ϴ�����ֵ
			diwen_send_data16(0x1010,weight_count);
			weight_angle=weight_count*180/weight_limit;
			diwen_send_data16(0x1000,weight_angle);
			//printf("->%d,����:%dKg,������%d\r\n",weight_current,weight_count,weight_angle);
			
			
			/**************�߶ȴ������ɼ�***************/      
			if(AD7190_RDY_STATE==0)
			{
				uint8_t sample[4]={0};
				HAL_SPI_Receive(&hspi_weight,sample,4,0xFFFFFF); 
				if((sample[3]&0x80)==0)
				{
					uint8_t temp=(sample[3]&0x07);                                                      
					ad7190_data[temp]=(sample[0]<<16) | (sample[1]<<8) | (sample[2]);
					high_current=((float)ad7190_data[temp]/0x800000-1)*2997;//���ñ�ʵ��ο���ѹ2997mV
				}
			}
			//printf("high_current=%d\r\n",high_current);
			diwen_send_data16(0x1270,high_current);
			high_count=(high_current-high_zero)*high_proportion;	//��ǰ�߶�ֵ
			if(ALARM_FLAG_HIGH==0)
			{
				diwen_send_data16(0x1030,(uint16_t)((high_count+0.05f)*10));
			}
			else
				diwen_send_data16(0x1030,(uint16_t)high_limit*10);
			//printf("high_count=%0.3f\r\n",high_count);
			if(fabs(high_count-net_high_temp)>=1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				net_high_temp=high_count;
			}
			
			if((ALARM_FLAG_HIGH==0)&&(high_count>=high_limit))		//������ֵ
			{
				high_warning=1;
				ALARM_FLAG_HIGH=1;	//������Ϊ1 waring =0
			}
			if((ALARM_FLAG_HIGH==1)&&(high_count<high_limit))
			{
				ALARM_FLAG_HIGH=0;
				ALARM_FLAG=1;		//�ָ����ͱ�־
			}
			
			//high_speed=high_count-previous_high_count;		//��ʱ1s�ɼ�ʱ��
			high_speed=fabs(high_count-previous_high_count);
			//printf("high_speed=%0.3f\r\n",high_speed);
			if((high_count>previous_high_count)&&(high_speed>0.25f))
			{
				diwen_send_data16(0x1020,0x0001);
				diwen_send_data16(0x1040,(uint16_t)((high_speed+0.05f)*10));
			}
			else if((high_count<previous_high_count)&&(high_speed>0.25f))
			{
				diwen_send_data16(0x1020,0x0002);
				diwen_send_data16(0x1040,(uint16_t)((high_speed+0.05f)*10));
			}
			else if(high_speed<0.25f)
			{
				diwen_send_data16(0x1020,0x0000);
				diwen_send_data16(0x1040,0x0000);
			}
				previous_high_count=high_count;		//��¼Ϊ�ϴθ߶�ֵ
			

			
			/*************��Ǵ������ɼ�***************/
			memset(rs485Rx_Buf,0,256);
			//UART_Tx(qingjiao,7);
			UART_Tx(wendu,8);
			HAL_Delay(100);
			
			if(RS485_USARTx_RX_STA&0X8000)	//485���յ�һ��������
			{
				RS485_USARTx_RX_STA=0;			//��־��ʼ��	 
				//���ݱȶ�
				if(rs485Rx_Buf[7]==0x01)	//����
				{
					x_angle=rs485Rx_Buf[8]+(float)(rs485Rx_Buf[9]+rs485Rx_Buf[10]*256)/1000;
					diwen_send_data16(0x1050,x_angle*10);
					//printf("x_angle=%0.1f\r\n",x_angle);
				}
				if(rs485Rx_Buf[7]==0x00)	//����
				{
					x_angle=0-(rs485Rx_Buf[8]+(float)(rs485Rx_Buf[9]+rs485Rx_Buf[10]*256)/1000);
					diwen_send_data16(0x1050,x_angle*10);
					//printf("x_angle=%0.1f\r\n",x_angle);
				}
				if(rs485Rx_Buf[13]==0x01)	//����
				{
					y_angle=rs485Rx_Buf[14]+(float)(rs485Rx_Buf[15]+rs485Rx_Buf[16]*256)/1000;
					diwen_send_data16(0x1060,y_angle*10);
					//printf("y_angle=%0.1f\r\n",y_angle);
				}
				if(rs485Rx_Buf[13]==0x00)	//����
				{
					y_angle=0-(rs485Rx_Buf[14]+(float)(rs485Rx_Buf[15]+rs485Rx_Buf[16]*256)/1000);
					diwen_send_data16(0x1060,y_angle*10);
					//printf("y_angle=%0.1f\r\n",y_angle);
				}
				
				
				if((ALARM_FLAG_ANGLEX==0)&&(fabs(x_angle)>=angle_limit))		//������ֵ
				{
					angle_warning=1;
					ALARM_FLAG_ANGLEX=1;	//������Ϊ1 waring =0
				}
				if((ALARM_FLAG_ANGLEX==1)&&(fabs(x_angle)<angle_limit))
				{
					ALARM_FLAG_ANGLEX=0;
					ALARM_FLAG=1;		//�ָ����ͱ�־
				}
				if((ALARM_FLAG_ANGLEY==0)&&(fabs(y_angle)>=angle_limit))		//������ֵ
				{
					angle_warning=1;
					ALARM_FLAG_ANGLEY=1;	//������Ϊ1 waring =0
				}
				if((ALARM_FLAG_ANGLEY==1)&&(fabs(y_angle)<angle_limit))
				{
					ALARM_FLAG_ANGLEY=0;
					ALARM_FLAG=1;		//�ָ����ͱ�־
				}
				
			}		
			//else 
				//printf("485ͨѶ����\r\n");
			/*�ж��Ƿ񱨾����жϵ�Դ*/
			if((ALARM_FLAG_WEIGHT|ALARM_FLAG_HIGH|ALARM_FLAG_ANGLEX|ALARM_FLAG_ANGLEY)==1)
			{
				diwen_send_data16(0x1120,0x0001);	//��ֹ����
				HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);		//�ص�
				HAL_GPIO_WritePin(OUTPUT3_GPIO, OUTPUT3_GPIO_PIN, GPIO_PIN_RESET);  //�򿪱���
			}
			else
			{
				if(((IO1||IO2)==0)&&(work_register_sta==1))		//���ǰ���Źرգ���������֤״̬
				{
					diwen_send_data16(0x1120,0x0000);	//��������
					HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_SET);	//����
				}
				HAL_GPIO_WritePin(OUTPUT3_GPIO, OUTPUT3_GPIO_PIN, GPIO_PIN_SET);  //�رձ���
			}
			
			//printf("USART1_RX_STA=%d\r\n",USART1_RX_STA);
			Work_Count=0;
			HAL_TIM_Base_Start_IT(&htim5);
		}
		/****************���ڻ�***************************/
		if(USART1_RX_flg==1)	//USART1���յ�һ��������
		{
			USART1_RX_flg=0;
			uint8_t i;
			printf("name=%s\r\n",usart1Rx_Buf);
			for(i=0;i<8;i++)
			{
				name[i]=usart1Rx_Buf[21+i];
			}
			printf("name=%s\r\n",name);
			diwen_send_numascii(0x1070,name,8);	
			for(i=0;i<18;i++)
			{
				NameID[i]=usart1Rx_Buf[1+i];
			}
			diwen_send_numascii(0x1080,NameID,18);	
			
			/*************��Ա��Ϣ¼��ɹ����رտ��ڻ�*********************/
			work_register_sta=1;			//��Ϣ¼��ɹ���־,��ʼ����ʱ
			HAL_GPIO_WritePin(OUTPUT1_GPIO, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);  //����ߣ�����ر��޵�ѹ �رտ��ڻ�
			HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_SET);  //�򿪿���ϵͳ
			
			//�򿪵�Դ
			diwen_send_data16(0x10f0,worktime_limit);
			diwen_send_data16(0x1150,alreadyworktime);
			//���װ�ؼ���
			diwen_send_data16(0x1160,work_load_cnt);
			diwen_send_data16(0x1170,illegal_operation_cnt);
			io_check();
			
			memset(usart1Rx_Buf,0,256);
			
			register_time_flag=1;
			
		}
		
		
		/*******************************************/
		
		/*********************GSM����ͨ��*********************************/
		if((high_warning==1)||(weight_warning==1)||(angle_warning==1))			//��־λ�б����仯 
		{
			if(high_warning==1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\",\"info\":\"crash\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				HAL_Delay(100);
			}
			if(weight_warning==1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\",\"info\":\"overweight\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				HAL_Delay(100);
			}
			if(angle_warning==1)
			{
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\",\"info\":\"overangle\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,name,NameID,alreadyworktime);
				printf("EndStr=%s\r\n",EndStr);
				GSM_USART_printf("%s",EndStr);
				HAL_Delay(100);
			}
			high_warning=0;
			weight_warning=0;
			angle_warning=0;
			
		}

		if(ALARM_FLAG==1)
		{
			ALARM_FLAG=0;
			sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
				"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightlimit\":\"%d\",\"highlimit\":\"%d\",\"anglelimit\":\"%.1f\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_limit,high_limit,angle_limit,name,NameID,alreadyworktime);
			printf("EndStr=%s\r\n",EndStr);
			GSM_USART_printf("%s",EndStr);
		}
		
		/******************�������������ݴ���************************/
		if(HMI_USARTx_RX_STA&0X8000)	//���������յ�һ��������
		{
			HMI_USARTx_RX_STA=0;			//��־��ʼ��	 
			uint16_t addr;
			
			if(HMI_USARTx_RX_BUF[3]==0x83)	//���յ������޸���
			{
				addr=((uint16_t)HMI_USARTx_RX_BUF[4]<<8)|HMI_USARTx_RX_BUF[5];		//�õ���ַ
				switch(addr)
				{
					case 0x12b0:				//������������
						weight_zero_value=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[0]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[1]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("��������=%d\r\n",weight_zero_value);
						break;
					case 0x12c0:				//���ز���ֵ����
						weight_zero=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[2]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[3]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("���ز���ֵ=%d\r\n",weight_zero);
					break;
					case 0x12f0:				//װ��������������
						weight_add_value=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[4]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[5]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("װ������=%d\r\n",weight_add_value);
					break;
					case 0x1300:				//װ���������ֵ
						weight_add=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[6]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[7]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("װ�ز���ֵ=%d\r\n",weight_add);
					break;
					case 0x1340:				//������ֵ
						weight_limit=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[8]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[9]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("������ֵ=%d\r\n",weight_limit);
					break;
					
					case 0x1320:				//���ű궨
						back_door=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[10]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[11]=HMI_USARTx_RX_BUF[8];
						if(back_door==1)
						{
							IO2=!IO2;
						}
						write_flag=1;
						io_check();
						printf("����=%d\r\n",back_door);
					break;
					case 0x1310:				//ǰ�ű궨
						front_door=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[12]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[13]=HMI_USARTx_RX_BUF[8];
						if(front_door==1)
						{
							IO1=!IO1;
						}
						write_flag=1;
						io_check();
						printf("ǰ��=%d\r\n",front_door);
					break;
					
					case 0x1290:				//�����õ׾����߶�
						high_zero_value=(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/10;
						Tx_Buffer[14]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[15]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("�����õ׾����߶�=%d\r\n",high_zero_value);
						break;
					case 0x12a0:				//�����õײ���ֵ
						high_zero=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[16]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[17]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("�����õײ���ֵ=%d\r\n",high_zero);
					break;
					case 0x12d0:				//�����ö������߶�
						high_top_value=(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/10;
						Tx_Buffer[18]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[19]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("�����ö������߶�=%d\r\n",high_top_value);
					break;
					case 0x12e0:				//�����ö�����ֵ
						high_top=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[20]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[21]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("�����ö�����ֵ=%d\r\n",high_top);
					break;
					case 0x1330:				//�߶���ֵ
						high_limit=(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/10;
						Tx_Buffer[22]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[23]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("�߶���ֵ=%d\r\n",high_limit);
					break;
					case 0x1350:				//�����ֵ
						angle_limit=(float)(HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8])/100;
						Tx_Buffer[24]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[25]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("�����ֵ=%0.2f\r\n",angle_limit);
					break;			
					case 0x1360:				//��Ч����ʱ������
						worktime_limit=HMI_USARTx_RX_BUF[7]*256+HMI_USARTx_RX_BUF[8];
						Tx_Buffer[26]=HMI_USARTx_RX_BUF[7];
						Tx_Buffer[27]=HMI_USARTx_RX_BUF[8];
						write_flag=1;
						printf("��Ч����ʱ��=%d\r\n",worktime_limit);
					break;
					
					default: break;			

					
				}
				if(write_flag==1)
				{
					Tx_Buffer[28]=0x5a;
					/* ����SPI��������д�� */
					SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 				
					/* �����ͻ�����������д��flash�� */ 	
					SPI_FLASH_BufferWrite(Tx_Buffer,FLASH_WriteAddress, sizeof(Tx_Buffer));
					//calibrate_parameter();
					write_flag=0;
				}
			}	
		}		
		
	}
}

/**
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  __HAL_RCC_PWR_CLK_ENABLE();                                     //ʹ��PWRʱ��

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  //���õ�ѹ�������ѹ����1

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;      // �ⲿ����8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                        //��HSE 
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                    //��PLL
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;            //PLLʱ��Դѡ��HSE
  RCC_OscInitStruct.PLL.PLLM = 8;                                 //8��ƵMHz
  RCC_OscInitStruct.PLL.PLLN = 336;                               //336��Ƶ
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                     //2��Ƶ���õ�168MHz��ʱ��
  RCC_OscInitStruct.PLL.PLLQ = 7;                                 //USB/SDIO/������������ȵ���PLL��Ƶϵ��
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // ϵͳʱ�ӣ�168MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHBʱ�ӣ� 168MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;               // APB1ʱ�ӣ�42MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;               // APB2ʱ�ӣ�84MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_RCC_EnableCSS();                                            // ʹ��CSS���ܣ�����ʹ���ⲿ�����ڲ�ʱ��ԴΪ����
  
 	// HAL_RCC_GetHCLKFreq()/1000    1ms�ж�һ��
	// HAL_RCC_GetHCLKFreq()/100000	 10us�ж�һ��
	// HAL_RCC_GetHCLKFreq()/1000000 1us�ж�һ��
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);                // ���ò�����ϵͳ�δ�ʱ��
  /* ϵͳ�δ�ʱ��ʱ��Դ */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* ϵͳ�δ�ʱ���ж����ȼ����� */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * ��������: ���趨ֵ����У׼�����ͳ�ֵ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void calibrate_parameter(void)
{
	/***********����У׼ϵ������*************/
	weight_proportion=(float)(weight_add_value-weight_zero_value)/(weight_add-weight_zero);
	weight_initial_value=weight_add_value-weight_proportion*weight_add;
	printf("%0.1f->%d",weight_proportion,weight_initial_value);
	
	/*************�߶�У׼ϵ������*****************/
	high_proportion=(float)(high_top_value-high_zero_value)/(high_top-high_zero);
	high_initial_value=high_top_value-high_proportion*high_top;
	printf("%0.3f->%d",high_proportion,high_initial_value);
}

/**
  * ��������: ���ֵ����ֵ����
  * �������: ��
  * �� �� ֵ: ��ֵ����ֵ
  * ˵    ��: ��
  */
int32_t int_abs(int32_t value1,int32_t value2)
{
  if(value1>value2)
    return (value1-value2);
  else
    return (value2-value1);
}

/**
  * ��������: ��ֵƽ���˲�
  * �������: ��
  * �� �� ֵ: �˲����ֵ
  * ˵    ��: ��
  */
uint16_t filter(void)
{
	uint16_t value_buf[N],temp=0;   
	uint16_t sum=0;  
	unsigned char count,i,j;
	for(count=0;count<N;count++)
  {
		value_buf[count] = ADC_ConvertedValue[count];//�ȴ�ADת��
  }
                        
   for(j=0;j<N-1;j++)  //����
   {
     for(i=0;i<N-j-1;i++)        
     {
       if ( value_buf[i] > value_buf[i+1] )
        {
          temp = value_buf[i];
          value_buf[i] = value_buf[i+1];
          value_buf[i+1] = temp;
        }
     }
    }
	 for(count=15;count<N-15;count++)                                //��λ��ƽ�� ��Flag��־���� 
		{
     sum+=value_buf[count];
		}
		printf("sum=%f\r\n",sum*3.3/40960);
   return (uint16_t)(sum/(N-30));
}

/**
  * ��������: IO�ڼ�⹦��
  * �������: ��
  * �� �� ֵ: 
  * ˵    ��: ��
  */
void io_check(void)
{
	//printf("io1%d,io2%d,front_door%d,back_door%d\r\n",IO1,IO2,front_door,back_door);
	/**************ǰ��********************/
	
	if(IO1==0)
	{
		diwen_send_data16(0x1100,0x0000);
		//diwen_send_data16(0x1310,0x0000);
	}
	else if(IO1==1)
	{
		diwen_send_data16(0x1100,0x0001);
		//diwen_send_data16(0x1310,0x0001);
	} 
	/****************����***************************/
	if(IO2==0)
	{
		diwen_send_data16(0x1110,0x0000);
		//diwen_send_data16(0x1320,0x0000);
	}
	else if(IO2==1)
	{
		diwen_send_data16(0x1110,0x0001);
		//diwen_send_data16(0x1320,0x0001);
	} 
	/***********����״̬*************/
	if(((IO1||IO2)==0)&&(work_register_sta==1)&&((ALARM_FLAG_WEIGHT|ALARM_FLAG_HIGH|ALARM_FLAG_ANGLEX|ALARM_FLAG_ANGLEY)==0))	//ǰ���Źر��ұ�ʶ��״̬,�����ޱ����²���һ������װ��
	{
		HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_SET);	//����
		diwen_send_data16(0x1120,0x0000);
		work_load_cnt++;		//����װ�ؼ���һ��
		diwen_send_data16(0x1160,work_load_cnt);
		if(weight_count>=weight_limit)		//���� ����Υ��
		{
			illegal_operation_cnt++;
			diwen_send_data16(0x1170,illegal_operation_cnt);
		}
		weight_change=weight_count-weight_close;		//������=��ǰ����-�ϴι�������
		weight_close=weight_count;								//������������¼Ϊ�ϴι�������
		sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"load\",\"weight\":\"%d\",\"high\":\"%0.1f\",\"speed\":\"%0.1f\",\"xangle\":\"%0.1f\",\"yangle\":\"%0.1f\","
		"\"frontdoor\":\"%d\",\"backdoor\":\"%d\",\"weightchange\":\"%d\",\"author\":\"%s\",\"ID\":\"%s\",\"worktime\":\"%d\"}",ID_buff,weight_count,high_count,high_speed,x_angle,y_angle,IO1,IO2,weight_change,name,NameID,alreadyworktime);
		//printf("EndStr=%s\r\n",EndStr);
		GSM_USART_printf("%s",EndStr);
	}
	else
	{
		diwen_send_data16(0x1120,0x0001);
		HAL_GPIO_WritePin(OUTPUT4_GPIO, OUTPUT4_GPIO_PIN, GPIO_PIN_RESET);	//�ص�
	}
}

/**
  * ��������: ������ģʽ�¶�ʱ���Ļص�����
  * �������: htim����ʱ�����
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim6)
	{
		//printf("�������ݳ���:%d\r\n",RS485_USARTx_RX_STA);
		HAL_UART_Transmit(&husartx,rs485Rx_Buf,RS485_USARTx_RX_STA,0xffff);    //���ͽ��յ�������
		RS485_USARTx_RX_STA|=1<<15;	//��ǽ������
		HAL_TIM_Base_Stop_IT(&htim6);				//�رն�ʱ��
	}
	if(htim == &htim5)
	{
		Count++;
		timer_count++;
		Work_Count++;
	}
	if(htim == &htim7)
	{
//		printf("�������ݳ���:%d\r\n",HMI_USARTx_RX_STA);
//		HAL_UART_Transmit(&husartx,HMI_USARTx_RX_BUF,HMI_USARTx_RX_STA,0xffff);    //���ͽ��յ�������
		HMI_USARTx_RX_STA|=1<<15;	//��ǽ������
		
		HAL_TIM_Base_Stop_IT(&htim7);				//�رն�ʱ��
	}
}
/** 
  * ��������: ���ڽ����жϻص�����
  * �������: ���ھ��
  * �� �� ֵ: ��
  * ˵    ��: ʹ��һ����ʱ���ıȽ��жϺ͸����ж���Ϊ���ճ�ʱ�ж�
  *           ֻҪ���յ����ݾͽ���ʱ����������0,�������Ƚ��жϵ�ʱ��
  *           ˵���Ѿ���ʱ1.5���ַ���ʱ��,�϶�Ϊ֡����,����Ǹ����ж�
  *           ����Ϊ�ǽ������
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//485���ڽ��ջص�
  if(huart == &husart_485)
  {
		__HAL_TIM_SET_COUNTER(&htim6,0);	//���������
		if(RS485_USARTx_RX_STA==0)	//���ܽ������ݣ�������ʱ��
		{
			__HAL_TIM_CLEAR_FLAG(&htim6,TIM_FLAG_UPDATE);
      HAL_TIM_Base_Start_IT(&htim6);
		}
		rs485Rx_Buf[RS485_USARTx_RX_STA] = rs485RxBuffer;
    RS485_USARTx_RX_STA++;
		HAL_UART_Receive_IT(&husart_485,&rs485RxBuffer,1);	//�ٴο�������
  }
	
	//gsm���ڽ��ջص�
	if(huart == &husartx_GSM)
	{
		if(gsmuart_p<GSM_UART_BUFF_SIZE)
		{
			gsm_uart_buff[gsmuart_p] =gsmRxBuffer; 
			gsmuart_p++; 
			HAL_UART_Receive_IT(&husartx_GSM,&gsmRxBuffer,1);
		}
		else
		{
			clean_rebuff();       
		}
	}
	//���������ڽ��ջص�
	if(huart == &husartx_HMI)
	{
		__HAL_TIM_SET_COUNTER(&htim7,0);	//���������
		if(HMI_USARTx_RX_STA==0)	//���ܽ������ݣ�������ʱ��
		{
			__HAL_TIM_CLEAR_FLAG(&htim7,TIM_FLAG_UPDATE);
      HAL_TIM_Base_Start_IT(&htim7);
		}
		HMI_USARTx_RX_BUF[HMI_USARTx_RX_STA] = hmiRxBuffer;
    HMI_USARTx_RX_STA++;
		HAL_UART_Receive_IT(&husartx_HMI,&hmiRxBuffer,1);	//�ٴο�������
	}
	
	if(huart == &usartx_1)	//���ڻ�����
	{
		if((USART1_RX_STA==0)&&(aRxBuffer!=0x26))
		{
			USART1_RX_STA=0;
			USART1_RX_flg=0;
		}
		else
		{
			usart1Rx_Buf[USART1_RX_STA] = aRxBuffer;
			USART1_RX_STA++;
			
			if(aRxBuffer==0x26)
			{
				if(USART1_RX_STA!=1)
				{
					//USART1_RX_STA|=1<<15;	//��ǽ������
					USART1_RX_STA=0;
					USART1_RX_flg=1;
				}
			}
		}
		HAL_UART_Receive_IT(&usartx_1,&aRxBuffer,1);
	}
}

