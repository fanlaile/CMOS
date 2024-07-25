/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Inc/common.h
  * @author  MCD Application Team
  * @brief   This file provides all the headers of the common functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H
#define __COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "gd32e23x.h"

/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);
#define ApplicationAddress    						 0x8003000
#define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
#define FLASH_SIZE                        (0x10000)  /* 64 KBytes */
#define FLASH_LAST_PAGE    								 0X0800F800
#define FLASH_DATA_VAL     								0x1234

//计算上传文件大小 
#define FLASH_IMAGE_SIZE                   (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))
/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define TX_TIMEOUT          ((uint32_t)100)
#define RX_TIMEOUT          ((uint32_t)0xFFFFFFFF)

/* Exported macro ------------------------------------------------------------*/
#define IS_CAP_LETTER(c)    (((c) >= 'A') && ((c) <= 'F'))
#define IS_LC_LETTER(c)     (((c) >= 'a') && ((c) <= 'f'))
#define IS_09(c)            (((c) >= '0') && ((c) <= '9'))
#define ISVALIDHEX(c)       (IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       (c - '0')

#define CONVERTHEX_ALPHA(c) (IS_CAP_LETTER(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define CONVERTHEX(c)       (IS_09(c) ? ((c) - '0') : CONVERTHEX_ALPHA(c))

/* Exported functions ------------------------------------------------------- */
void Int2Str(uint8_t *p_str, uint32_t intnum);
uint32_t Str2Int(uint8_t *inputstr, uint32_t *p_intnum);
void Serial_PutByte( uint8_t ch );
void SerialPutString(uint8_t *s);
void common_init(void);
uint32_t SerialKeyPressed(uint8_t *key);
void SerialPutChar(uint8_t c);
uint32_t FLASH_PagesMask(__IO uint32_t Size);
uint8_t FLASH_ErasePage(uint32_t adr);
uint8_t Execute_user_Program(void);
uint8_t GetKey(void);
uint8_t GD_UART_Receive(uint32_t usart, uint8_t *val, uint8_t num, uint32_t timeout);
#endif  /* __COMMON_H */

/*******************(C)COPYRIGHT 2016 STMicroelectronics *****END OF FILE******/