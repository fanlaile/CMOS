/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include "adc.h"
uint32_t adcBuf;


/* USER CODE END 0 */

TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim16;

/* TIM14 init function */
void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */
  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 9;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */
//	HAL_TIM_Base_Start_IT(&htim14);
  /* USER CODE END TIM14_Init 2 */

}
/* TIM16 init function */
void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 499;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM14)
  {
  /* USER CODE BEGIN TIM14_MspInit 0 */

  /* USER CODE END TIM14_MspInit 0 */
    /* TIM14 clock enable */
    __HAL_RCC_TIM14_CLK_ENABLE();

    /* TIM14 interrupt Init */
    HAL_NVIC_SetPriority(TIM14_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM14_IRQn);
  /* USER CODE BEGIN TIM14_MspInit 1 */

  /* USER CODE END TIM14_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspInit 0 */

  /* USER CODE END TIM16_MspInit 0 */
    /* TIM16 clock enable */
    __HAL_RCC_TIM16_CLK_ENABLE();

    /* TIM16 interrupt Init */
    HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM16_IRQn);
  /* USER CODE BEGIN TIM16_MspInit 1 */

  /* USER CODE END TIM16_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM14)
  {
  /* USER CODE BEGIN TIM14_MspDeInit 0 */

  /* USER CODE END TIM14_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM14_CLK_DISABLE();

    /* TIM14 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM14_IRQn);
  /* USER CODE BEGIN TIM14_MspDeInit 1 */

  /* USER CODE END TIM14_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM16)
  {
  /* USER CODE BEGIN TIM16_MspDeInit 0 */

  /* USER CODE END TIM16_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM16_CLK_DISABLE();

    /* TIM16 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM16_IRQn);
  /* USER CODE BEGIN TIM16_MspDeInit 1 */

  /* USER CODE END TIM16_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void LSD_PRINT(uint16_t *data)
{
	for(uint16_t i=0;i<1024;i++)
	{
		printf("adc = %d\r\n",data[i]);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//	static uint8_t sp_status=0,cp_status=0;
//	static uint16_t cnt=0,ADC_cnt=0;
//	UNUSED(htim);
	if(htim->Instance == TIM16)  /* tim3*/
	{
		
	}
//	if(htim->Instance == TIM14)
//	{
//		if(cp_status == 0)
//		{
////			printf("tim14\r\n");
////			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_SET);
//			GPIOA->BSRR = (uint32_t)GPIO_PIN_4;
//			
////			if(LSD_CMOS.LSD_START==1){
////				LSD_CMOS.LSD_ADC[ADC_cnt] = adc_get_result();
////				if(LSD_CMOS.LSD_ADC[ADC_cnt-1]>3000 && LSD_CMOS.LSD_ADC[ADC_cnt]<2700)
////				{
////					printf("adc = %d",ADC_cnt);
////				}
////			}
////			if(ADC_cnt>=LSD_SIZE)
////			{
////				ADC_cnt=0;
////				LSD_PRINT(LSD_CMOS.LSD_ADC);
////				LSD_CMOS.LSD_START=0;
////				printf("\r\n\r\n\r\n");
////			}
////			else{
////				ADC_cnt++;
////			}
//		}
//		else
//		{
////			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4, GPIO_PIN_RESET);
////			GPIOA->BRR = (uint32_t)GPIO_PIN_4;
//		}
//		cp_status=!cp_status;
//	}
}

/* USER CODE END 1 */
