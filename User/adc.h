/****************************************************************************
 * @file    
 * @version  V1.00
 * @brief    
 * @author
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#ifndef __ADC_H__
#define __ADC_H__

#include "gd32e23x.h"


/* USER CODE BEGIN Private defines */
#define LSD_SIZE (1500+50)

typedef struct  {
	
	uint8_t LED_PWM;
	uint8_t SP_STATUS;
	uint8_t LSD_START;
	uint8_t LSD_INT;
	uint8_t CMOS_START;
	uint16_t LSD_RESULT;
	uint16_t LSD_ORIGIN;
	uint16_t LSD_ADC[LSD_SIZE*2];
	float LSD_VALUE[5];
	uint8_t CMOS_TIMER;
	uint8_t LSD_AUTO;
	uint16_t EP_HI;
	uint16_t EP_LO;
	uint16_t Laser_Exp;
	uint16_t Line_Period;
	uint16_t pWd,pRd;
	uint8_t LSD_ERR_CNT;
}LSD_Struct;


#define IO3V3(x)		gpio_bit_write(GPIOF,GPIO_PIN_1,x)


void ADC_Init(void);
void DMA_ConvCpltCallback(void);
void ADC_Start_DMA( uint16_t *pData, uint16_t length);
void ADC_Stop_DMA(void);

uint16_t adc_get_result(void);
unsigned int Get_ADC_Value(void);
unsigned int Get_Temp(void);
#endif //__TEMPERATURE_H__



