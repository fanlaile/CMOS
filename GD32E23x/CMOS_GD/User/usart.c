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
uint8_t transmitter_buffer[128];
uint8_t receiver_buffer[128];
uint8_t transfersize = 128;
uint8_t receivesize = 128;
__IO uint8_t txcount = 0; 
__IO uint16_t rxcount = 0; 
uint8_t Rx_flag = 0; 
int32_t str_to_num=0;

/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd call back function                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int8_t _helpf(uint8_t* s);
int8_t _LAONf(uint8_t* s);
int8_t _LAOFf(uint8_t* s);
int8_t _DISTf(uint8_t* s);
int8_t _MEONf(uint8_t* s);
int8_t _MEOFf(uint8_t* s);
int8_t _SETPf(uint8_t* s);
int8_t _ADCf(uint8_t* s);
int8_t _RDFLf(uint8_t* s);
int8_t _WRFLf(uint8_t* s);
int8_t _SETOf(uint8_t* s);
int8_t _GETOf(uint8_t* s);
/*---------------------------------------------------------------------------------------------------------*/
/* usart cmd struct                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

extern char p_buf[];
extern LSD_Struct LSD_CMOS;

typedef struct {
	uint8_t priority;
	char *cmd;
	int8_t (*f)(uint8_t* s);
}cmd_T;

cmd_T CMD_T[] = {
	{0,	"help\r\n"							,_helpf			},
	{0,	"ADC?\r\n"							,_ADCf			},
	{0,	"laser on\r\n"					,_LAONf			},
	{0,	"laser off\r\n"					,_LAOFf			},
	{0,	"distance\r\n"					,_DISTf			},
	{0,	"measure start\r\n"			,_MEONf			},
	{0,	"measure stop\r\n"			,_MEOFf			},
	{1,	"set pwm="							,_SETPf			},
	{1,	"read flash"						,_RDFLf			},
	{1,	"write flash="					,_WRFLf			},
	{0,	"setorigin\r\n"					,_SETOf			},
	{0,	"getorigin\r\n"					,_GETOf			},
};
uint8_t cmd_num = sizeof(CMD_T)/sizeof(CMD_T[0]);


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

	usart_baudrate_set(USART0,9600U);

	usart_receive_config(USART0, USART_RECEIVE_ENABLE);

	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

	usart_interrupt_enable(USART0, USART_INT_RBNE);

	nvic_irq_enable(USART0_IRQn, 0);

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
	uint8_t i;
    if(Rx_flag)
	{
		for(i=0;i<cmd_num;i++)
		{
			if(me_strcmp(receiver_buffer,CMD_T[i].cmd ,CMD_T[i].priority)==1)
			{
				CMD_T[i].f(receiver_buffer);
				
				Rx_flag = 0;
				
				memset(receiver_buffer,0,sizeof(receiver_buffer));
				return;
			}
		}
		Rx_flag = 0;
		memset(receiver_buffer,0,sizeof(receiver_buffer));
		V_STR_Printf("COMMAND,ERROR");
	}
	
	
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
		if(receiver_buffer[rxcount]==0x0a)
		{
			Rx_flag=1;
			rxcount=0;
		}
		else
		{
			rxcount++;
		}
    }

    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)){
        /* transmit data */
        usart_data_transmit(USART0, transmitter_buffer[txcount++]);
        if(txcount == transfersize){
            usart_interrupt_disable(USART0, USART_INT_TBE);
        }
    }
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
void LASER_ON(void)
{
	uint16_t val;
	val=LSD_CMOS.LED_PWM*10;
	timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,val);
//	printf("pwm=%d\r\n",val);
}
void LASER_OFF(void)
{
	timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);
}

int8_t _helpf(uint8_t* s)
{
	printf("以下是控制指令，请根据需求发送!\r\n");
	printf("每条指令请以\\r\\n结尾!\r\n");
	printf("laser on打开激光\r\n");
	printf("laser off关闭激光\r\n");
	printf("distance距离查询\r\n");
	printf("measure start开启长测\r\n");
	printf("measure stop关闭长测\r\n");
	printf("set pwm=设置激光pwm\r\n");
	printf("lsd origin = %d\r\n",LSD_CMOS.LSD_ORIGIN);
	return 1;
}

int8_t _LAONf(uint8_t* s)
{
	uint16_t val;
	LASER_ON();
//	printf("OK\r\n");
	
	V_STR_Printf("ON,OK");
	return 1;
}
int8_t _LAOFf(uint8_t* s)
{
	LASER_OFF();
//	printf("OK\r\n");
	timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);
	V_STR_Printf("OFF,OK");
	return 1;
}
int8_t _DISTf(uint8_t* s){
	LASER_ON();
	LSD_CMOS.LSD_START=1;

	return 1;
}
int8_t _MEONf(uint8_t* s){
	LASER_ON();
	V_STR_Printf("measure start,OK");
	LSD_CMOS.CMOS_START=1;
	return 1;
}
int8_t _MEOFf(uint8_t* s){
	LASER_OFF();
	V_STR_Printf("measure stop,OK");
	LSD_CMOS.CMOS_START=0;
	return 1;
}
int8_t _SETPf(uint8_t* s){
	Get_Num(s,'=');
	if(str_to_num>100){
		str_to_num=100;
	}
	LSD_CMOS.LED_PWM=str_to_num;
//	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, str_to_num);
	sprintf(p_buf,"set pwm=%d,OK",str_to_num);
	V_STR_Printf(p_buf);
	return 1;
}
int8_t _ADCf(uint8_t* s){
	uint16_t adc_val;
	adc_val=Get_ADC_Value();
	printf("adc");
	sprintf(p_buf,"ADC=%d,OK",adc_val);
	V_STR_Printf(p_buf);
	return 1;
}
int8_t _RDFLf(uint8_t* s){
	uint32_t *data;
	uint32_t adr;
	uint8_t num;
	Get_Num(s,'h');
	num=str_to_num*4;
	adr = FMC_DATA_ADDR+num;
	FMC_FLASH_Read(data,adr);
	printf("fmc=%x",*data);
	
	
	return 1;
}
int8_t _WRFLf(uint8_t* s){
	uint32_t num;
	Get_Num(s,'=');
	num=str_to_num;
	FMC_FLASH_Write(num,FMC_DATA_ADDR);
	printf("ok\r\n");
	return 1;
}
int8_t _SETOf(uint8_t* s)
{
	LASER_ON();
	LSD_CMOS.LSD_START=1;
	LSD_CMOS.LSD_RESULT=1;
	return 1;
}
int8_t _GETOf(uint8_t* s){
	uint32_t *data;
	FMC_FLASH_Read(data,FMC_DAT1_ADDR);
	printf("ORIGIN=%d\r\n",*data);
	return 1;
}

