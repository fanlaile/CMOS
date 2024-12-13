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
void Get_LSD_Data(void);
uint8_t FLASH_Init(void);

uint8_t	gADC_Done = 0;
uint8_t	gDIS_Done = 0;
// uint16_t pWd=0,pRd=LSD_SIZE;
uint32_t gLineCount = 0;
// uint8_t  g_transfer_complete = SET;
void DMA_ConvCpltCallback()
{
		EP_H(); 
		gLineCount++;
		gADC_Done = SET;
		EP_L();
		if(LSD_CMOS.CMOS_START) {
			if( gDIS_Done == RESET ) {
				if( LSD_CMOS.pWd == 0 ) {
					LSD_CMOS.pWd = LSD_SIZE; 
					LSD_CMOS.pRd = 0;
				}
				else {
					LSD_CMOS.pWd = 0; 
					LSD_CMOS.pRd = LSD_SIZE;
				}
			}
			ADC_Start_DMA( (uint16_t *)&LSD_CMOS.LSD_ADC[LSD_CMOS.pWd], LSD_SIZE);
//			Get_LSD_Data();
		}
		EP_L();
}

void Get_LSD_Data()
{
	uint16_t i;
	uint16_t tmpVal;
	uint16_t maxLoc, minLoc;
	uint16_t maxVal=0, minVal=0xffff;
	
	gDIS_Done = SET;
	if(	gADC_Done == SET) {
		for(i=80;i<LSD_SIZE;i++) {	//
			tmpVal = LSD_CMOS.LSD_ADC[i];
			if( tmpVal > maxVal) 
			{ 
				maxVal = tmpVal;
				maxLoc = i;
			}
			if( tmpVal < minVal) 
			{ 
				minVal = tmpVal;
				minLoc = i;
			}
		}
//		LSD_CMOS.LSD_VALUE[0] = maxVal;
//		LSD_CMOS.LSD_VALUE[1] = maxLoc;
//		LSD_CMOS.LSD_VALUE[2] = minVal;
//		LSD_CMOS.LSD_VALUE[3] = minLoc;		
		if((gLineCount & 0x1f) == 0 ) 
		{
//			sprintf(p_buf,"max Val=%d\r\n",maxVal);
//			printf("max=%d(%d),  min=%d(%d)\n\r",maxVal, maxLoc, minVal, minLoc);
//			for(i=0 ; i<LSD_SIZE ; i++)
//			{
//				printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
//			}
			CMOS_ADC(1);
//			CMOS_RUN_CLOCK();
		}
//		printf("1111\r\n");
		gADC_Done = RESET;
	}
	gDIS_Done = RESET;	
}

/*!
    \brief      configure DMA interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(DMA_Channel0_IRQn, 0);
}

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
	nvic_config();
	
	Usart_GPIO_Init();
	ADC_Init();
	delay_1ms(100);
//	timer2_config();	  // SP & Laser_Exp 
	timer13_config();		// CP
		
	Usart_Init();
	ds18b20_init();
	readtemp();
	delay_1ms(100);
	FLASH_Init();
	printf("========== c378 Test Start fan ==========\r\n\r\n ");

//  LSD_CMOS.CMOS_START = 1;
	if(	LSD_CMOS.CMOS_START ==1)
		ADC_Start_DMA( (uint16_t *)&LSD_CMOS.LSD_ADC[LSD_CMOS.pWd], LSD_SIZE);

	while(1)
	{	
		UART_TEST_HANDLE();			//uart debug
		if(LSD_CMOS.LSD_START || LSD_CMOS.CMOS_START)
		{
			Get_LSD_Data();
		
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
//	LASER_ON();

////	CMOS_CLK();
//	return;

//	if(LSD_CMOS.CMOS_TIMER)
	{
		
//		printf("timer\r\n");				
		if(arount_cnt>=2)
		{
			CMOS_DIS(arount_cnt);
			memset(LSD_CMOS.LSD_ADC,0,sizeof(LSD_CMOS.LSD_ADC));
			memset(LSD_CMOS.LSD_VALUE,0x00,sizeof(LSD_CMOS.LSD_VALUE));
			show_flag=0;                                                                                          
			
			arount_cnt=0;
			if(LSD_CMOS.CMOS_START==0)
			{				
//				LASER_OFF();
			}
			delay_1ms(1);
			LSD_CMOS.LSD_INT=0;
			LSD_CMOS.SP_STATUS=0;
			LSD_CMOS.LSD_START=0;
			LSD_CMOS.CMOS_TIMER=0;
		}
		else
		{
			CMOS_ADC(arount_cnt);
			memset(LSD_CMOS.LSD_ADC,0,sizeof(LSD_CMOS.LSD_ADC));
			LSD_CMOS.LSD_INT=0;
			arount_cnt++;
//			delay_1ms(5);
//			LSD_CMOS.CMOS_TIMER=0;
		}
	}
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
		FMC_FLASH_Read(&fmc_str.fmc_buffer[DATA_TIM2_P],FMC_DAT3_ADDR);
		LSD_CMOS.LSD_ORIGIN=(uint16_t)fmc_str.fmc_buffer[DATA_ORIGIN];
		LSD_CMOS.LED_PWM=(uint16_t)fmc_str.fmc_buffer[DATA_PWM];
		LSD_CMOS.Line_Period=(uint16_t)fmc_str.fmc_buffer[DATA_TIM2_P];
	}
	timer2_config();
	return 0;
}




