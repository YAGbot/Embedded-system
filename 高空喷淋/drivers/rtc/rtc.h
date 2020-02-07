#ifndef _RTC_H
#define _RTC_H



#include "stm32f10x.h"


//ʱ��ṹ��
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//������������
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;	
	vu8  flag;
}_calendar_obj;			


extern _calendar_obj calendar;	//�����ṹ��

extern u8 const mon_table[12];	//�·��������ݱ�
void disp_time(u8 x,u8 y,u8 size);//���ƶ�λ�ÿ�ʼ��ʾʱ��
void disp_week(u8 x,u8 y,u8 size,u8 lang);//��ָ��λ����ʾ����
u8 rtc_init(void);        //��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 is_leap_year(u16 year);//ƽ��,�����ж�
u8 rtc_alarm_set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 rtc_get(u32 timecount, _calendar_obj * pcalendar);
u8 rtc_get_week(u16 year,u8 month,u8 day);
u8 rtc_set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//����ʱ��			 
u8 rtc_xget(u32 timecount,  _calendar_obj * pcalendar);



#endif

