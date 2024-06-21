/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
//extern uint16_t adcBuf[];
//extern uint16_t LSD_ADC[];
extern char p_buf[];
LSD_Struct LSD_CMOS={ 
	0,
	0,
	0,
	0,
	0,
	{0},
	{0},
};


/* USER CODE END 0 */

ADC_HandleTypeDef hadc;

/* ADC init function */
void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC GPIO Configuration
    PA0     ------> ADC_IN0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC GPIO Configuration
    PA0     ------> ADC_IN0
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint32_t adc_get_result(void)
{
	
    HAL_ADC_Start(&hadc);//单次转换模式，每次转换完成后ADC转换会自动停止
    HAL_ADC_PollForConversion(&hadc, 1);//ADC转换，转换参数ms
    return (uint16_t)HAL_ADC_GetValue(&hadc);
}



void ADC_dispose (void)
{
	

//	HAL_ADC_Start_DMA(&hadc, (uint32_t *)&adcBuf, LSD_SIZE);
//	static   uint16_t i;
//	for(i=0;i<LSD_SIZE;i++){                       							 //遍历10次，进行滤波
//		LSD_CMOS.LSD_ADC[i]=(uint16_t )adcBuf[i];
//		sprintf(p_buf,"adc[%d] = %d\r\n",i,LSD_CMOS.LSD_ADC[i]);
////		printf("%s",p_buf);
//		if(LSD_CMOS.LSD_ADC[i]<2300 && LSD_CMOS.LSD_START==0){
//			LSD_CMOS.LSD_START=1;
////			printf("11111\r\n\r\n");
//		}
//		else if(LSD_CMOS.LSD_START==1 && LSD_CMOS.LSD_ADC[i]>3100){
//			LSD_CMOS.LSD_START=2;
//			LSD_CMOS.LSD_OFFSET=i;
////			printf("22222\r\n\r\n");
//		}
//		else if(LSD_CMOS.LSD_START==2 && LSD_CMOS.LSD_ADC[i]<2500){
//			LSD_CMOS.LSD_VALUE=i-LSD_CMOS.LSD_OFFSET;
//			sprintf(p_buf,"lsd = %f\r\n",LSD_CMOS.LSD_VALUE);
//			printf("%s",p_buf);
//			LSD_CMOS.LSD_START=0;
//			LSD_CMOS.LSD_OFFSET=0;
//			break;
//		}
//	}
//	LSD_CMOS.LSD_START=0;
//	LSD_CMOS.LSD_OFFSET=0;
	
}

/* USER CODE END 1 */
