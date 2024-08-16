
/*!
    \file    flash_operation.c
    \brief   flash operation driver

    \version 2023-06-25, V3.1.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "flash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

FMC_STR fmc_str={
	0,
	{0X12341234, 0xabc,0, 30, 0,0},
	
};

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

//擦除flash
void FMC_ERASE_PAGE(uint32_t adr)
{
	uint32_t Address = adr;
	/* 解锁 */
	fmc_unlock();
	/* step1: erase pages */
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
	fmc_page_erase(Address);
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
	fmc_lock();
}

// 写flash数据
uint8_t FMC_WRITE_BUFFER(uint32_t adr,uint32_t *buffer,uint16_t len)
{
	uint16_t i;
	uint32_t Address = adr;
	/* 解锁 */
	fmc_unlock();
	/* step1: erase pages */
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
	fmc_page_erase(FMC_DATA_ADDR);
  /* step2: 操作FMC前先清空STAT 状态寄存器，非常必要*/
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
	// 开始写入
	for (i=0; i<len; i++)
	{
		fmc_word_program(Address, buffer[i]);
		Address += 4U;
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
	}
	fmc_lock();
	return 0;
}
/****************** flash write **************/
/*!
    \brief      FLASH写
    @Address    操作地址
    @Size       数据长度
*/
uint8_t FMC_FLASH_Write(uint32_t data ,uint32_t adr)
{
       
    uint32_t Address = adr;
	
    /* 解锁 */
    fmc_unlock();
	/* step1: erase pages */
    fmc_page_erase(Address);
    /* step2: 操作FMC前先清空STAT 状态寄存器，非常必要*/
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
    fmc_word_program(Address, data);
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
    /* 上锁 */
    fmc_lock();
    return 0;
}
/****************** flash read **************/

/*!
    \brief      FLASH读
    @Address    操作地址
    @data      数据指针
*/
uint8_t FMC_FLASH_Read(uint32_t *data ,uint32_t adr)
{
    uint32_t Address = adr;
 
    *data = *( __IO uint32_t* ) Address;
	
    return 0;

}

/**
  * @brief  跳转到用户程序执行
  * @param  void
  * @retval none
  */
uint8_t Execute_user_Program(void)
{
	__disable_irq();
	
	
//	NVIC_SystemReset();
//    if (((*(__IO uint32_t *)ApplicationAddress) & 0x2FFE0000) == 0x20000000)//判断用户是否已经下载程序，防止跑飞
//    {
//				SerialPutString((uint8_t*)"jump to new\r\n");
        //跳转至用户代码
        JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);
        Jump_To_Application = (pFunction)JumpAddress;
        //初始化用户程序的堆栈指针
        __set_MSP(*(__IO uint32_t *)ApplicationAddress);
        Jump_To_Application();
//    }
//    else
//    {
//				SerialPutString((uint8_t*)"jump error\r\n");
//        return 1;
//    }   
    return 0;
}
 

