/****************************************************************************
 * @file    
 * @version  V1.00
 * @brief    
 * @author
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef __UART_DEBUG_H__
#define __UART_DEBUG_H__

#include "gd32e23x.h"





void Usart_Init(void);
void Usart_GPIO_Init(void);
void UART_TEST_HANDLE(void);
void V_STR_Printf(char *STR);
#endif //__UART_DEBUG_H__


