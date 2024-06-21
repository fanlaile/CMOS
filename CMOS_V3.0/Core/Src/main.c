/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern  LSD_Struct LSD_CMOS;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint16_t adcBuf[LSD_SIZE];
char p_buf[128];
uint16_t adc_Buf;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

extern uint8_t aRxBuffer;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */


  HAL_Delay(100);


  HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);

  printf("========== c378 Test Start fan ==========\r\n\r\n");
   
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  
		UART_TEST_HANDLE();
		if(LSD_CMOS.LSD_START)
		{
			CMOS_RUN_CLOCK();
		}

//	HAL_Delay(1);
  }
  /* USER CODE END 3 */
}

/**
  * @brief CMOS RUN CP & SP
  * @retval None
  */
void CMOS_RUN_CLOCK(void)
{
	static uint8_t arount_cnt=0;
	uint16_t cnt=0,odd_cnt=0;
	float lsd_max=0,lsd_min=0xffff,lsd_sum=0;
	for(uint16_t i=0; i<4400;i++)
	{
		if(i%2)
		{
			if(i==1){
				SP_H();
				LSD_CMOS.SP_STATUS=1;
			}
			CP_L();
			adc_Buf=adc_get_result();
			if(cnt<LSD_SIZE && LSD_CMOS.LSD_INT && LSD_CMOS.SP_STATUS)
			{
				
//				LSD_CMOS.LSD_ADC[cnt++]=(adc_Buf>>4)&0xfff;
				LSD_CMOS.LSD_ADC[cnt]+=adc_Buf;
			}
			if((odd_cnt++)%2)
			{
				cnt++;
			}
		}
		else
		{
			CP_H();
			if(i==4)
			{
				SP_L();
			}
		}

	}
	for(uint16_t k=0;k<LSD_SIZE;k++)
	{
//		sprintf(p_buf,"adc[%d] = %d\r\n",k,LSD_CMOS.LSD_ADC[k]);
//		printf("%s",p_buf);
//		HAL_Delay(10);
		if(LSD_CMOS.LSD_ADC[k]>4000 &&k>0 && LSD_CMOS.CMOS_START==0)
		{
			LSD_CMOS.LSD_OFFSET=0;
			LSD_CMOS.CMOS_START=1;
		}
		if(LSD_CMOS.LSD_ADC[k]<4500 && LSD_CMOS.CMOS_START)
		{
			LSD_CMOS.CMOS_START=0;
			LSD_CMOS.LSD_VALUE[arount_cnt] = k-LSD_CMOS.LSD_OFFSET;
			printf("lsd[%d] = %.2f \r\n",arount_cnt,LSD_CMOS.LSD_VALUE[arount_cnt]);
			break;
		}
		
	}
	arount_cnt++;
	
	
//	cnt=0;
//	
//	odd_cnt=0;
//	LSD_CMOS.LSD_START=0;
	
	if(arount_cnt>=15)
	{
		for(uint8_t n=0;n<10;n++)
		{
			if(lsd_max<LSD_CMOS.LSD_VALUE[n+2]){
				lsd_max = LSD_CMOS.LSD_VALUE[n+2];
			}
			if(lsd_min>LSD_CMOS.LSD_VALUE[n+2]){
				lsd_min = LSD_CMOS.LSD_VALUE[n+2];
			}
			lsd_sum+=LSD_CMOS.LSD_VALUE[n+2];
		}
		lsd_sum=(lsd_sum-lsd_max-lsd_min)/8;
		printf("lsd = %.2f max=%.2f min=%.2f\r\n",lsd_sum,lsd_max,lsd_min);
		LSD_CMOS.LSD_START=0;
		arount_cnt=0;
	}
	
	LSD_CMOS.SP_STATUS=0;
	memset(LSD_CMOS.LSD_ADC,0x00,sizeof(LSD_CMOS.LSD_ADC));
	HAL_Delay(100);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
