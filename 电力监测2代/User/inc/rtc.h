/**
  **************************************************************************************
  * @File      rtc.h
  * @Author    ChuanYong Wan
  * @brief   	 Header files of RTC Clock drive functions.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *								 Without permission, shall not be reproduced and copied, shall not be 
	*							used for commercial purposes.
  **************************************************************************************
 **/
#ifndef __RTC_H
#define __RTC_H	 
/* Private typedef --------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------*/
/* Includes ---------------------------------------------------------------------------*/
#include "sys.h"
/* Private define ---------------------------------------------------------------------*/ 
/* Private variables ------------------------------------------------------------------*/
typedef struct 
{
	vu8  hour;
	vu8  min;
	vu8  sec;			
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}Calendar_TypeDef;	

/* Private functions ------------------------------------------------------------------*/
uint8_t RTC_Init(void);   
uint8_t Get_Week(uint16_t year, uint8_t month, uint8_t day);
uint8_t Get_Time(Calendar_TypeDef *pCalendar);
void Read_TimeStr(char *time);
#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/


