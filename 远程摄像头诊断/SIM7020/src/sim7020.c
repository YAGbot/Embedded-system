#include "sim7020.h"
#include "sim7020_usart.h"
#include "sim7020_timer.h"
#include "string.h" 
#include "uart.h"
#include "delay.h"
#include "stdlib.h"
#include <stdio.h>
#include "ping.h"

/*************	���س�������	**************/
u8 gsm_recv_flag = 0; //gu900��������״̬λ
u8 send_Msgflag =0;		//���ݷ���״̬λ

char ID[15];	
char rat_buf[10];
int rat_len=0;

MQTT_Msg g_MQTT_Msg;
const char *string = "AT+CMQNEW=\"61.181.255.77\",\"61613\",12000,200\r\n";		//����MQTT����
const char *apnstr = "AT*MCGDEFCONT=\"IP\",\"3GNET\"\r\n";		//����APN
char response = 0;

/************************************************************************************
* �������ƣ�GSM_Init
* ����������gsm��ʼ������
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void GSM_Init(void)
{
	sim7020_usart_init(115200);
	TIM7_Init();
}

/************************************************************************************
* �������ƣ�check_buff
* �����������������ݼ�麯��
* ��ڲ�����buff��
* ���ڲ�����0���ɹ� ��-1�����Ӵ�
*************************************************************************************/
int check_buff(u8 *buff, char *str)
{
    //�ַ�������
  if(strstr((const char *)buff,str)!=NULL)
    return 0;
  else
    return -1;
}

/************************************************************************************
* �������ƣ�gprs_check_cmd
* �����������ַ�����������
* ��ڲ�����
* ���ڲ�����
*************************************************************************************/
u8* gprs_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART1_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART1_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}


//��gprs��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART1_RX_STA=0;USART2_RX_REC_ATCOMMAD=1;
	if((u32)cmd<=0XFF)
	{  
		USART1->DR=(u32)cmd;
	}else gsm_printf("%s\r\n",cmd);//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(11);			//������ʱ������ӽ���ȥ�жϣ�
			if(USART1_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(gprs_check_cmd(ack))break;//�õ���Ч���� 
				USART1_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	USART1_RX_STA=0;USART2_RX_REC_ATCOMMAD=0;
	return res;
} 


void strToHexStr(char *src, char *str) 
{
	char *p=src;
	int j=0;
	while(*p!=0)
	{ 
		j+=sprintf(str+j,"%x",*p++);
	}
}

/************************************************************************************
* �������ƣ�sim7020_sta
* ����������sim7020״̬ȷ�ϣ�����ȡӲ����Ϣ
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void sim7020_sta(void)
{
	char buf[15]={0};
	u8 i;
	SIM7020_Snd_AT();					//���ģ���ܷ�ͨ��
	Set_ATE0();
	SIM7020_Snd_CPIN();				//��鿨״̬
	SIM7020_Snd_CSQ();				//��ȡ����״��
	SIM7020_Snd_CGREG();			//�鿴���總��״̬
	SIM7020_Snd_CGACT();			//�鿴�Ƿ񼤻�����
	SIM7020_Snd_GSN();				//��ȡIMEI
	for(i=0; i<15; i++)
	{
		//buf[i] = gsm_uart_buff[i+9];		//868334030061703
		buf[i] = USART1_RX_BUF[i+2];		//868334030061703		//������
		ID[i] = buf[i];
	}
	//printf("%s",buf);
	strToHexStr(buf,g_MQTT_Msg.imeid);
	//printf("imeid=%s\r\n",g_MQTT_Msg.imeid);
}

/************************************************************************************
* �������ƣ�sim7020_mqtt
* ����������sim7020����mqttҵ��
* ��ڲ�������
* ���ڲ�������
*************************************************************************************/
void sim7020_mqtt(void)
{
	printf("�ر�mqtt����\r\n");
	SIM7020_Sed_CMQDISCON();//�ر�mqtt����
	delay_ms(1000);
	
	printf("����������\r\n");	
	SIM7020_New_Mqtt();			//����������
	delay_ms(1000);
	
	printf("��������\r\n");
	SIM7020_Sed_CMQCON();		//��������
	delay_ms(1000);
	
	printf("����\r\n");
	SIM7020_Sed_CMQSUB();		//����
	delay_ms(1000);
}


/*******************************************************************************
* ������ : Set_ATE0
* ����   : ȡ������
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void Set_ATE0(void)
{
	gprs_send_cmd((u8*)"ATE0\r\n",(u8*)"OK",100);								//ȡ������		
}

/*******************************************************************
* ������ : Mcu_sim7020_AT_OK
* ����   : ����AT������OK������uart5���ں�BC95ģ��֮��ͨ���Ƿ�����
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
u8 SIM7020_Snd_AT(void)
{
	u8 try_num = 10;
	int res;
  do
  {
		res=gprs_send_cmd((u8*)"AT\r\n",(u8*)"OK",100);
    if(res==0)			//��⵽OK����NBIOTģ���Ѿ��������������Է�������ATָ��
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* ������ : SIM7020_Snd_CPIN
* ����   : ����AT+CPIN?������+CPIN:READY������SIM���Ƿ���λ
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
u8 SIM7020_Snd_CPIN(void)
{
	u8 try_num = 10;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CPIN?\r\n",(u8*)"+CPIN: READY",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* ������ : SIM7020_Snd_CPIN
* ����   : ����AT+CSQ�����ز�Ϊ+CSQ: 99, 99�����Ի�ȡ������
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
u8 SIM7020_Snd_CSQ(void)
{
	u8 try_num = 60;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CSQ\r\n",(u8*)"+CSQ: 0,0",100)!=0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(2000);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* ������ : SIM7020_Snd_CGREG
* ����   : ����AT+CGREG?������+CGREG: 0,1�����PS���總��
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
u8 SIM7020_Snd_CGREG(void)
{
	u8 try_num = 20;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CGREG?\r\n",(u8*)"+CGREG: 0,1",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(2000);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* ������ : SIM7020_Snd_CGACT
* ����   : ����AT+CGACT?������Ϊ+CGACT: 1,1������PDN ����ɹ�
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
u8 SIM7020_Snd_CGACT(void)
{
	u8 try_num = 20;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CGACT?\r\n",(u8*)"+CGACT: 1,1",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* ������ : SIM7020_Snd_CFUN
* ����   : ����AT+CFUN����Ƶ����
* ����   : sta
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
void SIM7020_Snd_CFUN(u8 sta)
{
	if(sta)
	{
		gprs_send_cmd((u8*)"AT+CFUN=1",(u8*)"+CPIN: READY",100);
	}
	else
		gprs_send_cmd((u8*)"AT+CFUN=0",(u8*)"+CPIN: NOT READY",100);
}

/*******************************************************************
* ������ : SIM7020_Snd_APN
* ����   : ����AT*MCGDEFCONT������ΪOK���ֶ�����APN
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
void SIM7020_Snd_APN(void)
{
	gprs_send_cmd((u8*)apnstr,(u8*)"OK",100);
}

/*******************************************************************
* ������ : SIM7020_Snd_GSN
* ����   : ����AT+GSN������OK����ȡimei
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
u8 SIM7020_Snd_GSN(void)
{
	u8 try_num = 10;
  do
  {
    if(gprs_send_cmd((u8*)"AT+GSN\r\n",(u8*)"OK",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* ������ : SIM7020_Snd_CCLK
* ����   : ����AT+CCLK?������OK����ȡ����ʱ��
* ����   : ��
* ���   :
* ����   : 0 : �ɹ�   ���� ��ʧ��
* ע��   : ����AT������OK
********************************************************************/
u8 SIM7020_Snd_CCLK(void)
{
	u8 try_num = 10;
  do
  {
    if(gprs_send_cmd((u8*)"AT+CCLK?\r\n",(u8*)"OK",100)==0)	
    {
			delay_ms(1000);
    	break;
    }
		delay_ms(800);
  }while(--try_num);
  return try_num;
}

/*******************************************************************
* ������ : SIM7020_New_Mqtt
* ����   : SIM7020ʵ��mqttҵ��
* ����   : ��
* ���   :
* ����   :
* ע��   : ����AT������OK
********************************************************************/
void SIM7020_New_Mqtt(void)
{
	gprs_send_cmd((u8*)string,(u8*)"OK",500);
}

/*******************************************************************
* ������ : SIM7020_Sed_CMQCON
* ����   : SIM7020����mqtt����
* ����   : ��
* ���   :
* ����   : 
* ע��   : ����AT������OK
********************************************************************/
void SIM7020_Sed_CMQCON(void)
{
	//printf("AT+CMQCON=0,3,\"%s\",600,0,0,\"thhy\",\"thhy550550\"\r\n",ID);
	//gsm_printf("AT+CMQCON=0,3,\"%s\",60,0,0,\"thhy\",\"thhy550550\"\r\n",ID);
	char rqbuf[100];
	sprintf(rqbuf,"AT+CMQCON=0,3,\"%s\",600,0,0,\"thhy\",\"thhy550550\"\r\n",ID);
	gprs_send_cmd((u8*)rqbuf,(u8*)"OK",200);
}	


/*******************************************************************
* ������ : SIM7020_Sed_CMQCON
* ����   : SIM7020����mqtt����
* ����   : ��
* ���   :
* ����   : 
* ע��   : ����AT������OK	
********************************************************************/
void SIM7020_Sed_CMQPUB(void)
{
	u8 num = 3;
	int ret;
	char buf[150];
	char msg_buf[150];
	char send_buf[200];
	send_Msgflag = 1;
	
	sprintf(buf,"{\"ID_IMEI\":\"%s\",\"video\":\"%d\",\"ele\":\"%d\",\"net\":\"%d\"}",ID,g_MQTT_Msg.video_sta,g_MQTT_Msg.power_sta,Net_Info.Loss_Rate);
	strToHexStr(buf,msg_buf);
	sprintf(send_buf,"AT+CMQPUB=0,\"Topic\",1,0,0,%d,\"%s\"\r\n",strlen(msg_buf),msg_buf);	
	
	while(num)
	{
		--num;
		ret=gprs_send_cmd((u8*)send_buf,(u8*)"OK",1000);
		if(ret==0)
		{
			printf("���ͽ���\r\n");
			error_count=0;	//���¼���
			break;
		}		
		delay_ms(2000);
	}
	send_Msgflag = 0;
}




/*******************************************************************************
* ������ : itoi
* ����   : ����ת��Ϊ�ַ���
* ����   :
* ���   :
* ����   :
* ע��   :
*******************************************************************************/
void itoi(char *str, unsigned long num)
{
	char index[]="0123456789";
	char i = 0, j = 0;
    char temp;

    do
    {
        str[i++] = index[num%10];
        num /= 10;
    }while(num);

    str[i]='\0';

    for(j=0; j<=(i-1)/2; j++)
    {
        temp=str[j];
        str[j]=str[i-j-1];
        str[i-j-1]=temp;
    }
}

/*******************************************************************************
* ������ : itoc
* ����   : ��������ת��Ϊ�ַ�
* ����   : ichar����
* ���   :
* ����   : �ַ�
* ע��   :
*******************************************************************************/
char itoc(unsigned char ichar)
{
	char index[]="0123456789";

    return index[ichar%10];
}



void SIM7020_Sed_CMQSUB(void)
{
	gprs_send_cmd((u8*)"AT+CMQSUB=0,\"myTopic\",1\r\n",(u8*)"OK",1000);
}

void SIM7020_Sed_CMQDISCON(void)
{
	gprs_send_cmd((u8*)"AT+CMQDISCON=0\r\n",(u8*)"OK",1000);
}


void SIM7020_Sed_CMQUNSUB(void)
{
	gprs_send_cmd((u8*)"AT+CMQUNSUB=0,\"myTopic\"\r\n",(u8*)"OK",1000);
}





