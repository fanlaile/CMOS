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

void CMOS_RUN_CLOCK(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	/*  configure SysTick */
	systick_config();
	delay_1ms(100);
	/* configure EXTI, TIMER */
	gpio_config();
	
	Usart_GPIO_Init();
	PWM_timer_config();
	ADC_Init();
	Usart_Init();

	printf("========== c378 Test Start fan ==========\r\n\r\n ");
    
	while(1)
	{
		
		UART_TEST_HANDLE();			//uart debug
		if(LSD_CMOS.LSD_START || LSD_CMOS.CMOS_START)
		{
			CMOS_RUN_CLOCK();
			
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

	CMOS_CLK();
	
	CMOS_ADC(arount_cnt);

	arount_cnt++;
	
	
	if(arount_cnt>=5)
	{
		CMOS_DIS(arount_cnt);
		arount_cnt=0;
		LSD_CMOS.LSD_INT=0;
		LSD_CMOS.SP_STATUS=0;
		LSD_CMOS.LSD_START=0;
		memset(LSD_CMOS.LSD_ADC,0,sizeof(uint16_t)*1424);
		memset(LSD_CMOS.LSD_VALUE,0x00,sizeof(LSD_CMOS.LSD_VALUE));
	}
	LSD_CMOS.LSD_INT=0;
	LSD_CMOS.SP_STATUS=0;
	memset(LSD_CMOS.LSD_ADC,0,sizeof(uint16_t)*1424);
	delay_1ms(1);
}

