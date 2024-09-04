/****************************************************************************
 * @file    	main.c
 * @version  	V1.00
 * @brief    	
 * @author		FAN
 * @note	
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/

#include "gd32e23x.h"
#include "gd32e230f_start.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>


/* USER CODE BEGIN PV */

char p_buf[128];
uint16_t adc_Buf;
uint8_t arount_cnt=0;
extern  LSD_Struct LSD_CMOS;
extern uint8_t show_flag;

void CMOS_RUN_CLOCK(void);
uint8_t FLASH_Init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	
	nvic_vector_table_set(NVIC_VECTTAB_FLASH,0x3000);
	__set_PRIMASK(0);
	/*  configure SysTick */
	systick_config();
	delay_1ms(100);
	/* configure EXTI, TIMER */
	gpio_config();
	
	Usart_GPIO_Init();
	PWM_timer_config();
	ADC_Init();
	Usart_Init();
	ds18b20_init();
//	
	delay_1ms(100);
  FLASH_Init();
//	printf("========== c378 Test Start fan ==========\r\n\r\n ");
	
	while(1)
	{
		
		UART_TEST_HANDLE();			//uart debug
		if(LSD_CMOS.LSD_START || LSD_CMOS.CMOS_START)
		{
			CMOS_RUN_CLOCK();
			
		}
		if(Laser_Init.led_init_flag)
		{
			Laser_Calibration();
		}
	}
}


/**
  * @brief CMOS RUN CP & SP
  * @retval None
  */
void CMOS_RUN_CLOCK(void)
{
//	static uint8_t arount_cnt=0;
//	memset(LSD_CMOS.LSD_ADC,0x00,sizeof(LSD_CMOS.LSD_ADC));
//	memset(LSD_CMOS.LSD_VALUE,0x00,sizeof(LSD_CMOS.LSD_VALUE));
	LASER_ON();

	CMOS_CLK();
	
	CMOS_ADC(arount_cnt);

	
	
	
	if(arount_cnt>=3)
	{
		CMOS_DIS(arount_cnt);
		arount_cnt=0;
		show_flag=0;                                                                                          
		LSD_CMOS.LSD_INT=0;
		LSD_CMOS.SP_STATUS=0;
		LSD_CMOS.LSD_START=0;
		if(LSD_CMOS.CMOS_START==0)
		{
//			LASER_OFF();
		}
		memset(LSD_CMOS.LSD_ADC,0,sizeof(LSD_CMOS.LSD_ADC));
		memset(LSD_CMOS.LSD_VALUE,0x00,sizeof(LSD_CMOS.LSD_VALUE));
	}
	LSD_CMOS.LSD_INT=0;
	LSD_CMOS.SP_STATUS=0;
	memset(LSD_CMOS.LSD_ADC,0,sizeof(LSD_CMOS.LSD_ADC));
	arount_cnt++;
	delay_1ms(1);
}
/**
  * @brief FLASH INIT
  * @retval None
  */
uint8_t FLASH_Init(void)
{
//	uint32_t *fmc_flag;
//	uint16_t k;
	FMC_FLASH_Read(&fmc_str.fmc_flag,FMC_FLAG_ADDR);
//	k=*fmc_flag>>16;
	if(fmc_str.fmc_buffer[FLAG_INIT]!=fmc_str.fmc_flag)
	{
		FMC_WRITE_BUFFER(FMC_START_ADDR, fmc_str.fmc_buffer,6);//初始化标志
//		printf("flash init\r\n");
	}
	else
	{
		FMC_FLASH_Read(&fmc_str.fmc_buffer[DATA_ORIGIN],FMC_DAT1_ADDR);
		FMC_FLASH_Read(&fmc_str.fmc_buffer[DATA_PWM],FMC_DAT2_ADDR);
		LSD_CMOS.LSD_ORIGIN=(uint16_t)fmc_str.fmc_buffer[DATA_ORIGIN];
		LSD_CMOS.LED_PWM=(uint16_t)fmc_str.fmc_buffer[DATA_PWM];

	}
	return 0;
}




