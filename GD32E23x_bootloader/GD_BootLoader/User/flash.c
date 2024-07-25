
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
/*!
    \brief      erase flash
    \param[in]  address: erase start address
    \param[in]  file_length: file length
    \param[in]  report_buffer: report buffer
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
void flash_erase (uint32_t address, uint32_t file_length, uint8_t* report_buffer)
{
	uint32_t erase_counter;
    uint16_t start_sector = 0U, end_sector = 0U, i = 0U;

	/* unlock the flash program/erase controller */
	fmc_unlock();
    /* clear pending flags */
  fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
	
	/* erase the flash pages */
	for(erase_counter = 0U; erase_counter < FMC_PAGE_SIZE; erase_counter++){
			fmc_page_erase(address + (FMC_PAGE_SIZE * erase_counter));
			fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
	}
	
	/* lock the main FMC after the erase operation */
	fmc_lock();


}

/*!
    \brief      write data to sectors of memory
    \param[in]  data: data to be written
    \param[in]  addr: sector address/code
    \param[in]  len: length of data to be written (in bytes)
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
void iap_data_write (uint8_t *data, uint32_t addr, uint32_t len)
{
    uint32_t idx = 0U;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    /* data received are word multiple */
    for (idx = 0U; idx < len; idx ++) {
        if (FMC_READY == fmc_word_program(addr, *(uint32_t *)(data + idx))) {
            addr += 2U;
        } else {
            while(1) {
            }
        }
    }

    fmc_lock();
}
// 读flash数据
uint8_t fmc_read_buffer(uint16_t offset,uint8_t *buffer,uint16_t len)
{
	uint32_t rdAddr = FMC_START_ADDR + offset;
 
	for( uint16_t dataIndex = 0; dataIndex < len; dataIndex++ )
	{
		buffer[dataIndex] = *( __IO uint8_t* ) rdAddr;
		rdAddr += 1;//地址累加
	}
	return 0;
}
// 写flash数据
uint8_t fmc_write_buffer(uint16_t offset,uint8_t *buffer,uint16_t len)
{
	// 因为FMC是需要按4字节写入，所以这里需要做个转换，将uint_t8转换成uint_t32
	// 计算转换成uint32后长度
	uint16_t size = ceil(len/4.0f);
	uint32_t *data = malloc(size*4);
	memset(data,0,size*4);
	
	// uint8_t*转uint32_t*
	for(uint16_t i=0;i<size;i++)
	{
		data[i] = buffer[i*4+0] + (buffer[i*4+1]<<8) + (buffer[i*4+2]<<16) + (buffer[i*4+3]<<24);
	}
	
	fmc_state_enum FLASHStatus;
	uint16_t i;    
	uint32_t AddressTemp = 0;
 
	/*写入数据*/
	AddressTemp = FMC_START_ADDR + offset;
	/* 解锁 */
	fmc_unlock();
	
	/* clear all pending flags */
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
	/* erase the flash pages */
	fmc_page_erase(FMC_START_ADDR);
	
	/* 操作FMC前先清空STAT 状态寄存器，非常必要*/
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
	// 开始写入
	for (i=0; i<size; i++)
	{
		FLASHStatus = fmc_word_program(AddressTemp, data[i]);
		AddressTemp += 4U;
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
	}
	/* 上锁 */
	fmc_lock();
	
	free(data);
	
	return 0;
 
}
/****************** flash write **************/
/*!
    \brief      FLASH写
    @Address    操作地址
    @Size       数据长度
*/
uint8_t FLASH_ProgramWord(uint32_t adr ,uint32_t data)
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

// 写flash数据
uint8_t iap_write_32bit_data(uint32_t adr,uint16_t len,int32_t* buffer)
{
	uint16_t i;
	uint32_t Address = adr;
	/* 解锁 */
	fmc_unlock();
	/* step1: erase pages */
	fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );

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


