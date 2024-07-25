
#ifndef __FLASH_OPERATION_H
#define __FLASH_OPERATION_H

#include "gd32e23x.h"

typedef struct{
	uint32_t fmc_flag;
	uint32_t fmc_buffer[6];
}FMC_STR;

extern FMC_STR fmc_str;


#define	FLAG_UPDATA 	0
#define	FLAG_INIT  		1
#define	DATA_ORIGIN 	2
#define	DATA_PWM 			3
#define	DATA_RES1 		4
#define	DATA_RES2 		5


#define ApplicationAddress   			0x8000000

// “≥¥Û–° 1K
#define FMC_PAGE_SIZE		((uint16_t)0x400U)
#define FMC_START_ADDR 	((uint32_t)0x0800FC00U)
#define FMC_DATA_ADDR   ((uint32_t)0x0800FC00U)
#define FMC_FLAG_ADDR		(FMC_DATA_ADDR+4)
#define FMC_DAT1_ADDR		(FMC_DATA_ADDR+8)
#define FMC_DAT2_ADDR		(FMC_DATA_ADDR+12)
#define FMC_DAT3_ADDR		(FMC_DATA_ADDR+16)
#define FMC_DAT4_ADDR		(FMC_DATA_ADDR+20)

/* base address of the FMC sectors */


/* function declarations */
/* erase flash */
uint8_t FMC_WRITE_BUFFER(uint32_t adr,uint32_t *buffer,uint16_t len);

uint8_t FMC_FLASH_Write(uint32_t data ,uint32_t adr);
uint8_t FMC_FLASH_Read(uint32_t *data ,uint32_t adr);

uint8_t Execute_user_Program(void);
void FMC_ERASE_PAGE(uint32_t adr);

#endif /* __FLASH_OPERATION_H */
