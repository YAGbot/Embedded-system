#ifndef __SIM7020_H
#define __SIM7020_H 			   
#include "sys.h"

extern u8 gsm_recv_flag;
//�����ϱ����ݽṹ��
typedef struct
{
	u8 video_sta;
	u8 power_sta;
	char ID[15];
}MQTT_Msg;

////��������Զ�����õ�IP
//typedef struct
//{
//;
//}REC_IP;

extern int Loss_Rate;
extern MQTT_Msg g_MQTT_Msg;
//extern REC_IP gREC_IP;

void sim7020_sta(void);
void sim7020_mqtt(void);
void GSM_Init(void);
int check_buff(u8 *buff, char *str);
u8* gprs_check_cmd(u8 *str);
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

void strToHexStr(char *src, char *str);

/*���״̬*/
u8 Check_STATUS(void);
u8 Set_ATE0(void);
u8 SIM7020_Snd_AT(void);
u8 SIM7020_Snd_CPIN(void);
u8 SIM7020_Snd_CSQ(void);
u8 SIM7020_Snd_CGREG(void);
u8 SIM7020_Snd_CGACT(void);
u8 SIM7020_Snd_APN(void);
void SIM7020_Snd_CFUN(void);
u8 SIM7020_Snd_GSN(void);

/*����mqttҵ��*/
void SIM7020_Sed_CLIENTID(void);
void SIM7020_IPPORT(void);
void SIM7020_USERPWD(void);
void SIM7020_Snd_ATW(void);
void SIM7020_Snd_CFUN(void);
/*��������*/
void SIM7020_PUBLISH(void);
void SIM7020_SUBSCRIBE(void);

#endif

