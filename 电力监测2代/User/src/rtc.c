/**
  **************************************************************************************
  * @File      rtc.c
  * @Author    ChuanYong Wan
  * @brief     RTC Clock drive function and interface function.
  * @Version   1.0
	* @SDcard    High Capacity SD Memory Card 
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *				     Without permission, shall not be reproduced and copied, shall not be 
  *			       used for commercial purposes.
  **************************************************************************************
 **/
/* Include ----------------------------------------------------------------------------*/
#include "rtc.h" 	
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
								 
static const uint8_t Week_Table[12]   = {0,3,3,6,1,4,6,2,5,0,3,5};  
static const uint8_t Month_Table[12]  = {31,28,31,30,31,30,31,31,30,31,30,31};
static Calendar_TypeDef Calendar;
/* Function ---------------------------------------------------------------------------*/
/**
  * @brief  Judge whether it is a leap year 
	* @param  year: Year
	* @retval 0: Not a leap year
	*  				1: Is a leap year
 **/
static uint8_t Is_Leap_Year(uint16_t year)
{			  
	if(year%4==0) 
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0) return 1;   
			else return 0;   
		}
		else return 1;   
	}
	else return 0;	
}	 			   
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Set RTC Clock
	* @param  year:  Year(1970-2099)
	* @param  month: Month
	* @param  day:   Day
	* @param  hour:  Hour
	* @param  min:   Minute
	* @param  sec:   Second
	* @retval 0: Succeed
	*  				1: Failed
 **/
static uint8_t RTC_Set(uint16_t year, uint8_t month, uint8_t day, \
											 uint8_t hour, uint8_t min, uint8_t sec)
{
	uint16_t i = 0;
	uint32_t Seconds = 0;
	if((year<1970)||(year>2099)) return 1;	   
	for(i=1970;i<year;i++)	
	{
		if(Is_Leap_Year(i))
		{
			Seconds += 31622400;
		}
		else Seconds += 31536000;			 
	}
	month -= 1;
	for(i=0;i<month;i++)	   
	{
		Seconds += (uint32_t)Month_Table[i]*86400;
		if(Is_Leap_Year(year)&&(i==1))
		{
			Seconds += 86400;   
		}
	}
	Seconds += (uint32_t)(day-1)*86400;
	Seconds += (uint32_t)hour*3600;
  Seconds += (uint32_t)min*60;	
	Seconds += sec;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	
	PWR_BackupAccessCmd(ENABLE);	
	RTC_SetCounter(Seconds);	
	RTC_WaitForLastTask();	 	
	return 0;	    
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  RTC Clock 
	* @param  None
	* @retval 0: Succeed
	*  				1: Failed
 **/
uint8_t RTC_Init(void)
{
	uint8_t Repeat = 0;
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x5050)//Check whether it is the first time
	{	 			
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP,ENABLE);	
		PWR_BackupAccessCmd(ENABLE); 
		BKP_DeInit();
//		RCC_LSEConfig(RCC_LSE_ON);
		RCC_LSICmd(ENABLE);
		
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET)	//Wait for LSE to be ready
		{
			Repeat++;
			delay_ms(10);
		}
		if(Repeat>=250) return 1;//Failed to initialize LSE   
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);		  
		RCC_RTCCLKCmd(ENABLE);	 
		
		RTC_WaitForLastTask();	
		RTC_WaitForSynchro();		 
		
		RTC_WaitForLastTask();	
		RTC_SetPrescaler(40000);
		
		RTC_WaitForLastTask();
		RTC_Set(2018,12,31,23,59,01);  
		
		BKP_WriteBackupRegister(BKP_DR1, 0X5049);	
	}
	else
	{
		RTC_WaitForSynchro();	
		RTC_WaitForLastTask();	
	}	    				     
	return 0; 
}		 
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Get week
	* @param  year:  Year(1970-2099)
	* @param  month: Month
	* @param  day:   Day
	* @retval Week
 **/
uint8_t Get_Week(uint16_t year, uint8_t month, uint8_t day)
{	
	uint16_t Temp = 0;
	uint8_t YearH = 0, YearL = 0;
	
	YearH = year/100;	YearL = year%100;  
	if (YearH>19)
	{
		YearL+=100;
	}

	Temp = YearL+YearL/4;
	Temp = Temp%7; 
	Temp = Temp+day+Week_Table[month-1];
	
	if((YearL%4==00) && (month<3))
	{
		Temp--;
	}
	return(Temp%7);
}			  
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Get time
	* @param  None
	* @retval 0: Succeed
	*  				Others: Error code
 **/
uint8_t Get_Time(Calendar_TypeDef *pCalendar)
{
	uint16_t Year = 0;
	static uint16_t Temp = 0;
  uint32_t Days=0, Seconds=0; 
	 
  Seconds = RTC_GetCounter();	 
 	Days = Seconds/86400;   
	if(Temp!=Days)
	{	  
		Temp = Days;
		Year = 1970;
		while(Days>=365)
		{				 
			if(Is_Leap_Year(Year))
			{
				if(Days>=366)
				{
					Days -= 366;
				}
				else 
				{
					Year++;
					break;
				}  
			}
			else 
			{
				Days -= 365;
			}				
			Year++;  
		}   
		pCalendar->w_year = Year;
		Year = 0;
		while(Days>=28)
		{
			if(Is_Leap_Year(pCalendar->w_year)&&Year==1)
			{
				if(Days>=29)
				{
					Days-=29;
				}
				else break; 
			}
			else 
			{
				if(Days>=Month_Table[Year])
				{
					Days -= Month_Table[Year];
				}
				else break;
			}
			Year++;  
		}
		pCalendar->w_month = Year+1;
		pCalendar->w_date  = Days+1;  	
	}
	Days = Seconds%86400;     		 	   
	pCalendar->hour = Days/3600;     	
	pCalendar->min  = (Days%3600)/60; 	
	pCalendar->sec  = (Days%3600)%60; 	
	pCalendar->week = Get_Week(pCalendar->w_year, pCalendar->w_month, pCalendar->w_date);  

	return 0;
}	 
/*-------------------------------------------------------------------------------------*/						
void Read_TimeStr(char *time)
{
	Get_Time(&Calendar);
	sprintf(time, "%d/%d/%d-%d:%d", Calendar.w_year, Calendar.w_month, Calendar.w_date, \
																	Calendar.hour, Calendar.min);
}
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/


