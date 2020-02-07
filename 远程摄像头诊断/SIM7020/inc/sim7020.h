#ifndef __SIM7020_H
#define __SIM7020_H 			   
#include "sys.h"

extern u8 gsm_recv_flag;
extern u8 send_Msgflag;
//定义上报内容结构体
typedef struct
{
	u8 video_sta;
	u8 power_sta;
	char imeid[30];
}MQTT_Msg;

//接收来自远程配置的IP
//typedef struct
//{
//	;
//}REC_IP;


extern MQTT_Msg g_MQTT_Msg;
//extern REC_IP gREC_IP;

void sim7020_sta(void);
void sim7020_mqtt(void);
void GSM_Init(void);
int check_buff(u8 *buff, char *str);
u8* gprs_check_cmd(u8 *str);
u8 gprs_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

void strToHexStr(char *src, char *str);

void Set_ATE0(void);
u8 SIM7020_Snd_AT(void);
u8 SIM7020_Snd_CPIN(void);
u8 SIM7020_Snd_CSQ(void);
u8 SIM7020_Snd_CGREG(void);
u8 SIM7020_Snd_CGACT(void);
void SIM7020_Snd_CFUN(u8 sta);
void SIM7020_Snd_APN(void);
u8 SIM7020_Snd_GSN(void);
u8 SIM7020_Snd_CCLK(void);

void SIM7020_New_Mqtt(void);
void SIM7020_Sed_CMQCON(void);
void SIM7020_Sed_CMQSUB(void);
void SIM7020_Sed_CMQPUB(void);
void SIM7020_Sed_CMQDISCON(void);
void SIM7020_Sed_CMQUNSUB(void);

void Mqtt_Msg_load(void);
void itoi(char *str, unsigned long num);
char itoc(unsigned char ichar);

#endif

