/**
  **************************************************************************************
  * @File      malloc.H
  * @Author    ChuanYong Wan
  * @brief   	 Header files of malloc.c.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *								 Without permission, shall not be reproduced and copied, shall not be 
	*							used for commercial purposes.
  **************************************************************************************
 **/
 
#ifndef __MALLOC_H
#define __MALLOC_H

/* Include ----------------------------------------------------------------------------*/
#include "stdint.h"
#include "stm32f10x.h"

/* Define -----------------------------------------------------------------------------*/
#ifndef NULL
#define NULL 0
#endif

#define MEM_BLOCK_SIZE 32 //Byte
#define MEM_SIZE 			 (35*1024)
#define MEM_TABLE_SIZE (MEM_SIZE/MEM_BLOCK_SIZE)

/* Function prototype -----------------------------------------------------------------*/	 		
uint32_t mem_malloc(uint32_t size);	
uint8_t  mem_free(uint32_t offset);

void MEM_Init(void);
void My_memset(void *addr, uint8_t value, uint32_t count); 		
void My_memcpy(void *des, void *src, uint32_t cnt);
void *My_mem_malloc(uint32_t size);			
void My_mem_free(void *ptr); 
void *My_mem_realloc(void *addr, uint32_t size);
#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/












