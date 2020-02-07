/**
  **************************************************************************************
  * @File      sd_card.c
  * @Author    ChuanYong Wan
  * @brief     SD card drive function. Only support v2.0 SDHC/SDXC
  * @Version   1.0
	* @Date			 2018/08/15
	* @SDcard    High Capacity SD Memory Card 
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *				     Without permission, shall not be reproduced and copied, shall not be 
  *			       used for commercial purposes.
  **************************************************************************************
 **/
 
/* Include ----------------------------------------------------------------------------*/
#include "string.h"
#include "sd_card.h"
#include "stm32f10x_rcc.h"

/* Variable ---------------------------------------------------------------------------*/
SPI_InitTypeDef SD_SPI_InitStructure; 

/* Function ---------------------------------------------------------------------------*/
static void SD_SPIInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(SD_SPIGPIO_RCC|SD_SPI_NUM, ENABLE);
	RCC_APB2PeriphClockCmd(SD_SPICS_RCC, ENABLE);	
 
	GPIO_InitStructure.GPIO_Pin   = SD_SPICLK_PIN|SD_SPIMISO_PIN|SD_SPIMOSI_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SD_SPI_GPIOX, &GPIO_InitStructure);
 	GPIO_SetBits(SD_SPI_GPIOX, SD_SPICLK_PIN|SD_SPIMISO_PIN|SD_SPIMOSI_PIN);

	GPIO_InitStructure.GPIO_Pin   = SD_SPICS_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP ;   
	GPIO_Init(SD_SPICS_GPIOX, &GPIO_InitStructure);
	GPIO_SetBits(SD_SPICS_GPIOX, SD_SPICS_PIN);
	
	SD_SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SD_SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;
	SD_SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b;
	SD_SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;	
	SD_SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;
	SD_SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft; 
	SD_SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SD_SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;	
	SD_SPI_InitStructure.SPI_CRCPolynomial = 7;	
	SPI_Init(SD_SPIX, &SD_SPI_InitStructure);
 
	SPI_Cmd(SD_SPIX, ENABLE);
}
/*-------------------------------------------------------------------------------------*/
void SD_SPI_SetSpeed(uint8_t speed)
{
	SD_SPI_InitStructure.SPI_BaudRatePrescaler = speed;
  SPI_Init(SD_SPIX, &SD_SPI_InitStructure);
	SPI_Cmd(SD_SPIX, ENABLE);
} 
/*-------------------------------------------------------------------------------------*/
uint8_t SD_SPI_TxRxByte(uint8_t byte)
{					 
	uint16_t cnt = SPI_RETRY_CNT;
	while((SPI_I2S_GetFlagStatus(SD_SPIX, SPI_I2S_FLAG_TXE)==RESET)&&(cnt--));	
	if(cnt==0) return SPI_ERROR;
	cnt = SPI_RETRY_CNT;	
	SPI_I2S_SendData(SD_SPIX, byte);

	while((SPI_I2S_GetFlagStatus(SD_SPIX, SPI_I2S_FLAG_RXNE)==RESET)&&(cnt--));  
	if(cnt==0) return SPI_ERROR;	
	return SPI_I2S_ReceiveData(SD_SPIX);			    
}
/*-------------------------------------------------------------------------------------*/
uint8_t SDcard_WaitReady(void)
{
	uint32_t t=0;
	do
	{
		if(SD_SPI_TxRxByte(0XFF)==0XFF) return SD_OK;
		t++;		  	
	}while(t<0XFFFFFF);
	return SD_ERROR;
}
/*-------------------------------------------------------------------------------------*/
static void SDcard_DisSelect(void)
{
	SD_CS = 1;
 	SD_SPI_TxRxByte(0XFF); //Provide 8 extra clock times
}
/*-------------------------------------------------------------------------------------*/
static uint8_t SDcard_Select(void)
{
	uint32_t cnt = 0;
	SD_CS = 0;
	do
	{
		if(SD_SPI_TxRxByte(0XFF)==0XFF)	return SD_OK; 
		cnt++;		  	
	}while(cnt<0XFFFFFF);
	
	SDcard_DisSelect();
	return SD_ERROR;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Send a command to the SD card.
	* @param  cmd: Command 
	* @param  arg: Command parameters
	* @param  crc: CRC check value
	* @retval SDcard's response 
	*			    Error code	
 **/													  
static uint8_t SDcard_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t ret=0, cnt=0; 
	SDcard_DisSelect();
	if(SDcard_Select()==SD_ERROR)	return SDERR_ACK;

	SD_SPI_TxRxByte(cmd|0X40);
	SD_SPI_TxRxByte(arg>>24);
	SD_SPI_TxRxByte(arg>>16);
	SD_SPI_TxRxByte(arg>>8);
	SD_SPI_TxRxByte(arg);	  
	SD_SPI_TxRxByte(crc); 
	
	if(cmd==CMD12) SD_SPI_TxRxByte(0XFF); //Skip a stuff byte when stop reading
	cnt = 0XFF;
	do
	{
		ret = SD_SPI_TxRxByte(0XFF);
	}while((ret&0X80)&&(cnt--)); //Check R1
  return ret;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Reading data from SD card.
	* @param  buff: Data buffer 
	* @param  len : The length of the data to be read 
	* @retval SD_OK
	*			    Error code		
 **/
static uint8_t SDcard_RecvData(uint8_t *buff, uint16_t len)
{			  	  
	uint32_t i = 0;
	uint16_t cnt = 0XFFFF;
	while((SD_SPI_TxRxByte(0XFF)!=0XFE)&&cnt) cnt--; //Start block token:0XFE	 	  
	if(cnt==0) return SDERR_ACK;

	while(len--)
	{
		buff[i] = SD_SPI_TxRxByte(0XFF);
		i++;
	}
	SD_SPI_TxRxByte(0XFF); //Two dummy CRC
	SD_SPI_TxRxByte(0XFF);									  					    
	return SD_OK;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Write a block(512 bytes) of SD card.
	* @param  buff: Data bufffer 
	* @param  cmd: Command 
	* @retval SD_OK
	*			    Error code	
 **/
static uint8_t SDcard_SendBlock(uint8_t *buff, uint8_t cmd)
{	
	uint16_t i=0, ret=0;	
	uint32_t cnt = 0XFFFFFF;
	while((SD_SPI_TxRxByte(0XFF)!=0XFF)&&cnt) cnt--;
	if(cnt==0) return SDERR_ACK; 
	
	SD_SPI_TxRxByte(cmd);
	if(cmd!=0XFD)       //Not the end token:0XFD
	{
		for(i=0;i<512;i++) SD_SPI_TxRxByte(buff[i]);
	  SD_SPI_TxRxByte(0XFF); //Two dummy CRC
	  SD_SPI_TxRxByte(0XFF);
		ret = SD_SPI_TxRxByte(0XFF); 
		if((ret&0X1F)!=0X05) return SDERR_WRITE; //Check data response token	
	}						 									  					    
  return SD_OK;
}
/*-------------------------------------------------------------------------------------*/
///**
//  * @brief  Get the CID information of the SD card.
//	* @param  *cid_data: Storage of CID information, at least 16Byte  
//	* @retval SD_OK
//	*			    Error code		
// **/													   
//static uint8_t SD_GetCID(uint8_t *cid_data)
//{
//	uint8_t ret = 0;	   
//	if(SDcard_SendCmd(CMD10,0,0X01)==0X00) //Read the card's CID register
//	{
//		ret = SDcard_RecvData(cid_data,16);  //Receiving 16 bytes of data	 
//	}
//	SDcard_DisSelect(); //Cancel chip select
//	if(ret)	return SD_ERROR;
//	else		return SD_OK;
//}				
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Get the CSD information of the SD card.
	* @param  *cid_data: Storage of CSD information, at least 16Byte  
	* @retval SD_OK
	*			    Error code
 **/													   
static uint8_t SDcard_GetCSD(uint8_t *csd_data)
{
	uint8_t ret=0;	 
	if(SDcard_SendCmd(CMD9,0,0X01)==0X00)  //Read card's CSD register
	{
    ret = SDcard_RecvData(csd_data, 16); //Receiving 16 bytes of data 
  }
	SDcard_DisSelect(); //Cancel chip select
	if(ret!=SD_OK)	return SD_ERROR;
	else	          return SD_OK;
} 
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Get the total number of sectors in the SD card. The number of bytes in each 
	*					sector must be 512. Because if it is not 512, the initialization can not be 
	*					passed
	* @param  None
	* @retval 0:Error 
	*					Other--The capacity of the SD card (the number of sectors(512KB))	
 **/															  
uint32_t SDcard_GetCapacity(void)
{
	uint8_t CSD[16];
	uint32_t C_size = 0;  
	memset(CSD, 0, sizeof(CSD));
	
	if(SDcard_GetCSD(CSD)==SD_ERROR)	return SD_ERROR;  
	if((CSD[0]&0xC0)==0x40) //CSD Version 2.0
	{	
		C_size = (uint32_t)CSD[9]+((uint32_t)CSD[8]<<8)+((uint32_t)(CSD[7]&0x3F)<<16)+1;	   
	}
	return C_size;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Read SD card.
	* @param  *buf:   Data buffer
	* @param  sector: Sector to be read
	* @param  num:    The number of sectors you want to read
	* @retval ret
 **/ 
uint8_t SDcard_ReadDisk(uint8_t *buf, uint32_t sector, uint8_t num)
{
	uint8_t ret = 0;
	if(num==1)
	{
		ret = SDcard_SendCmd(CMD17,sector,0X01);   //Read command
		if(ret==0) ret = SDcard_RecvData(buf,512); //Receive 512 Bytes	   
	}
	else
	{
		ret = SDcard_SendCmd(CMD18,sector,0X01); //Continuous reading command
		if(ret==0)
		{
			do
			{
				ret = SDcard_RecvData(buf,512); //Receive 512 Bytes 
				buf += 512;  
			}while((--num)&&(ret==SD_OK)); 	
		}
		SDcard_SendCmd(CMD12,0,0X01); //Send stop commmand
	}   
	SDcard_DisSelect(); //Cancel chip select
	return ret;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Write SD card.
	* @param  *buf:   Data buffer
	* @param  sector: Sector to be writed
	* @param  num:    Sector number
	* @retval ret
 **/ 
uint8_t SDcard_WriteDisk(uint8_t *buf, uint32_t sector, uint8_t num)
{
	uint8_t ret = 0;
	if(num==1)
	{
		ret = SDcard_SendCmd(CMD24,sector,0X01);     //Write command
		if(ret==0) ret = SDcard_SendBlock(buf,0xFE); //Write 512 Bytes	   
	}
	else
	{
		SDcard_SendCmd(CMD55,0,0X01);	
		SDcard_SendCmd(CMD23,num,0X01);
 		ret = SDcard_SendCmd(CMD25,sector,0X01); //Continuous writing command
		if(ret==0)
		{
			do
			{
				ret = SDcard_SendBlock(buf,0xFC); //Send 512 Bytes	 
				buf += 512;  
			}while((--num) && (ret==SD_OK));
			ret = SDcard_SendBlock(NULL,0xFD); //Send 512 Bytes
		}
	}   
	SDcard_DisSelect();//Cancel chip select
	return ret;
}	
/*-------------------------------------------------------------------------------------*/
uint8_t SDcard_Init(void)
{
	uint8_t ret=0, buf[4]={0};
	uint8_t SDcard_Type = 0;
	u16 i=0, cnt=20;
	
	memset(buf, 0, sizeof(buf));
	SD_SPIInit();
 	for(i=0;i<10;i++) SD_SPI_TxRxByte(0XFF);    //Sending at least 74 pulses
	do
	{
		ret = SDcard_SendCmd(CMD0,0,0x95);        //Entering the IDLE state
	}while((ret!=0X01)&&(cnt--));
	
	if(ret==0X01)
	{
		if(SDcard_SendCmd(CMD8,0x1AA,0x87)==0X01) //Check supply voltage(2.7-3.6V)
		{
			for(i=0;i<4;i++) buf[i] = SD_SPI_TxRxByte(0XFF);
			if((buf[2]==0X01)&&(buf[3]==0XAA))
			{
				cnt = 0XFFFE;
				do
				{
					SDcard_SendCmd(CMD55,0,0X01);       //Define the next cmd is specific cmd
					ret = SDcard_SendCmd(CMD41,0x40000000,0X01); //Activates card's init process
				}while(ret&&(cnt--));
				if(cnt&&(SDcard_SendCmd(CMD58,0,0X01)==0))	 //Read card's OCR register
				{
					for(i=0;i<4;i++) buf[i] = SD_SPI_TxRxByte(0XFF);
					if(buf[0]&0x40) SDcard_Type = SDHXC;
					else            SDcard_Type = SDSC;
				}
			}
		}
	}

	SDcard_DisSelect();
	SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);//High speed
	if(SDcard_Type==SDHXC) return SD_OK;
	else return SD_ERROR;	  		
}
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

