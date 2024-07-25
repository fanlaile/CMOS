/****************************************************************************
 * @file     uart_debug.c
 * @version  V1.00
 * @brief    uart command source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t transmitter_buffer[256];
uint8_t receiver_buffer[256];
uint8_t transfersize = 255;
uint8_t receivesize = 255;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 
uint8_t Rx_flag = 0; 
int32_t str_to_num=30;

/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd call back function                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd struct                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/


/**
  * @brief  Print a string on the HyperTerminal
  * @param  p_string: The string to be printed
  * @retval None
  */
void Serial_PutString(uint8_t *p_string)
{
  printf("%s",p_string);
}

/**
 * @brief    retarget the C library printf function to the USART
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
/**
 * @brief    串口IO初始化
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void Usart_GPIO_Init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
}
/**
 * @brief    串口初始化
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */
void Usart_Init(void)
{
	/* 使能USART时钟*/
 
	rcu_periph_clock_enable(RCU_USART0);

	/* USART 配置*/

	usart_deinit(USART0);

	usart_word_length_set(USART0, USART_WL_8BIT);
	usart_stop_bit_set(USART0, USART_STB_1BIT);
	usart_parity_config(USART0, USART_PM_NONE);

	usart_baudrate_set(USART0,9600);

	usart_receive_config(USART0, USART_RECEIVE_ENABLE);

	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

//	usart_interrupt_enable(USART0, USART_INT_RBNE);

//	nvic_irq_enable(USART0_IRQn, 0);

	usart_enable(USART0);
	
	
}

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
	
	
}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void USART0_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){
        /* receive data */
		receiver_buffer[rxcount] = usart_data_receive(USART0);
//        if(Rxcount == receivesize){
//            usart_interrupt_disable(USART0, USART_INT_RBNE);
//        }
		
	}

//    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)){
//        /* transmit data */
//			if(Rx_flag){
//        usart_data_transmit(USART0, receiver_buffer[txcount++]);
//			}
//        if(txcount == transfersize){
//            usart_interrupt_disable(USART0, USART_INT_TBE);
//					Rx_flag=0;
//        }
//    }
}
/**
 * @brief    从字符串中提取数字 
 *
 * @param    None
 *
 * @returns  None 
 *
 * @details  
 *
 */
void Get_Num(uint8_t *str,char ch)
{
	
	char *s;
	str_to_num=0;
	
	if(strchr((char*)str,ch) != NULL)
	{
		s = strchr((char*)str,ch)+1;
//		printf("str = %s",s);
		str_to_num=atoi(s);
	}

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



