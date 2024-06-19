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
extern uint16_t adcBuf[];
//extern uint16_t LSD_ADC[];
extern char p_buf[];


LSD_Struct LSD_CMOS={
	0,
	0,
	0,
	{0},
	0,
};
/* USER CODE END 0 */

ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

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
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
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

    /* ADC1 DMA Init */
    /* ADC Init */
    hdma_adc.Instance = DMA1_Channel1;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_adc) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc);

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

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
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
	
//	HAL_ADC_Start_DMA(&hadc, (uint32_t *)My_adcData,adc_max);//因为你选择的软件触发，所以每次采集都需要开启一次
	HAL_ADC_Start_DMA(&hadc, (uint32_t *)&adcBuf, LSD_SIZE);
	static   uint16_t i;
	static uint8_t uart_cnt=0,err_cnt=0;
	for(i=0;i<LSD_SIZE;i++){                       							 //
		if(LSD_CMOS.UART_START==0 ){
			break;
		}
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6)==RESET){
			LSD_CMOS.UART_START=0;
			printf("Please turn on the laser first\r\n");
			break;
		}
		LSD_CMOS.LSD_ADC[i]=(uint16_t )adcBuf[i];
		sprintf(p_buf,"adc[%d] = %d\r\n",i,LSD_CMOS.LSD_ADC[i]);
//		printf("%s",p_buf);
		if(LSD_CMOS.LSD_ADC[i]<2300 && LSD_CMOS.LSD_START==0){
			LSD_CMOS.LSD_START=1;
//			printf("11111\r\n\r\n");
		}
		else if(LSD_CMOS.LSD_START==1 && LSD_CMOS.LSD_ADC[i]>3100){
			LSD_CMOS.LSD_START=2;
			LSD_CMOS.LSD_OFFSET=i;
//			printf("22222\r\n\r\n");
		}
		else if(LSD_CMOS.LSD_START==2 && LSD_CMOS.LSD_ADC[i]<1100){
			LSD_CMOS.LSD_VALUE=(i-LSD_CMOS.LSD_OFFSET);//*0.034;
			sprintf(p_buf,"lsd = %.2fmm\r\n",LSD_CMOS.LSD_VALUE);
			printf("%s",p_buf);
			uart_cnt++;
			if(uart_cnt>=10)
			{
				LSD_CMOS.UART_START=0;
				uart_cnt=0;
				err_cnt=0;
			}
//			LSD_CMOS.UART_START=0;
			LSD_CMOS.LSD_START=0;
			LSD_CMOS.LSD_OFFSET=0;
			break;
		}
		if(i>=LSD_SIZE-1){
			err_cnt++;
			if(err_cnt>=20){
				LSD_CMOS.UART_START=0;
				err_cnt=0;
				uart_cnt=0;
				printf("error!\r\n");
			}
//			LSD_CMOS.UART_START=0;
//			printf("Out of range\r\n");
		}
	}
	LSD_CMOS.LSD_START=0;
	LSD_CMOS.LSD_OFFSET=0;
	
}


/* USER CODE END 1 */
