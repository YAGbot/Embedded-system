/**
  **************************************************************************************
  * @File      file_system.c
  * @Author    ChuanYong Wan
  * @brief     File system interface function.
  * @Version   2.0
	* @FatFs     R0.10a 
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *				     Without permission, shall not be reproduced and copied, shall not be 
  *			       used for commercial purposes.
  **************************************************************************************
 **/
/* Include ----------------------------------------------------------------------------*/
#include "uart.h"
#include "delay.h"
#include "cJSON.h"
#include "stdlib.h"
#include "string.h"
#include "malloc.h"
#include "file_system.h"

/* Variable ---------------------------------------------------------------------------*/												 
static FIL *gFileConfig=NULL, *gFileBackup=NULL;	
static FATFS *gFile_System=NULL;	
#if DATA_MANAGE_WITHTIME
FIL *gFileX=NULL;
#else
static uint8_t gNum  = 0;
static FIL *gFile1=NULL, *gFile2=NULL, *gFile3=NULL;
static char *File1_Name  = "0:1.TXT";
static char *File2_Name  = "0:2.TXT";
static char *File3_Name  = "0:3.TXT";
#endif
static char *Backup_Name = "0:backup.TXT";
static char *Config_Name = "0:config.TXT";
static Time_TypeDef Time;

/* Function ---------------------------------------------------------------------------*/
int8_t FileSystem_Mount(char *lg_dri_num)
{
	uint8_t ret=0;
	gFile_System = (FATFS*)My_mem_malloc(sizeof(FATFS));
	gFileConfig  = (FIL*)My_mem_malloc(sizeof(FIL));
	gFileBackup  = (FIL*)My_mem_malloc(sizeof(FIL));
	#if DATA_MANAGE_WITHTIME
	gFileX       = (FIL*)My_mem_malloc(sizeof(FIL));
	#else
		gFile1     = (FIL*)My_mem_malloc(sizeof(FIL));
		gFile2     = (FIL*)My_mem_malloc(sizeof(FIL));
		gFile3     = (FIL*)My_mem_malloc(sizeof(FIL));
	#endif
	if(gFile_System&&gFileConfig&&gFileBackup)
	{
		ret = f_mount(gFile_System, lg_dri_num, 0);
		if(ret!=FR_OK)
		{				
			#if DATA_MANAGE_WITHTIME
				My_mem_free(gFileX);
			#else
				My_mem_free(gFile1);	
				My_mem_free(gFile2);	
				My_mem_free(gFile3);
			#endif			
			My_mem_free(gFileBackup);
			My_mem_free(gFileConfig);
			My_mem_free(gFile_System);
		}
		return ret;
	}
	else 
	{
		#if DATA_MANAGE_WITHTIME
			My_mem_free(gFileX);
		#else
			My_mem_free(gFile1);	
			My_mem_free(gFile2);	
			My_mem_free(gFile3);
		#endif	
		My_mem_free(gFileBackup);
		My_mem_free(gFileConfig);
		My_mem_free(gFile_System);	
	}
	return -1;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Read the total capacity and remaining capacity of the disk
	* @param  *total_sectors:  Pointer of total capacity
	* @param  *remain_sectors: Pointer of remain capacity
	* @retval FR_OK
	*					Error code
 **/
uint8_t Capacity_Read(unsigned long *total_sectors, unsigned long *remain_sectors)
{
	uint8_t ret;
	DWORD Remain_Clust;
	
  ret = f_getfree("0:", &Remain_Clust, &gFile_System);
  if(ret==FR_OK)
	{					
	  *total_sectors  = (gFile_System->n_fatent-2)*gFile_System->csize;	
	  *remain_sectors = Remain_Clust*gFile_System->csize; 	
 	}
	return ret;
}		   
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  New file
	* @param  *name: Pointer of file name
	* @param  *file: Pointer of file struct
	* @retval FILE_OK
	*					Error code
 **/
int8_t File_New(char *name, FIL *file)
{
	FILINFO *File_Info = NULL;
	uint8_t ret = 0;
	
	if(file==NULL) return FILEERR_NEW;
	if(f_stat(name, File_Info)==FR_OK) return FILEERR_EXISTED;
	else
	{
		ret = f_open(file, name, FA_CREATE_NEW);
		f_close(file);
		if(ret==FR_OK) return FILE_OK;
	}
	return FILEERR_NEW;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Write file
	* @param  *name: Pointer of file name
	* @param  *file: Pointer of file struct
	* @param  *str:  Pointer of file's content
	* @retval FR_OK
	*					Error code
 **/
int8_t File_Write(char *name, FIL *file, char *str)
{
	uint32_t Max_Size  = 1000*1000*1000;
	uint32_t File_Size = 0;
	uint8_t ret1 = 0;
	int     ret2 = 0;
	
	if(file==NULL) return FILEERR_WRITE;
	File_Size = f_size(file);
	if(File_Size<Max_Size)
	{
		ret1 = f_open(file, name, FA_OPEN_EXISTING|FA_WRITE);
		if(ret1==FR_OK)
		{
			ret1 = f_lseek(file, File_Size);
			if(ret1==FR_OK)
			{
				ret2 = f_puts(str, file);
				f_close(file);
				if(ret2<0) return FILEERR_WRITE;
				else       return FILE_OK;
			}
			f_close(file);
			return FILEERR_WRITE;
		}
		return FILEERR_OPEN;
	}
	return FILEERR_OVERFLOW;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Read file
	* @param  *name: Pointer of file name
	* @param  *file: Pointer of file struct
	* @param  *buf:  Pointer of storage buffer
	* @retval FR_OK
	*					Error code
 **/
int8_t File_Read(char *name, FIL *file, char *buf) 
{
	uint32_t File_Size = f_size(file);
	uint32_t Len = 0;
	uint8_t ret;
	
	if(file==NULL) return FILEERR_READ;
	ret = f_open(file, name, FA_OPEN_EXISTING|FA_READ);
	if(ret==FR_OK) 
	{		
		ret = f_read(file, buf, File_Size, &Len);
		f_close(file);
		if(ret==FR_OK) return FILE_OK;
		else return FILEERR_READ;
	}
	return FILEERR_OPEN;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Delete file
	* @param  *name: Pointer of file name
	* @param  *file: Pointer of file struct
	* @retval FILE_OK
	*					Error code
 **/
int8_t File_Delete(char *name)
{
	uint8_t ret = 0;
	
	ret = f_unlink(name);
	if(ret==FR_OK) return FILE_OK;
	else return FILEERR_DELETE;
}
/*-------------------------------------------------------------------------------------*/
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
int8_t FileSystem_FileCreate(void)
{
	uint8_t ret = 0;
	ret = File_New(Backup_Name, gFileBackup);
	ret = File_New(Config_Name, gFileConfig);
	#if DATA_MANAGE_WITHTIME
	#else
	ret = File_New(File1_Name,  gFile1);
	ret = File_New(File2_Name,  gFile2);
	ret = File_New(File3_Name,  gFile3);
	#endif
	return ret;
}
/*-------------------------------------------------------------------------------------*/
#if DATA_MANAGE_WITHTIME
static void Get_Time(char *str, Time_TypeDef *time)
{
	char TimeStr[30] = {0};
	char *p=NULL, *pNext=NULL;
	cJSON *root = cJSON_Parse(str); 
	if(root!=NULL) sprintf(TimeStr, "%s", cJSON_GetObjectItem(root, "Time")->valuestring);
	cJSON_Delete(root);
	
	p = TimeStr;
	pNext = strstr(p, "/");
	if(pNext!=NULL) 
	{
		memcpy(time->year, p, pNext-p);
		p = pNext;
		p += 1;
	}
	
	pNext = strstr(p, "/");
	if(pNext!=NULL) 
	{
		memcpy(time->month, p, pNext-p);
		p = pNext;
		p += 1;
	}
	
	pNext = strstr(p, "-");
	if(pNext!=NULL) 
	{
		memcpy(time->day, p, pNext-p);
		p = pNext;
		p += 1; 
	}
}
/*-------------------------------------------------------------------------------------*/
int8_t DataStr_WriteWithTime(char *str)
{
	char  LastYear_Dir[10]={0}, Year_Dir[10]={0}, Month_Dir[15]={0}, Day_File[20]={0};
	int   Year=0, Month=0, Day=0, ret2=0;
	uint8_t ret1 = 0;
	FILINFO *File_Info = NULL;
	
	memset(Time.year, 0, sizeof(Time.year));
	memset(Time.month, 0, sizeof(Time.month));
	memset(Time.day, 0, sizeof(Time.day));
	memset(Time.hour, 0, sizeof(Time.hour));
	memset(Time.min, 0, sizeof(Time.min));
	memset(Time.sec, 0, sizeof(Time.sec));
	Get_Time(str, &Time);
	Year  = atoi((char *)Time.year);
	sprintf(LastYear_Dir, "0:%d",   Year-1);
	if(f_stat(LastYear_Dir, File_Info)==FR_OK) 
	{
		Month = atoi((char *)Time.month);
		Day   = atoi((char *)Time.day);
		sprintf(Day_File, "0:%d/%s/%s.TXT", Year-1, (char *)Time.month, (char *)Time.day);
		f_unlink(Day_File);
		if((Month==2)&&(Day==28)&&(Is_Leap_Year(Year-1)))
		{
			sprintf(Day_File, "0:%d/%s/%d.TXT", Year-1, (char *)Time.month, Day+1);
			f_unlink(Day_File);
		}
		if(Month>1) 
		{
			sprintf(Month_Dir, "0:%d/%d", Year-1, Month-1);
			if(f_stat(Month_Dir, File_Info)==FR_OK) f_unlink(Month_Dir);
		}
		if((Month==12)&&(Day==31))
		{
			sprintf(Month_Dir, "0:%d/%d", Year-1, Month);
			f_unlink(Month_Dir);
			f_unlink(LastYear_Dir);
		}
		
	}
	memset(Month_Dir, 0, sizeof(Month_Dir));
	memset(Day_File, 0, sizeof(Day_File));

	sprintf(Year_Dir,  "0:%s",           (char *)Time.year);
	sprintf(Month_Dir, "0:%s/%s",        (char *)Time.year, (char *)Time.month);
	sprintf(Day_File,  "0:%s/%s/%s.TXT", (char *)Time.year, (char *)Time.month, \
																			 (char *)Time.day);
	
	if(f_stat(Year_Dir,  File_Info)!=FR_OK) f_mkdir(Year_Dir);
	if(f_stat(Month_Dir, File_Info)!=FR_OK) f_mkdir(Month_Dir);

	if(f_stat(Day_File, File_Info)==FR_OK) 
	{
		ret2 = File_Write(Day_File, gFileX, str);
		if(ret2!=FILE_OK) return FILEERR_WRITE;
		else              return FILE_OK;
	}
	else
	{
		ret1 = f_open(gFileX, Day_File, FA_CREATE_NEW);
		f_close(gFileX);
		delay_ms(100);
		if(ret1==FR_OK) 
		{
			ret1 = f_open(gFileX, Day_File, FA_OPEN_EXISTING|FA_WRITE);
			if(ret1==FR_OK)
			{
				ret2 = f_puts(str, gFileX);
				f_close(gFileX);
				if(ret2<0) return FILEERR_WRITE;
				else       return FILE_OK;
			}
		}
	}
	return FILE_ERROR;
}
/*-------------------------------------------------------------------------------------*/
#else
int8_t DataStr_Write(char *str)
{
	int8_t ret = 0;
	uint32_t Max_Size   = 1000*1000*1000;
	uint32_t File1_Size = f_size(gFile1);
	uint32_t File2_Size = f_size(gFile2);
	uint32_t File3_Size = f_size(gFile3);
	
	if(File1_Size<Max_Size)
	{
		gNum = 1;
		ret = File_Write(File1_Name, gFile1, str);
		return ret;
	}
	else if(File2_Size<Max_Size)
	{
		gNum = 2;
		ret = File_Write(File2_Name, gFile2, str);
		return ret;
	}
	else if(File3_Size<Max_Size)
	{
		gNum = 3;
		ret = File_Write(File3_Name, gFile3, str);
		return ret;
	}

	if((gNum==3)||(gNum==0))
	{
		File_Delete(File1_Name);
		delay_ms(1000);
		File_New(File1_Name, gFile1);
		delay_ms(1000);
		gNum = 1;
		ret = File_Write(File1_Name, gFile1, str);
		return ret;
	}
	else if(gNum==1)
	{
		File_Delete(File2_Name);
		delay_ms(1000);
		File_New(File2_Name, gFile2);
		delay_ms(1000);
		gNum = 2;
		ret = File_Write(File2_Name, gFile2, str);
		return ret;
		
	}
	else if(gNum==2)
	{
		File_Delete(File3_Name);
		delay_ms(1000);
		File_New(File3_Name, gFile3);
		delay_ms(1000);
		gNum = 3;
		ret = File_Write(File3_Name, gFile3, str);
		return ret;
	}
	return FILEERR_WRITE;
}
#endif
/*-------------------------------------------------------------------------------------*/
int8_t BackupFile_New(void)
{
	return File_New(Backup_Name, gFileBackup);
}
/*-------------------------------------------------------------------------------------*/
int8_t BackupFile_Write(char *str)
{
	return File_Write(Backup_Name, gFileBackup, str);
}
/*-------------------------------------------------------------------------------------*/
int8_t BackupFile_Read(char *buf)
{
	return File_Read(Backup_Name, gFileBackup, buf);
}
/*-------------------------------------------------------------------------------------*/
int8_t BackupFile_Delete(void)
{
	return File_Delete(Config_Name);
} 			   
/*-------------------------------------------------------------------------------------*/
int8_t ConfigFile_New(void)
{
	return File_New(Config_Name, gFileConfig);
}
/*-------------------------------------------------------------------------------------*/
int8_t ConfigFile_Write(char *str)
{
	return File_Write(Config_Name, gFileConfig, str);
}
/*-------------------------------------------------------------------------------------*/
int8_t ConfigFile_Read(char *buf)
{
	return File_Read(Config_Name, gFileConfig, buf);
}
/*-------------------------------------------------------------------------------------*/
int8_t ConfigFile_Delete(void)
{
	return File_Delete(Backup_Name);
} 			   
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

