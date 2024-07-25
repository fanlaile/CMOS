/****************************************************************************
 * @file    
 * @version  V1.00
 * @brief    
 * @author
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef __MAIN_H__
#define __MAIN_H__

#include "gd32e23x.h"

#define BOOTLOADER  					(0x08000000)   // 11KB  //BootLoader起始地址  

#define UPDATE_FLAG     			(0x08002C00) 	  //升级标志地址 
#define UPDATE_FLAG_MAX_SIZE	(0x0400)			// 1KB

#define APP_CODE_OFFSET 			(0x08003000)   	//APP起始地址 
#define APP_CODE_SIZE   			(0x6800)      // 26KB            

#define UPDATE_CODE_OFFSET 		(0x08009800)   //APPBAK区(备份区）起始地址
#define UPDATE_CODE_SIZE   		(0x6800)      // 26KB  

#define FLASH_SECTOR_SIZE 	 	(0x400)   		// 1KB

/** 
  * @brief  GD Status structures definition  
  */  
typedef enum 
{
  GD_OK       = 0x00U,
  GD_ERROR    = 0x01U,
  GD_BUSY     = 0x02U,
  GD_TIMEOUT  = 0x03U
} GD_StatusTypeDef;

void jump_to_app(uint32_t addr);
uint8_t FLASH_data_check(uint32_t faddr, uint16_t value);
#endif //__MAIN_H__


