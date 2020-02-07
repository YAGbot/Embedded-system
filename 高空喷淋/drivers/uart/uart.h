/**
  ******************************************************************************
  * @file    uart.h
  * @author  BUG
  * @version V1.0.0
  * @date    2018/2/25
  * @brief   UART������������
  ******************************************************************************
  */
#ifndef _UART_H
#define _UART_H



#include "stm32f10x.h"
#include <stdio.h>

#define PRRINTF1 


#define print(format, ...) printf (format, ##__VA_ARGS__)



/* �������� */
void uart2_init(u32 bound);
void uart2_send_byte(u8 c);






#endif


