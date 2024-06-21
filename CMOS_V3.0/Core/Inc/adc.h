/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc;

/* USER CODE BEGIN Private defines */
typedef struct  {
	uint8_t SP_STATUS;
	uint8_t LSD_START;
	uint8_t LSD_INT;
	uint8_t CMOS_START;
	uint16_t LSD_OFFSET;
	uint16_t LSD_ADC[LSD_SIZE];
	float LSD_VALUE[15];
}LSD_Struct;


/* USER CODE END Private defines */

void MX_ADC_Init(void);

/* USER CODE BEGIN Prototypes */
uint32_t adc_get_result(void);
void ADC_dispose (void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

