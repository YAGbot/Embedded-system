/**
  **************************************************************************************
  * @File      sd_card.h
  * @Author    ChuanYong Wan
  * @brief   	 Header files of SD card drive function. Only support v2.0 SDHC/SDXC
	* @Version   1.0
	* @SDcard    High Capacity SD Memory Card
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *						 Without permission, shall not be reproduced and copied, shall not be 
	*						 used for commercial purposes.
  **************************************************************************************
 **/
#ifndef __SD_CARD_H
#define __SD_CARD_H	 
/* Include ----------------------------------------------------------------------------*/
#include "sys.h"
#include "stm32f10x.h"	
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"

/* Define -----------------------------------------------------------------------------*/
#define SD_SPIX        SPI1
#define SD_SPICS_PIN   GPIO_Pin_0
#define SD_SPICLK_PIN  GPIO_Pin_5
#define SD_SPIMISO_PIN GPIO_Pin_6
#define SD_SPIMOSI_PIN GPIO_Pin_7
#define SD_SPI_NUM     RCC_APB2Periph_SPI1
#define SD_SPI_GPIOX   GPIOA
#define SD_SPICS_GPIOX GPIOB
#define SD_SPIGPIO_RCC RCC_APB2Periph_GPIOA
#define SD_SPICS_RCC   RCC_APB2Periph_GPIOB
#define SDSC 1     //SDSC
#define SDHXC	2    //SDHC or SDXC
#define CMD0    0  //SD card reset
#define CMD1    1
#define CMD8    8  //SEND_IF_COND
#define CMD9    9  //Read CSD data
#define CMD10   10 //Read CID data
#define CMD12   12 //Stop data transmission
#define CMD16   16 //Set SectorSize, Should return 0x00
#define CMD17   17 //Read sector
#define CMD18   18 //Read Multi sector
#define CMD23   23 //Should erasure of N block before multiple sector writes
#define CMD24   24 //Write sector
#define CMD25   25 //Write Multi sector
#define CMD41   41 //Should return 0x00
#define CMD55   55 //Should return 0x01
#define CMD58   58 //Read OCR information
#define CMD59   59 //Enable/Disable CRC£¬Should return 0x00
#define SPI_RETRY_CNT 1024
#define SPI_OK     	0
#define SPI_ERROR   1
#define SD_OK     	0x80
#define SD_ERROR 		0x81
#define SDERR_ACK   (SD_ERROR+1)
#define SDERR_WRITE (SD_ERROR+2)	   						 	 
#define	SD_CS  PBout(0)				

/* Function prototype -----------------------------------------------------------------*/
void    SD_SPI_SetSpeed(uint8_t speed);
uint8_t SD_SPI_TxRxByte(uint8_t byte);

uint8_t  SDcard_Init(void);
uint8_t  SDcard_WaitReady(void);
uint8_t  SDcard_ReadDisk(uint8_t *buf, uint32_t sector, uint8_t num);
uint8_t  SDcard_WriteDisk(uint8_t *buf, uint32_t sector, uint8_t num);	
uint32_t SDcard_GetCapacity(void);  				    
#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

