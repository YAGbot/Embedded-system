#ifndef _RTC_H
#define _RTC_H



#include "stm32f10x.h"


//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;	
	vu8  flag;
}_calendar_obj;			


extern _calendar_obj calendar;	//日历结构体

extern u8 const mon_table[12];	//月份日期数据表
void disp_time(u8 x,u8 y,u8 size);//在制定位置开始显示时间
void disp_week(u8 x,u8 y,u8 size,u8 lang);//在指定位置显示星期
u8 rtc_init(void);        //初始化RTC,返回0,失败;1,成功;
u8 is_leap_year(u16 year);//平年,闰年判断
u8 rtc_alarm_set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 rtc_get(u32 timecount, _calendar_obj * pcalendar);
u8 rtc_get_week(u16 year,u8 month,u8 day);
u8 rtc_set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间			 
u8 rtc_xget(u32 timecount,  _calendar_obj * pcalendar);



#endif

