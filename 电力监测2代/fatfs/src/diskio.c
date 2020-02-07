/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sd_card.h"
#include "malloc.h"

/* Definitions of physical drive number for each drive */
#define SD_CARD		0

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS ret = 0;

	switch (pdrv) 
	{
		case SD_CARD :
			ret = SDcard_Init();
			if(ret!=SD_OK)
			{
				SD_SPI_SetSpeed(SPI_BaudRatePrescaler_256);
				SD_SPI_TxRxByte(0xFF);
				SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);
			}
			break;
		default:
			break;
	}
	if(ret!=SD_OK) return STA_NOINIT;
	return 0;
}
/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DSTATUS ret = 0;
	
	if(count==0) return RES_PARERR;
	switch (pdrv) 
	{
		case SD_CARD:
			ret = SDcard_ReadDisk(buff, sector, count);
			if(ret!=SD_OK)
			{
				SD_SPI_SetSpeed(SPI_BaudRatePrescaler_256);
				SD_SPI_TxRxByte(0xFF);
				SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);
			}
			break;
		default:
			break;
	}
	if(ret!=SD_OK) return RES_ERROR;	 
  return RES_OK;
}
/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DSTATUS ret = 0;
	
	if(count==0) return RES_PARERR;
	switch (pdrv) 
	{
		case SD_CARD:
			ret = SDcard_WriteDisk((uint8_t*)buff, sector, count);
			break;
		default:
			break;
	}
	if(ret!=SD_OK) return RES_ERROR;	 
  return RES_OK;
}
/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT ret;						  			     
	if(pdrv==SD_CARD)//SD¿¨
	{
		switch(cmd)
		{
			case CTRL_SYNC:
				SD_CS=0;
				if(SDcard_WaitReady()==0) ret = RES_OK; 
				else                      ret = RES_ERROR;	  
				SD_CS=1;
				break;	 
			case GET_SECTOR_SIZE:
					*(WORD*)buff = 512;
					ret = RES_OK;
					break;	 
			case GET_BLOCK_SIZE:
					*(WORD*)buff = 8;
					ret = RES_OK;
					break;	 
			case GET_SECTOR_COUNT:
					*(DWORD*)buff = SDcard_GetCapacity();
					ret = RES_OK;
					break;
			default:
					ret = RES_PARERR;
					break;
		}
	}
	return ret;
}
/*-------------------------------------------------------------------------------------*/
DWORD get_fattime (void)
{				 
	return 0;
}			 
///*-------------------------------------------------------------------------------------*/
//void *ff_memalloc(UINT size)			
//{
//	return (void*)My_mem_malloc(size);
//}
///*-------------------------------------------------------------------------------------*/
//void ff_memfree(void* mf)		 
//{
//	My_mem_free(mf);
//}
