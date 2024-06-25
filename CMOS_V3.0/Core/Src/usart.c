/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "adc.h"
#include "gpio.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
extern uint16_t adcBuf[];
extern char p_buf[];
extern LSD_Struct LSD_CMOS;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;

 
uint8_t aRxBuffer;

int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
    while(HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,0xffff) != HAL_OK);
	return 0;
}
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t transmitter_buffer[128];
uint8_t receiver_buffer[128];
uint8_t transfersize = 128;
uint8_t receivesize = 128;
uint8_t txcount = 0; 
uint16_t rxcount = 0; 
uint8_t Rx_flag = 0; 
int32_t str_to_num;

/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd call back function                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int8_t _HELPf(uint8_t* s);
int8_t _LAONf(uint8_t* s);
int8_t _LAOFf(uint8_t* s);
int8_t _DISTf(uint8_t* s);

/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd struct                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct {
	uint8_t priority;
	char *cmd;
	int8_t (*f)(uint8_t* s);
}cmd_T;

cmd_T CMD_T[] = {
	{0,	"help\r\n"			,_HELPf			},
	{0,	"laser on\r\n"		,_LAONf			},
	{0,	"laser off\r\n"		,_LAOFf			},
	{0,	"distance\r\n"		,_DISTf			},

};
uint8_t cmd_num = sizeof(CMD_T)/sizeof(CMD_T[0]);
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
 * @brief    字符串比较函数
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
int8_t me_strcmp(uint8_t *str1, char *str2, uint8_t mode)
{
	uint8_t i,len,result=1;
	
	
	if(mode == 0)
		len = strlen((char*)str1);
	else
		len = strlen((char*)str2);
	
	for(i=0;i<len;i++)
	{
		if(str1[i]!=str2[i])
		{
			result = 0;
		}
	}
	return result;
}
/**
 * @brief    串口中断
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	

		if(rxcount >= 255)  //溢出判断
		{
			rxcount = 0;
			memset(receiver_buffer,0x00,sizeof(receiver_buffer));
//			HAL_UART_Transmit(&huart1, (uint8_t *)&transmitter_buffer, sizeof(transmitter_buffer),0xFFFF);	
		}
		else
		{
			receiver_buffer[rxcount++] = aRxBuffer;   //接收数据转存
		
			if((receiver_buffer[rxcount-1] == 0x0A)&&(receiver_buffer[rxcount-2] == 0x0D)) //判断结束位
			{
				Rx_flag=1;
//				rxcount = 0;
//				memset(receiver_buffer,0x00,sizeof(receiver_buffer)); //6清空数组
			}
		}

		HAL_UART_Receive_IT(&huart1,(uint8_t *)&aRxBuffer,1);
		
	
}

/**
 * @brief    Uart command 
 *
 * @param    None
 *
 * @returns  None
 *
 * @details  串口命令处理函数
 *
 */
void UART_TEST_HANDLE(void)
{
	uint8_t i;
  if(Rx_flag)
	{
		for(i=0;i<cmd_num;i++)
		{
			if(me_strcmp(receiver_buffer,CMD_T[i].cmd ,CMD_T[i].priority)==1)
			{
				CMD_T[i].f(receiver_buffer);
				
				Rx_flag = 0;
				rxcount = 0;
				memset(receiver_buffer,0,sizeof(receiver_buffer));
				return;
			}
		}
		Rx_flag = 0;
		rxcount = 0;
		memset(receiver_buffer,0,sizeof(receiver_buffer));
		V_STR_Printf("COMMAND,ERROR");
	}
}
/**
 * @brief    print verify
 *
 * @param    None
 *
 * @returns  None 
 *
 * @details  发送带检验
 *
 */
void V_STR_Printf(char *STR)
{
	uint8_t i=1;
	uint8_t result=STR[0];
	while(STR[i]!=NULL )
	{
		result^=STR[i];
		i++;
	}
	printf("$%s|%x~",STR,result);
}

/**
 * @brief    UART Callback function 
 *
 * @param    None
 *
 * @returns  None 
 *
 * @details  指令处理回调函数
 *
 */
int8_t _HELPf(uint8_t* s)
{
	printf("以下是控制指令，请根据需求发送!\r\n");
	printf("每条指令请以\\r\\n结尾!\r\n");
	printf("laser on打开激光\r\n");
	printf("laser off关闭激光\r\n");
	printf("distance?距离查询\r\n");
	return 1;
}

int8_t _LAONf(uint8_t* s)
{
	LASER_ON();
//	printf("OK\r\n");
	V_STR_Printf("ON,OK");
	return 1;
}
int8_t _LAOFf(uint8_t* s)
{
	LASER_OFF();
//	printf("OK\r\n");
	V_STR_Printf("OFF,OK");
	return 1;
}
int8_t _DISTf(uint8_t* s){
	LSD_CMOS.LSD_START=1;
	return 1;
}
/* USER CODE END 1 */
