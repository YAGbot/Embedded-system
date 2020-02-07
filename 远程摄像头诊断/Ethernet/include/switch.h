/**
  **************************************************************************************
  * @File      switch.h
  * @Author    ChuanYong Wan
  * @brief   	 Header files of switch drive functions.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *						 Without permission, shall not be reproduced and copied, shall not be used
	*						 for commercial purposes.
  **************************************************************************************
 **/
 
#ifndef	_SWITCH_H_
#define	_SWITCH_H_

/* Include ----------------------------------------------------------------------------*/
#include "stdint.h"

/* Function Prototype -----------------------------------------------------------------*/
void Switch_Init(void);
uint8_t Switch_Line1Read(void);
uint8_t Switch_Line2Read(void);

#endif
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/

