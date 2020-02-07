#include "stdio.h"
#include "math.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "rs485.h"
#include "diwen.h"
#include "timer.h"
#include "4G_usart.h"
#include "spi.h"
#include "flash.h"
#include "hx711.h"

/*****��������***********/
void fitter(void);		//����˲�
void qingjiao_getDate(void);	//���
void fengsu_getDate(void); 	//����
void calibrate_parameter(void);	//У׼ֵ��������
void config_copy(void);	//��ʼ�����ò���
void Data_scan(void);		//�����޸ı�������
/* ˽�б��� */
u8 qingjiao[7]={0x5A,0xA5,0x20,0xf0,0x01,0x03,0x00};	//��ȡ��Ǵ�����ָ��
u8 fengsu[8]={0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0a};	//��ȡ��Ǵ�����ָ��
u16 addr;
/* �������� */
char EndStr[500];
u8 ID_buff[11];
u32 FLASH_SIZE;
u8 Tx_Buffer[65];		//spiͨ���ڴ棬���ֽڶ�д
u8 Rx_Buffer[65];
#define SIZE sizeof(Tx_Buffer)	 
u16 fengsuData;
u8 write_flag=0;		//д���־����һ���仯��Ҫ����д��flash
u8 check_id_flag=0;
u8 item=0;
//������ʾ��־λ
u8 waringview_gaodu=0;
u8 waringview_fududu=0;
u8 waringview_huizhuan=0;
u8 waringview_zhongliang=0;
u8 waringview_qingjiao=0;
u8 waringview_fengsu=0;
/***********�Ƕȱ���***************/
float x_angle,y_angle;
/***********�����������޸Ĳ�������***************/
float top_position;
u16 top_sampl;
float bottom_position;
u16 bottom_smapl;
float near_range;
u16 near_smapl;
float far_range;
u16 far_smapl;
float current_rotation;
u16 current_rotationsmapl;
float right_rotation;
u16 right_rotationsmapl;
float empty_weight;
u16 empty_weightsmapl;
float current_weight;
u16 current_weightsmapl;
float upper_limit;
float near_limit;
float remote_limit;
int16_t left_limit;
int16_t right_limit;
float level_limit0,level_limit1,vertical_limit0,vertical_limit1,rate_limit0,rate_limit1,windspeed_limit0,windspeed_limit1,angle_limit0,angle_limit1;	//height�����߶�
float height=50;
/***********��������ʾ��������***************/
float xiaobi_data,dabi_data,tagao,zuobiao_x,zuobiao_y,beilv;
float current_high;		//�߶�
float temp_current_high;	//��ʱֵ
float current_range;	//����
float temp_current_range;	//��ʱֵ
float current_rotation_data;	//��ת
float temp_current_rotation_data;	//��ʱֵ
float current_weight;		//����
float current_torque;		//���أ��ٷֱ�
float current_windspeed;		//����
float current_angle;	//�Ƕ�

char Dev_ID[10];	//�豸ID

float weight_proportion=0; //��������ϵ��
float high_proportion=0; //�߶ȱ���ϵ��
float Range_proportion=0;	//���ȱ���ϵ��
float rotation_proportion=0;	//��ת����ϵ��
int32_t weight_initial_value=0;	//��ʼֵ,�൱�ڹ�ʽ�е�b
int32_t high_initial_value=0;	//��ʼֵ,�൱�ڹ�ʽ�е�b
int32_t Range_initial_value=0;	//��ʼֵ,�൱�ڹ�ʽ�е�b
int32_t rotation_initial_value=0;	//��ʼֵ,�൱�ڹ�ʽ�е�b

/***������־λ***/
u8 fengsubaojing_flag=0;
u8 fengsuyujing_flag=0;
u8 qingjiaobaojing_flag=0;			
u8 qingjiaoyujing_flag=0;			
u8 gaodubaojing_flag=0;
u8 fudubaojing_flag=0;
u8 huizhuanbaojing_flag=0;
u8 chaozhongbaojing_flag=0;	

u8 ALARM_FLAG_gaodu=0;	//����������־����ֹ��α���������0:�������������ɴ������� 1���Ѵ���������ȴ�����״̬�����ɴ�������
u8 ALARM_FLAG_fudu=0;
u8 ALARM_FLAG_huizhuan=0;
u8 ALARM_FLAG_zhongliang=0;
u8 ALARM_FLAG_fengsu=0;
u8 ALARM_FLAG_qingjiao=0;
/* ���ڱ���ת�������ĵ�ѹֵ */	 
 __IO uint32_t ADC1ConvertedVoltage[3]; 
 __IO uint32_t ADC1ConvertedVoltage_temp[3]={6,6,6};
u32 ad=0;
/* ��չ���� ------------------------------------------------------------------*/
extern __IO u16 ADC1ConvertedValue[64][ADC_NUMOFCHANNEL];


 int main(void)
 {	
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	control_init();
	LED_Init();
	uart_init(115200);	 //���ڳ�ʼ��Ϊ9600
	RS485_Init(36,9600);//485��ʼ��
	UART4_Init(115200);	//���������ڳ�ʼ��	
	GSM_Init();			//4Gģ���ʼ��
	SPI_Flash_Init();  		//SPI FLASH ��ʼ�� 
	while(SPI_Flash_ReadID()!=W25Q64)							//��ⲻ��W25Q64
	{
		printf("flash error\r\n");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
	FLASH_SIZE=8*1024*1024;	//FLASH ��СΪ8M�ֽ�
  /* ADC ��ʼ�� */
	ADCx_Init();
	ADInit(); 
  TIM3_Init(999,7199);		//100ms�ж�
	delay_ms(2000);
	liebiao_view();
	time3_start();		//��ʱ����
  /* ���ز�����ʼ�� */
	config_copy();
	calibrate_parameter();
	while(1)
	{
		if(net_flag==1)
		{
			net_flag=0;
			if(check_id_flag==0)		//�豸ID��ȡ ����ֱ���ɹ�
			{
				/* ����״̬������豸ID��ȡ */
				if(gprs_send_cmd((u8*)"usr.cnAT+IMEI\r\n",(u8*)"+IMEI:",100)==0)
				{
					u8 i;
					u8 tmpbuff[7];
					for(i=0;i<6;i++)
					{
						tmpbuff[i]=UART5_RX_BUF[18+i];//�޻��ԣ��У�24
					}
					tmpbuff[6]='\0';
					//printf("%s\r\n",tmpbuff);
					sprintf((char *)ID_buff,"THHY%s",tmpbuff);
					printf("%s",ID_buff);
					diwen_send_numascii(0x1510,ID_buff,11);
					SPI_Flash_Write((u8*)ID_buff,FLASH_SIZE-200,sizeof(ID_buff));		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
					check_id_flag=1;
				}
			}
			if(gprs_send_cmd((u8*)"usr.cnAT+SOCKALK\r\n",(u8*)"+SOCKALK:ON",100)==0)
			{
				printf("�����ɹ�\r\n");
				diwen_send_numascii(0x1520,"OK",2);
			} 
			else
			{
				printf("����ʧ��\r\n");
				diwen_send_numascii(0x1520,"NO",2);
			}
			sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"jump\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
			"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"upper_limit\":\"%.1f\",\"near_limit\":\"%.1f\",\"remote_limit\":\"%.1f\",\"left_limit\":\"%d\",\"right_limit\":\"%d\",\"windspeed_limit\":\"%.1f\",\"angle_limit\":\"%.1f\",\"weight_limit\":\"%.1f\",\"height\":\"%.1f\"}",ID_buff,current_weight,\
			current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle,upper_limit,near_limit,remote_limit,left_limit,right_limit,windspeed_limit0,angle_limit0,my5010[1][1],height);
			//printf("%s",EndStr);
			//printf("111=%d\r\n",strlen(EndStr));
			gsm_printf("%s",EndStr);
		}
		/*************���Ĵ��������ղ�����ȡ***************/
		if(USART4_RX_STA&0X8000)	//���������յ�һ��������
		{
			USART4_RX_STA=0;			//��־��ʼ��	 
			Data_scan();
		}
		
		/*100msAD���ݲɼ�*/
		if(ad_flag==1)
		{
			fitter();
			if(abs(ADC1ConvertedVoltage[0]-ADC1ConvertedVoltage_temp[0])>4)		//����AD����ֵ����1�ı仯��ˢ��
			{
//				printf("ad=%d\r\n",ADC1ConvertedVoltage[0]);
//				printf("adtemp=%d\r\n",ADC1ConvertedVoltage_temp[0]);
				diwen_send_data16(0x1720,ADC1ConvertedVoltage[0]);
				ADC1ConvertedVoltage_temp[0]=ADC1ConvertedVoltage[0];
				//�߶�ֵ
				current_high=((float)ADC1ConvertedVoltage[0]-top_sampl)/high_proportion+top_position;		//�߶�ֵ
				if(current_high<0)		//����ɼ������ݳ������ޣ�ǿ��Ϊ0
				{
					current_high=0;
				}
				if(current_high>500)		//����ɼ������ݳ������ޣ�ǿ��Ϊ0
				{
					current_high=bottom_position+1;
				}
			//printf("current_high=%.1f\r\n",current_high);
//			if(fabs(current_high-temp_current_high)>=0.5f)		//ʵ����������
//			{
					if(current_high>=0&&current_high<500)			//��ֹ�������
					{
						diwen_send_data16(0x1530,current_high*10);
						//data�����ϱ�
						if(fabs(current_high-temp_current_high)>=1.5f)
						{
							sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
							"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\"}",ID_buff,current_weight,\
							current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
							//printf("%s",EndStr);
							gsm_printf("%s",EndStr);
						}
						temp_current_high=current_high;
					}
//			}
				if((current_high-upper_limit)<=0)		//���б���
				{
					if(ALARM_FLAG_gaodu==0)
					{
						diwen_send_data16(0x1590,0);	//����״̬ͼ��
						diwen_send_numascii(0x1650,"�߶�״̬����",12);
						gaodubaojing_flag=1;
						ALARM_FLAG_gaodu=1;
					}
				}
				if((current_high-bottom_position)>=0)		//���б���
				{
					if(ALARM_FLAG_gaodu==0)
					{
						diwen_send_data16(0x1600,0);	//����״̬����ͼ��
						diwen_send_numascii(0x1650,"�߶�״̬����",12);
						gaodubaojing_flag=1;
						ALARM_FLAG_gaodu=1;
					}
				}
				if(((current_high-upper_limit)>0)&&((current_high-bottom_position)<0))
				{
					if(ALARM_FLAG_gaodu==1)
					{
						diwen_send_data16(0x1590,1);
						diwen_send_data16(0x1600,1);
					}
					ALARM_FLAG_gaodu=0;
					gaodubaojing_flag=0;
				}
			}
			if(abs(ADC1ConvertedVoltage[1]-ADC1ConvertedVoltage_temp[1])>4)
			{
				diwen_send_data16(0x1722,ADC1ConvertedVoltage[1]);
				ADC1ConvertedVoltage_temp[1]=ADC1ConvertedVoltage[1];
					//����ֵ
				current_range=((float)ADC1ConvertedVoltage[1]-near_smapl)/Range_proportion+near_range;	//����ֵ

				if(current_range<0)		//����ɼ������ݳ������ޣ�ǿ��Ϊ0
				{
					current_range=0;
				}
				if(current_range>500)	
				{
					current_range=remote_limit+1;
				}
			
			
//			if(fabs(current_range-temp_current_range)>=0.5f)
//			{
				if(current_range>=0&&current_range<500)			//��ֹ�������
				{
					diwen_send_data16(0x1540,current_range*10);
					if(fabs(current_range-temp_current_range)>=1.5f)
					{
						//data�����ϱ�
						sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
						"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\"}",ID_buff,current_weight,\
						current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
						//printf("%s",EndStr);
						gsm_printf("%s",EndStr);
					}
					temp_current_range=current_range;
				}
//			}
			
				if((current_range-near_limit)<0)		//���˱���
				{
					if(ALARM_FLAG_fudu==0)
					{
						diwen_send_data16(0x1620,0);
						diwen_send_numascii(0x1650,"С���н�����",12);
						fudubaojing_flag=1;
						ALARM_FLAG_fudu=1;
					}
				}
				else if((current_range-remote_limit)>0)	//Զ�˱���
				{
					if(ALARM_FLAG_fudu==0)
					{
						diwen_send_data16(0x1610,0);
						diwen_send_numascii(0x1650,"С���н�����",12);
						fudubaojing_flag=1;
						ALARM_FLAG_fudu=1;
					}
				}
				else
				{
					if(ALARM_FLAG_fudu==1)
					{
						diwen_send_data16(0x1610,1);
						diwen_send_data16(0x1620,1);
					}
					fudubaojing_flag=0;
					ALARM_FLAG_fudu=0;
				//diwen_send_numascii(0x1650,"            ",12);
				}
			
			}
			if(abs(ADC1ConvertedVoltage[2]-ADC1ConvertedVoltage_temp[2])>4)
			{
				diwen_send_data16(0x1724,ADC1ConvertedVoltage[2]);
				ADC1ConvertedVoltage_temp[2]=ADC1ConvertedVoltage[2];
				//��תֵ
				current_rotation_data=((int32_t)ADC1ConvertedVoltage[2]-current_rotationsmapl)/rotation_proportion+current_rotation;		//��תֵ
			
//				if(fabs(current_rotation_data-temp_current_rotation_data)>=5)
//				{
					diwen_send_data16(0x1545,(int16_t)current_rotation_data*10);
					//printf("current_rotation_data=%f\r\n",current_rotation_data);
					if(current_rotation_data>=0)		
					{
						diwen_send_data16(0x0120,(int)current_rotation_data%360);
					}
					else
					{
						diwen_send_data16(0x0120,((int)current_rotation_data+360*4)%360);
					}
				//data�����ϱ�
					if(fabs(current_rotation_data-temp_current_rotation_data)>=15)
					{
						sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"data\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
						"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\"}",ID_buff,current_weight,\
						current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
						//printf("%s",EndStr);
						gsm_printf("%s",EndStr);
					}
					temp_current_rotation_data=current_rotation_data;
//				}
			
			
				if(current_rotation_data<left_limit)	//��������ת
				{
					if(ALARM_FLAG_huizhuan==0)
					{
						FIFO3=1;	//ֹͣ��ת
						diwen_send_data16(0x1630,0);
						diwen_send_numascii(0x1650,"��ۻ�ת����",12);
						huizhuanbaojing_flag=1;
						ALARM_FLAG_huizhuan=1;
					}
				}
				else if(current_rotation_data>right_limit)	//��������ת
				{
					if(ALARM_FLAG_huizhuan==0)
					{
						FIFO4=1;	//ֹͣ��ת
						diwen_send_data16(0x1640,0);
						diwen_send_numascii(0x1650,"��ۻ�ת����",12);
						huizhuanbaojing_flag=1;
						ALARM_FLAG_huizhuan=1;
					}
				}
				else
				{
					if(ALARM_FLAG_huizhuan==1)
					{
						FIFO3=0;
						FIFO4=0;
						diwen_send_data16(0x1630,1);
						diwen_send_data16(0x1640,1);
					}
					huizhuanbaojing_flag=0;
					ALARM_FLAG_huizhuan=0;
				}
			}	
			
		}
				
		
		/************ ���������ݲɼ� ***********************/
		if(worktimes_flag==1)
		{
			time3_stop();
			worktimes_flag=0;
			/*******485�ഫ�������ݲɼ�***************
			**************************************/
			memset(RS485_RX_BUF,0,64);
			RS485_Send_Data(fengsu,8);
			delay_ms(100);
			fengsu_getDate();
			/*************��Ǵ������ɼ�***************/
			memset(RS485_RX_BUF,0,64);
			RS485_Send_Data(qingjiao,7);
			delay_ms(100);
			qingjiao_getDate();
			
			
			if((ALARM_FLAG_gaodu||ALARM_FLAG_fudu)==0)	//������
			{
				diwen_send_rectangle(0x2000,134+(temp_current_range)*260/50,90,136+(temp_current_range)*260/50,(temp_current_high)*300/bottom_position+90);
				diwen_send_rectangle(0x2100,125+(temp_current_range)*260/50,(temp_current_high)*300/bottom_position+90,145+(temp_current_range)*260/50,(temp_current_high)*300/bottom_position+110);
			}
			

			ad=ReadAD()/838.8608;
			//printf("ad=%d\r\n",ad);
			diwen_send_data16(0x1726,ad);
			if(ad<empty_weightsmapl)		//�������ֵС��ǿ��Ϊ0
			{
				ad=empty_weightsmapl;
			}
			current_weight=(ad-empty_weightsmapl)/weight_proportion;	//����ֵ
			diwen_send_data16(0x1550,current_weight*1000);
			
			//printf("current_range=%f,current_weight=%.4f\r\n",current_range,current_weight);
			if(current_range>0&&current_range<my5010[0][1])
			{
				current_torque=	current_weight*current_range/my5010[0][1]/my5010[1][1];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][1])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][1]&&current_range<my5010[0][2])
			{
				current_torque=	current_weight*current_range/my5010[0][2]/my5010[1][2];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][2])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][2]&&current_range<my5010[0][3])
			{
				current_torque=	current_weight*current_range/my5010[0][3]/my5010[1][3];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][3])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][3]&&current_range<my5010[0][4])
			{
				current_torque=	current_weight*current_range/my5010[0][4]/my5010[1][4];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][4])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][4]&&current_range<my5010[0][5])
			{
				current_torque=	current_weight*current_range/my5010[0][5]/my5010[1][5];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][5])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][5]&&current_range<my5010[0][6])
			{
				current_torque=	current_weight*current_range/my5010[0][6]/my5010[1][6];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][6])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][6]&&current_range<my5010[0][7])
			{
				current_torque=	current_weight*current_range/my5010[0][7]/my5010[1][7];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][7])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][7]&&current_range<my5010[0][8])
			{
				current_torque=	current_weight*current_range/my5010[0][8]/my5010[1][8];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][8])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else 
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
			if(current_range>my5010[0][8])
			{
				current_torque=	current_weight*remote_limit/my5010[0][8]/my5010[1][8];
				//printf("current_torque=%.1f\r\n",current_torque);
				diwen_send_data16(0x1560,current_torque*1000);
				if(current_weight<my5010[1][8])
				{
					//printf("������");
					ALARM_FLAG_zhongliang=0;
					chaozhongbaojing_flag=0;
				}
				else 
				{
					//printf("����");
					diwen_send_numascii(0x1650,"�������ر���",12);
					if(ALARM_FLAG_zhongliang==0)
					{
						chaozhongbaojing_flag=1;
						ALARM_FLAG_zhongliang=1;
					}
				}
			}
				/*******��ֵ�ж�**********/		
			if((ALARM_FLAG_zhongliang||ALARM_FLAG_gaodu||ALARM_FLAG_fudu||ALARM_FLAG_huizhuan||ALARM_FLAG_fengsu||ALARM_FLAG_qingjiao)==0)
			{
				diwen_send_numascii(0x1650,"            ",12);
			}	
			if((ALARM_FLAG_zhongliang||ALARM_FLAG_fengsu||ALARM_FLAG_qingjiao)==0)
			{
				if((ALARM_FLAG_gaodu==1)&&item==2)
				{
					diwen_send_numascii(0x1650,"�߶�״̬����",12);
				}
				if((ALARM_FLAG_fudu==1)&&item==4)
				{
					diwen_send_numascii(0x1650,"С���н�����",12);
				}
				if((ALARM_FLAG_huizhuan==1)&&item==6)
				{
					diwen_send_numascii(0x1650,"��ۻ�ת����",12);
				}
			}
			
			if(item==6)
			{
				item=0;
			}
			item++;
			time3_start();
			/********************************************************************/
		} 
		
		
		/*****������־λ�ж�*******/
		if((fengsubaojing_flag||qingjiaobaojing_flag||gaodubaojing_flag||fudubaojing_flag||huizhuanbaojing_flag||chaozhongbaojing_flag)==1)
		{
			if(fengsubaojing_flag==1)
			{
				//data�����ϱ�
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"windspeed\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(qingjiaobaojing_flag==1)
			{
				//data�����ϱ�
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overangle\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(gaodubaojing_flag==1)
			{
				//data�����ϱ�
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overhigh\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(fudubaojing_flag==1)
			{
				//data�����ϱ�
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overfudu\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(huizhuanbaojing_flag==1)
			{
				//data�����ϱ�
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overhuizhuan\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			if(chaozhongbaojing_flag==1)
			{
				//data�����ϱ�
				sprintf(EndStr,"{\"DevID\":\"%s\",\"action\":\"alarm\",\"weight\":\"%.3f\",\"high\":\"%.1f\",\"range\":\"%0.1f\",\"rotation\":\"%.1f\",\"torque\":\"%.1f\","
				"\"windspeed\":\"%.1f\",\"angle\":\"%.1f\",\"info\":\"overweight\"}",ID_buff,current_weight,\
				current_high,current_range,current_rotation_data,current_torque,current_windspeed,current_angle);
				//printf("%s",EndStr);
				gsm_printf("%s",EndStr);
			}
			fengsubaojing_flag=0;
			qingjiaobaojing_flag=0;
			gaodubaojing_flag=0;
			fudubaojing_flag=0;
			huizhuanbaojing_flag=0;
			chaozhongbaojing_flag=0;
		}
		

		//printf("***************%d.%d.%d.%d.%d.%d\r\n",ALARM_FLAG_zhongliang,ALARM_FLAG_gaodu,ALARM_FLAG_fudu,ALARM_FLAG_huizhuan,ALARM_FLAG_fengsu,ALARM_FLAG_qingjiao);
		if(ALARM_FLAG_zhongliang==1)		//����������������ֻ���½�
		{
			FIFO1=1;	//��������״̬��
			FIFO2=1;	//����ǰ��
		}
		else
		{
			FIFO1=0;
			FIFO2=0;
		}
	}	 
}

///***��ֵ�˲�����****/
//void fitter(void)
//{
//	u8 i,j;
//	ADC1ConvertedVoltage[0] = 0;
//  ADC1ConvertedVoltage[1] = 0;
//	ADC1ConvertedVoltage[2] = 0;
//  //ADC1ConvertedVoltage[3] = 0;
//	for(i=0;i<3;i++)
//  { 
//		for(j=0;j<64;j++)
//		{
//     ADC1ConvertedVoltage[i]+= ADC1ConvertedValue[j][i];
//    }
//		 ADC1ConvertedVoltage[i] = ADC1ConvertedVoltage[i]/64;
//	}
//}

/***��ֵ�˲�����****/
void fitter(void)
{
	u8 i,j,count;
	u16 temp=0;
	u16 value_buf1[64];
	u16 value_buf2[64];
	u16 value_buf3[64];
	ADC1ConvertedVoltage[0] = 0;
  ADC1ConvertedVoltage[1] = 0;
	ADC1ConvertedVoltage[2] = 0;
	for(count=0;count<64;count++)
  {
		value_buf1[count] = ADC1ConvertedValue[count][0];//�ȴ�ADת��
		value_buf2[count] = ADC1ConvertedValue[count][1];//�ȴ�ADת��
		value_buf3[count] = ADC1ConvertedValue[count][2];//�ȴ�ADת��
  }
	
	for(j=0;j<64-1;j++)  //����
   {
     for(i=0;i<64-j-1;i++)        
     {
       if ( value_buf1[i] > value_buf1[i+1] )
        {
          temp = value_buf1[i];
          value_buf1[i] = value_buf1[i+1];
          value_buf1[i+1] = temp;
        }
     }
    }
	 for(j=0;j<64-1;j++)  //����
   {
     for(i=0;i<64-j-1;i++)        
     {
       if ( value_buf2[i] > value_buf2[i+1] )
        {
          temp = value_buf2[i];
          value_buf2[i] = value_buf2[i+1];
          value_buf2[i+1] = temp;
        }
     }
    }
	 for(j=0;j<64-1;j++)  //����
   {
     for(i=0;i<64-j-1;i++)        
     {
       if ( value_buf3[i] > value_buf3[i+1] )
        {
          temp = value_buf3[i];
          value_buf3[i] = value_buf3[i+1];
          value_buf3[i+1] = temp;
        }
     }
    }
	for(count=10;count<64-10;count++)                                //��λ��ƽ�� ��Flag��־���� 
		{
			ADC1ConvertedVoltage[0]+=value_buf1[count];
			ADC1ConvertedVoltage[1]+=value_buf2[count];
			ADC1ConvertedVoltage[2]+=value_buf3[count];
		}
		ADC1ConvertedVoltage[0]=ADC1ConvertedVoltage[0]/44;
		ADC1ConvertedVoltage[1]=ADC1ConvertedVoltage[1]/44;
		ADC1ConvertedVoltage[2]=ADC1ConvertedVoltage[2]/44;
		
}

/**
  * ��������: ���ٶ�ȡ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void fengsu_getDate(void)
{
	if(rs485_recv_flag==1)	//485���յ�һ��������
		{
			rs485_recv_flag=0;			//��־��ʼ��		
			RS485_RX_CNT=0;			
			//���ݴ���
			if(RS485_RX_BUF[2]==0x02)	//���յ�������
			{
				fengsuData=RS485_RX_BUF[3]*256+RS485_RX_BUF[4];
				diwen_send_data16(0x1570,fengsuData);
				current_windspeed=fengsuData/10;
				//printf("fengsu=%.1fm/s\r\n",current_windspeed);
				if(current_windspeed>windspeed_limit0)
				{
					if(ALARM_FLAG_fengsu==0)
					{
						diwen_send_numascii(0x1650,"�������ٱ���",12);
						ALARM_FLAG_fengsu=1;
						fengsubaojing_flag=1;
					}
					
				}
				else if(current_windspeed>windspeed_limit1&&current_windspeed<windspeed_limit0)
				{
					fengsuyujing_flag=1;
					fengsubaojing_flag=0;
					ALARM_FLAG_fengsu=0;
				}
				else
				{
					fengsubaojing_flag=0;
					fengsuyujing_flag=0;
					ALARM_FLAG_fengsu=0;
				}
				
			}
		}	
		else
				printf("error\r\n");
}

/**
  * ��������: ��Ǵ�������ȡ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void qingjiao_getDate(void)
{
	if(rs485_recv_flag==1)	//485���յ�һ��������
		{
			rs485_recv_flag=0;			//��־��ʼ��		
			RS485_RX_CNT=0;			
				//���ݱȶ�
			if(RS485_RX_BUF[7]==0x01)	//����
			{
				x_angle=RS485_RX_BUF[8]+(float)(RS485_RX_BUF[9]+RS485_RX_BUF[10]*256)/1000;
				diwen_send_data16(0x1580,x_angle*10);
				//printf("x_angle=%0.1f\r\n",x_angle);
			}
			if(RS485_RX_BUF[7]==0x00)	//����
			{
				x_angle=0-(RS485_RX_BUF[8]+(float)(RS485_RX_BUF[9]+RS485_RX_BUF[10]*256)/1000);
				diwen_send_data16(0x1580,x_angle*10);
				//printf("x_angle=%0.1f\r\n",x_angle);
			}
			if(RS485_RX_BUF[13]==0x01)	//����
			{
				y_angle=RS485_RX_BUF[14]+(float)(RS485_RX_BUF[15]+RS485_RX_BUF[16]*256)/1000;
				//diwen_send_data16(0x1060,y_angle*10);
				//printf("y_angle=%0.1f\r\n",y_angle);
			}
			if(RS485_RX_BUF[13]==0x00)	//����
			{
				y_angle=0-(RS485_RX_BUF[14]+(float)(RS485_RX_BUF[15]+RS485_RX_BUF[16]*256)/1000);
				//diwen_send_data16(0x1060,y_angle*10);
				//printf("y_angle=%0.1f\r\n",y_angle);
			}		
		}
		current_angle=fabs(x_angle);
		if(current_angle>angle_limit0) 
		{
			if(ALARM_FLAG_qingjiao==0)
			{
				diwen_send_numascii(0x1650,"������Ǳ���",12);
				ALARM_FLAG_qingjiao=1;
				qingjiaobaojing_flag=1;
			}
			
		}
		else if(current_angle>angle_limit1&&current_angle<angle_limit0)
		{
			qingjiaoyujing_flag=1;
			qingjiaobaojing_flag=0;
			ALARM_FLAG_qingjiao=0;
		}
		else
		{
			qingjiaobaojing_flag=0;
			qingjiaoyujing_flag=0;
			ALARM_FLAG_qingjiao=0;
		}
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
	weight_proportion=(float)(current_weightsmapl-empty_weightsmapl)/(current_weight-empty_weight);
	weight_initial_value=current_weightsmapl-weight_proportion*current_weight;
	printf("%0.1f->%d\r\n",weight_proportion,weight_initial_value);
	
	/*************�߶�У׼ϵ������*****************/
	high_proportion=(float)(bottom_smapl-top_sampl)/(bottom_position-top_position);
	high_initial_value=bottom_smapl-high_proportion*bottom_position;
	printf("%0.3f->%d\r\n",high_proportion,high_initial_value);
	
	/*************����У׼ϵ������*****************/
	Range_proportion=(float)(far_smapl-near_smapl)/(far_range-near_range);
	Range_initial_value=far_smapl-Range_proportion*far_range;
	printf("%0.3f->%d\r\n",Range_proportion,Range_initial_value);
	
	/*************��תУ׼ϵ������*****************/
	rotation_proportion=(float)(right_rotationsmapl-current_rotationsmapl)/(right_rotation-current_rotation);
	rotation_initial_value=right_rotationsmapl-rotation_proportion*right_rotation;
	printf("%0.3f->%d\r\n",rotation_proportion,rotation_initial_value);
}

/**
  * ��������: ���ݳ�ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void config_copy(void)
{
	 printf("----��ʼ���ݿ���------\r\n"); 
	SPI_Flash_Read((u8*)Rx_Buffer,FLASH_SIZE-100,SIZE);				//�ӵ�����100����ַ����ʼ,��70���ֽ�
	if(Rx_Buffer[64]==0x5a)		//�����ݸĶ��ˣ�ֱ�Ӷ�ȡ��ʾ
	{
		u8 i;
		/**********��rx���ݸ��µ�tx��ֹ�����buf���ݱ����д0��flash*******************/
		for(i=0;i<65;i++)
		{
			Tx_Buffer[i]=Rx_Buffer[i];
		}
		top_position=(float)(Rx_Buffer[0]*256+Rx_Buffer[1])/10;
		diwen_send_data16(0x1728,top_position*10);
		printf("top_position=%.1f\r\n",top_position);
		
		top_sampl=Rx_Buffer[2]*256+Rx_Buffer[3];
		diwen_send_data16(0x1736,top_sampl);
		printf("top_sampl=%d\r\n",top_sampl);
		
		bottom_position=(float)(Rx_Buffer[4]*256+Rx_Buffer[5])/10;
		diwen_send_data16(0x1744,bottom_position*10);
		printf("bottom_position=%.1f\r\n",bottom_position);
		
		bottom_smapl=Rx_Buffer[6]*256+Rx_Buffer[7];
		diwen_send_data16(0x1752,bottom_smapl);
		printf("bottom_smapl=%d\r\n",bottom_smapl);
		
		near_range=(float)(Rx_Buffer[8]*256+Rx_Buffer[9])/10;
		diwen_send_data16(0x1730,near_range*10);
		printf("near_range=%.1f\r\n",near_range);
		
		near_smapl=Rx_Buffer[10]*256+Rx_Buffer[11];
		diwen_send_data16(0x1738,near_smapl);
		printf("near_smapl=%d\r\n",near_smapl);
		
		far_range=(float)(Rx_Buffer[12]*256+Rx_Buffer[13])/10;
		diwen_send_data16(0x1746,far_range*10);
		printf("far_range=%.1f\r\n",far_range);
		
		far_smapl=Rx_Buffer[14]*256+Rx_Buffer[15];
		diwen_send_data16(0x1754,far_smapl);
		printf("far_smapl=%d\r\n",far_smapl);
		
		current_rotation=(float)(Rx_Buffer[16]*256+Rx_Buffer[17])/10;
		diwen_send_data16(0x1732,current_rotation*10);
		printf("current_rotation=%.1f\r\n",current_rotation);
		
		current_rotationsmapl=Rx_Buffer[18]*256+Rx_Buffer[19];
		diwen_send_data16(0x1740,current_rotationsmapl);
		printf("current_rotationsmapl=%d\r\n",current_rotationsmapl);
		
		right_rotation=(float)(Rx_Buffer[20]*256+Rx_Buffer[21])/10;
		diwen_send_data16(0x1748,right_rotation*10);
		printf("right_rotation=%.1f\r\n",right_rotation);
		
		right_rotationsmapl=Rx_Buffer[22]*256+Rx_Buffer[23];
		diwen_send_data16(0x1756,right_rotationsmapl);
		printf("right_rotationsmapl=%d\r\n",right_rotationsmapl);
		
		empty_weight=(float)(Rx_Buffer[24]*256+Rx_Buffer[25])/1000;
		diwen_send_data16(0x1734,empty_weight*1000);
		printf("empty_weight=%.3f\r\n",empty_weight);
		
		empty_weightsmapl=Rx_Buffer[26]*256+Rx_Buffer[27];
		diwen_send_data16(0x1742,empty_weightsmapl);
		printf("empty_weightsmapl=%d\r\n",empty_weightsmapl);
		
		current_weight=(float)(Rx_Buffer[28]*256+Rx_Buffer[29])/1000;
		diwen_send_data16(0x1750,current_weight*1000);
		printf("current_weight=%.1f\r\n",current_weight);
		
		current_weightsmapl=Rx_Buffer[30]*256+Rx_Buffer[31];
		diwen_send_data16(0x1758,current_weightsmapl);
		printf("current_weightsmapl=%d\r\n",current_weightsmapl);
		
		upper_limit=(float)(Rx_Buffer[32]*256+Rx_Buffer[33])/10;
		diwen_send_data16(0x1762,upper_limit*10);
		printf("upper_limit=%.1f\r\n",upper_limit);
		
		near_limit=(float)(Rx_Buffer[34]*256+Rx_Buffer[35])/10;
		diwen_send_data16(0x1766,near_limit*10);
		printf("near_limit=%.1f\r\n",near_limit);
		
		remote_limit=(float)(Rx_Buffer[36]*256+Rx_Buffer[37])/10;
		diwen_send_data16(0x1768,remote_limit*10);
		printf("remote_limit=%.1f\r\n",remote_limit);
		
		left_limit=(int16_t)(Rx_Buffer[38]*256+Rx_Buffer[39])/10;
		diwen_send_data16(0x1772,left_limit*10);
		printf("left_limit=%d\r\n",left_limit);
		
		right_limit=(float)(Rx_Buffer[40]*256+Rx_Buffer[41])/10;
		diwen_send_data16(0x1774,right_limit*10);
		printf("right_limit=%df\r\n",right_limit);
		
		level_limit0=(float)(Rx_Buffer[42]*256+Rx_Buffer[43])/10;
		diwen_send_data16(0x1776,level_limit0*10);
		printf("level_limit0=%.1f\r\n",level_limit0);
		
		level_limit1=(float)(Rx_Buffer[44]*256+Rx_Buffer[45])/10;
		diwen_send_data16(0x1790,level_limit1*10);
		printf("level_limit1=%.1f\r\n",level_limit1);
		
		vertical_limit0=(float)(Rx_Buffer[46]*256+Rx_Buffer[47])/10;
		diwen_send_data16(0x1778,vertical_limit0*10);
		printf("vertical_limit0=%.1f\r\n",vertical_limit0);
		
		vertical_limit1=(float)(Rx_Buffer[48]*256+Rx_Buffer[49])/10;
		diwen_send_data16(0x1792,vertical_limit1*10);
		printf("vertical_limit1=%.1f\r\n",vertical_limit1);
		
		rate_limit0=(float)(Rx_Buffer[50]*256+Rx_Buffer[51])/10;
		diwen_send_data16(0x1780,rate_limit0*10);
		printf("rate_limit0=%.1f\r\n",rate_limit0);
		
		rate_limit1=(float)(Rx_Buffer[52]*256+Rx_Buffer[53])/10;
		diwen_send_data16(0x1794,rate_limit1*10);
		printf("rate_limit1=%.1f\r\n",rate_limit1);
		
		windspeed_limit0=(float)(Rx_Buffer[54]*256+Rx_Buffer[55])/10;
		diwen_send_data16(0x1782,windspeed_limit0*10);
		printf("windspeed_limit0=%.1f\r\n",windspeed_limit0);
		
		windspeed_limit1=(float)(Rx_Buffer[56]*256+Rx_Buffer[57])/10;
		diwen_send_data16(0x1796,windspeed_limit1*10);
		printf("windspeed_limit1=%.1f\r\n",windspeed_limit1);
		
		angle_limit0=(float)(Rx_Buffer[58]*256+Rx_Buffer[59])/10;
		diwen_send_data16(0x1784,angle_limit0*10);
		printf("angle_limit0=%.1f\r\n",angle_limit0);
		
		angle_limit1=(float)(Rx_Buffer[60]*256+Rx_Buffer[61])/10;
		diwen_send_data16(0x1798,angle_limit1*10);
		printf("angle_limit1=%.1f\r\n",angle_limit1);
		
		height=(float)(Rx_Buffer[62]*256+Rx_Buffer[63])/10;
		diwen_send_data16(0x1680,height*10);
		printf("height=%.1f\r\n",height);
	}
	delay_ms(1000);
	printf("----��ʼ��ȡID��------\r\n");
	SPI_Flash_Read(ID_buff,FLASH_SIZE-200,sizeof(ID_buff));				//�ӵ�����100����ַ����ʼ,��70���ֽ�
	if(ID_buff[0]=='T')
	{
		printf("�������");
		diwen_send_numascii(0x1510,ID_buff,11);
		check_id_flag=1;
	}
}

/**
  * ��������: �����޸����ݴ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void Data_scan(void)
{
	u8 h,l;
	if(USART4_RX_BUF[3]==0x83)		//�����޸�
			{
				addr=((u16)USART4_RX_BUF[4]<<8)|USART4_RX_BUF[5];		//�õ���ַ
				
				switch(addr)
				{
					case 0x1728:		//�����ö����������۾���
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[0]=h;
						Tx_Buffer[1]=l;
						top_position=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("top_position=%.1f\r\n",top_position);
					break;
					case 0x1736:		//�����ö�����Ӧ����ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[2]=h;
						Tx_Buffer[3]=l;
						top_sampl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("top_sampl=%d\r\n",top_sampl);
					break;
					case 0x1744:		//�����õף��������۾���
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[4]=h;
						Tx_Buffer[5]=l;
						bottom_position=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("bottom_position=%.1f\r\n",bottom_position);
					break;
					case 0x1752:		//�����õף���Ӧ����ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[6]=h;
						Tx_Buffer[7]=l;
						bottom_smapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("bottom_smapl=%d\r\n",bottom_smapl);
					break;
					case 0x1730:		//С���������������ľ���
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[8]=h;
						Tx_Buffer[9]=l;
						near_range=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("near_range=%.1f\r\n",near_range);
					break;
					case 0x1738:		//С������������ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[10]=h;
						Tx_Buffer[11]=l;
						near_smapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("near_smapl=%d\r\n",near_smapl);
					break;
					case 0x1746:		//С����Զ���������ľ���
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[12]=h;
						Tx_Buffer[13]=l;
						far_range=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("far_range=%.1f\r\n",far_range);
					break;
					case 0x1754:		//С����Զ����ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[14]=h;
						Tx_Buffer[15]=l;
						far_smapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						write_flag=1;
						printf("far_smapl=%d\r\n",far_smapl);
					break;
					case 0x1732:		//�˿̴��ת�� 
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[16]=h;
						Tx_Buffer[17]=l;
						current_rotation=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						write_flag=1;
						printf("current_rotation=%.1f\r\n",current_rotation);
					break;
					case 0x1740:		//�˿̴��ת�����ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[18]=h;
						Tx_Buffer[19]=l;
						current_rotationsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("current_rotationsmapl=%d\r\n",current_rotationsmapl);
					break;
					case 0x1748:	//�����ת���˿̴��ת��Ƕ�
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[20]=h;
						Tx_Buffer[21]=l;
						right_rotation=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
					
						write_flag=1;
						printf("right_rotation=%.1f\r\n",right_rotation);
					break;
					case 0x1756:	//�����ת���˿̲���ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[22]=h;
						Tx_Buffer[23]=l;
						right_rotationsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("right_rotationsmapl=%d\r\n",right_rotationsmapl);
					break;
					case 0x1734:	//�չ�ʵ������
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[24]=h;
						Tx_Buffer[25]=l;
						empty_weight=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/1000;
						
						write_flag=1;
						printf("empty_weight=%.3f\r\n",empty_weight);
					break;
					case 0x1742:	//�չ�����ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[26]=h;
						Tx_Buffer[27]=l;
						empty_weightsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("empty_weightsmapl=%d\r\n",empty_weightsmapl);
					break;
					case 0x1750:	//��������ʱ����
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[28]=h;
						Tx_Buffer[29]=l;
						current_weight=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/1000;
						
						write_flag=1;
						printf("current_weight=%.3f\r\n",current_weight);
					break;
					case 0x1758:	//��������ʱ����ֵ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[30]=h;
						Tx_Buffer[31]=l;
						current_weightsmapl=USART4_RX_BUF[7]*256+USART4_RX_BUF[8];
						
						write_flag=1;
						printf("current_weightsmapl=%d\r\n",current_weightsmapl);
					break;
					case 0x1762:	//����λ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[32]=h;
						Tx_Buffer[33]=l;
						upper_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("upper_limit=%.1f\r\n",upper_limit);
					break;
					case 0x1766:	//����λ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[34]=h;
						Tx_Buffer[35]=l;
						near_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("near_limit=%.1f\r\n",near_limit);
					break;
					case 0x1768:	//Զ��λ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[36]=h;
						Tx_Buffer[37]=l;
						remote_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("remote_limit=%.1f\r\n",remote_limit);
					break;
					case 0x1772:	//��ת��λ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[38]=h;
						Tx_Buffer[39]=l;
						left_limit=(int16_t)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("left_limit=%d\r\n",left_limit);
					break;
					case 0x1774:	//��ת��λ
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[40]=h;
						Tx_Buffer[41]=l;
						right_limit=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("right_limit=%d\r\n",right_limit);
					break; 
					case 0x1776:	//����ײˮƽ���뱨��
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[42]=h;
						Tx_Buffer[43]=l;
						level_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("level_limit0=%.1f\r\n",level_limit0);
					break;
					case 0x1790:	//����ײˮƽ����Ԥ��
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[44]=h;
						Tx_Buffer[45]=l;
						level_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("level_limit1=%.1f\r\n",level_limit1);
					break;
					case 0x1778:	//����ײ��ֱ���뱨��
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[46]=h;
						Tx_Buffer[47]=l;
						vertical_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("vertical_limit0=%.1f\r\n",vertical_limit0);
					break;
					case 0x1792:	//����ײ��ֱ����Ԥ��
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[48]=h;
						Tx_Buffer[49]=l;
						vertical_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("vertical_limit1=%.1f\r\n",vertical_limit1);
					break;
					case 0x1780:	//���ر��ʱ���
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[50]=h;
						Tx_Buffer[51]=l;
						rate_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("rate_limit0=%.1f\r\n",rate_limit0);
					break;
					case 0x1794:	//���ر���Ԥ��
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[52]=h;
						Tx_Buffer[53]=l;
						rate_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("rate_limit1=%.1f\r\n",rate_limit1);
					break;
					case 0x1782:	//���ٱ���
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[54]=h;
						Tx_Buffer[55]=l;
						windspeed_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("windspeed_limit0=%.1f\r\n",windspeed_limit0);
					break;
					case 0x1796:	//����Ԥ��
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[56]=h;
						Tx_Buffer[57]=l;
						windspeed_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("windspeed_limit1=%.1f\r\n",windspeed_limit1);
					break;
					case 0x1784:	//��Ǳ���
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[58]=h;
						Tx_Buffer[59]=l;
						angle_limit0=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("angle_limit0=%.1f\r\n",angle_limit0);
					break;
					case 0x1798:	//���Ԥ��
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[60]=h;
						Tx_Buffer[61]=l;
						angle_limit1=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("angle_limit1=%.1f\r\n",angle_limit1);
					break;
					case 0x1680:	//�����߶�
						h=USART4_RX_BUF[7];
						l=USART4_RX_BUF[8];
						Tx_Buffer[62]=h;
						Tx_Buffer[63]=l;
						height=(float)(USART4_RX_BUF[7]*256+USART4_RX_BUF[8])/10;
						
						write_flag=1;
						printf("height=%.1f\r\n",height);
					break;
					default: break;	
				}
				if(write_flag==1)
				{
					Tx_Buffer[64]=0x5a;
					SPI_Flash_Write((u8*)Tx_Buffer,FLASH_SIZE-100,SIZE);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
					write_flag=0;
				}
			}
}

