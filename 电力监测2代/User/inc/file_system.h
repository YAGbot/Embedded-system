/**
  **************************************************************************************
  * @File      file_system.c
  * @Author    ChuanYong Wan
  * @brief     Header files of file_system.c.
  * @Version   2.0
	* @FatFs     R0.10a 
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *				     Without permission, shall not be reproduced and copied, shall not be 
  *			       used for commercial purposes.
  **************************************************************************************
 **/
#ifndef __FILE_SYSTEM_H
#define __FILE_SYSTEM_H 			   
/* Include ----------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdint.h"
#include "ff.h"

/* Typedef ----------------------------------------------------------------------------*/
typedef struct 
{
	uint8_t  hour[3];
	uint8_t  min[3];
	uint8_t  sec[3];			
	uint16_t year[5];
	uint8_t  month[3];
	uint8_t  day[3];
	uint8_t  week[2];		 
}Time_TypeDef;	

/* Define -----------------------------------------------------------------------------*/
#define FILE_OK              1
#define FILE_ERROR           0
#define FILEERR_NEW          (FILE_ERROR-1)
#define FILEERR_WRITE        (FILE_ERROR-2)
#define FILEERR_READ         (FILE_ERROR-3)
#define FILEERR_OPEN         (FILE_ERROR-4)
#define FILEERR_CLOSE        (FILE_ERROR-5)
#define FILEERR_DELETE       (FILE_ERROR-6)
#define FILEERR_EXISTED      (FILE_ERROR-7)
#define FILEERR_OVERFLOW     (FILE_ERROR-8)
#define TXT_FILESIZE         (1000*1000*1000)
//#define DATA_MANAGE_WITHTIME 1			//以时间管理
#define DATA_MANAGE_WITHTIME 0		//以3个文件存储

/* Function prototype -----------------------------------------------------------------*/
uint8_t Capacity_Read(unsigned long *total_sectors, unsigned long *remain_sectors);	
int8_t FileSystem_Mount(char *lg_dri_num);
int8_t FileSystem_FileCreate(void);
int8_t File_Write(char *name, FIL *file, char *str);
int8_t File_Read(char *name, FIL *file, char *buf);
int8_t File_Delete(char *name);

int8_t BackupFile_New(void);
int8_t BackupFile_Write(char *str);
int8_t BackupFile_Read(char *buf);
int8_t BackupFile_Delete(void);

int8_t ConfigFile_New(void);
int8_t ConfigFile_Write(char *str);
int8_t ConfigFile_Read(char *buf);
int8_t ConfigFile_Delete(void);

#if DATA_MANAGE_WITHTIME
int8_t DataStr_WriteWithTime(char *str);
#else
int8_t DataStr_Write(char *str);
#endif

#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

