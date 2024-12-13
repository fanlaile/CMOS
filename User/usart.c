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
uint8_t show_flag = 0;

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
int8_t _SHOWf(uint8_t* s);
int8_t _RPWMf(uint8_t* s);
int8_t _WPWMf(uint8_t* s);
int8_t _LAINf(uint8_t* s);
int8_t _UPDTf(uint8_t* s);
int8_t _TEMPf(uint8_t* s);
int8_t _SETEf(uint8_t* s);
int8_t _GETTf(uint8_t* s);
int8_t _AUDOf(uint8_t* s);
int8_t _SATLf(uint8_t* s);
int8_t _RATLf(uint8_t* s);
int8_t _DCALf(uint8_t* s);
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
	{0,	"laser on\r\n"						,_LAONf			},
	{0,	"laser off\r\n"						,_LAOFf			},
	{0,	"distance\r\n"						,_DISTf			},
	{0,	"measure start\r\n"					,_MEONf			},
	{0,	"measure stop\r\n"					,_MEOFf			},
	{1,	"set pwm="							,_SETPf			},
	{1,	"read flash"						,_RDFLf			},
	{1,	"write flash="						,_WRFLf			},
	{0,	"setorigin\r\n"						,_SETOf			},
	{0,	"getorigin\r\n"						,_GETOf			},
	{0,	"show wave\r\n"						,_SHOWf			},
	{0,	"read pwm\r\n"						,_RPWMf			},
	{1,	"write pwm="						,_WPWMf			},
	{0,	"laser init\r\n"					,_LAINf			},
	{0,	"app updata\r\n"					,_UPDTf			},
	{0,	"temp?\r\n"							,_TEMPf			},
	{0,	"get temp\r\n"						,_GETTf			},
	{1,	"EP="								,_SETEf			},
	{0,	"auto-dodge\r\n"					,_AUDOf			},
	{1,	"set autoreload="					,_SATLf			},
	{0,	"read autoreload\r\n"				,_RATLf			},
	{0,	"disable calibration\r\n"			,_DCALf			},
	
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
 *+
 * @details  指令处理回调函数
 *
 */
void TIM2_Reinit(void)
{
	LASER_ON();
	timer_autoreload_value_config(TIMER2,LSD_CMOS.Line_Period);
}
void CMOS_work(void)
{
	if( LSD_CMOS.CMOS_START == 0) {
		LSD_CMOS.CMOS_START=1;
		LASER_ON();						// SP, Laser ON
//		ADC_Init();
//		delay_1ms(100);
//		PWM_timer_config();	// SP
		timer_enable(TIMER13); // CP ON
		ADC_Start_DMA( (uint16_t *)&LSD_CMOS.LSD_ADC[LSD_CMOS.pWd], LSD_SIZE);
		delay_1ms(10);
	}
}
void CMOS_turnoff(void)
{
	LASER_OFF();
	timer_disable(TIMER13);
	ADC_Stop_DMA();
	LSD_CMOS.CMOS_START=0;
}
void LASER_ON(void)
{
	float val_p;
	val_p=(float)LSD_CMOS.LED_PWM/100;
	LSD_CMOS.Laser_Exp =LSD_CMOS.Line_Period*val_p;
	uint16_t val = LSD_CMOS.Line_Period - LSD_CMOS.Laser_Exp;
//	printf("PWM = %d val_p = %.2f ;;; Laser_Exp = %d ;;; val = %d\r\n",LSD_CMOS.LED_PWM,val_p,LSD_CMOS.Laser_Exp,val);
	timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,val);
	timer_enable(TIMER2);
//	ADC_Start_DMA( (uint16_t *)&LSD_CMOS.LSD_ADC[0], LSD_SIZE);
//	printf("pwm=%d\r\n",val);
}

void LASER_OFF(void)
{
//	timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);
//	LSD_CMOS.Laser_Exp = 0;
	uint16_t val = LSD_CMOS.Line_Period - 0; // LSD_CMOS.Laser_Exp;
	timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,val);
//	timer_enable(TIMER2);
	timer_disable(TIMER2);
//	ADC_Stop_DMA();
}

//	"help\r\n" ,_helpf
int8_t _helpf(uint8_t* s)
{
	printf("vision:CMOS_GD_TD_V65\r\n");
	printf("以下是控制指令，请根据需求发送!\r\n");
	printf("每条指令请以\\r\\n结尾!\r\n");
	
	printf("laser on打开激光\r\n");
	printf("laser off关闭激光\r\n");
	printf("distance距离查询\r\n");
	printf("measure start开启长测\r\n");
	printf("measure stop关闭长测\r\n");
	printf("set pwm=设置激光pwm\r\n");
	printf("read pwm读取激光pwm\r\n");
	printf("setorigin设置原点\r\n");
	printf("getorigin读取原点\r\n");
	printf("show wave读取波形\r\n");
//	printf("laser init激光pwm自动调节\r\n");//laser init
	printf("app updata进入固件升级\r\n");//app updata
	printf("flash origin = %d\r\n",LSD_CMOS.LSD_ORIGIN);
	printf("flash pwm = %d\r\n",LSD_CMOS.LED_PWM);
	printf("get temp获取温度\r\n");
	printf("set autoreload=设置定时器重装载");
	printf("read autoreload读取定时器重装载");
	return 1;
}
//
//	"laser on\r\n",_LAONf
//
int8_t _LAONf(uint8_t* s)
{
	uint16_t val;
	LASER_ON();
//	printf("OK\r\n");	
	V_STR_Printf("ON,OK");
	return 1;
}

//
// "laser off\r\n"	,_LAOFf
//
int8_t _LAOFf(uint8_t* s)
{
	LASER_OFF();
	timer_disable(TIMER13);
	LSD_CMOS.LSD_START=0;
	LSD_CMOS.CMOS_START=0;
//	printf("OK\r\n");
//	timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);
	V_STR_Printf("OFF,OK");
	return 1;
}

//
// "distance\r\n",_DISTf
//
int8_t _DISTf(uint8_t* s){
	
	LSD_CMOS.LSD_START=1;
	CMOS_work();
	return 1;
}
//
// "measure start\r\n",_MEONf
//
int8_t _MEONf(uint8_t* s){

	CMOS_work();
	return 1;
}

//
//"measure stop\r\n",_MEOFf
//
int8_t _MEOFf(uint8_t* s){
	CMOS_turnoff();
	V_STR_Printf("measure stop,OK");
	return 1;
}

//
// "set pwm="							,_SETPf			},
//
int8_t _SETPf(uint8_t* s){
	Get_Num(s,'=');
	if(str_to_num>100){
		str_to_num=100;
	}
	LSD_CMOS.LED_PWM=str_to_num;
//	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, str_to_num);
	fmc_str.fmc_buffer[DATA_PWM]=LSD_CMOS.LED_PWM;
	FMC_WRITE_BUFFER(FMC_START_ADDR, fmc_str.fmc_buffer,6);//初始化标志
	sprintf(p_buf,"set pwm=%d,OK",str_to_num);
	V_STR_Printf(p_buf);
	return 1;
}

//
// "ADC?\r\n"							,_ADCf			},
//
int8_t _ADCf(uint8_t* s){
	uint16_t adc_val;
	adc_val=Get_ADC_Value();
	printf("adc");
	sprintf(p_buf,"ADC=%d,OK",adc_val);
	V_STR_Printf(p_buf);
	return 1;
}

//
// "read flash",_RDFLf
//
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

//
//	"write flash=",_WRFLf
//
int8_t _WRFLf(uint8_t* s){
	uint32_t num;
	Get_Num(s,'=');
	num=str_to_num;
	FMC_FLASH_Write(num, FMC_DATA_ADDR);
	printf("ok\r\n");
	return 1;
}

//	"setorigin\r\n",_SETOf
int8_t _SETOf(uint8_t* s)
{
	LASER_ON();
	LSD_CMOS.LSD_START=1;
	LSD_CMOS.LSD_RESULT=1;
	return 1;
}

//	"getorigin\r\n",_GETOf,
int8_t _GETOf(uint8_t* s){
	uint32_t *data;
	FMC_FLASH_Read(data,FMC_DAT1_ADDR);
	printf("ORIGIN=%d\r\n",*data);
	return 1;
}

//
//	"show wave\r\n",_SHOW
//
int8_t _SHOWf(uint8_t* s){
	printf("show wave\r\n");
	show_flag=1;
	CMOS_work();
	return 1;
}
//
//	"read pwm\r\n"					,_RPWMf			},
//
int8_t _RPWMf(uint8_t* s){
//	uint32_t *data;
//	FMC_FLASH_Read(data,FMC_DAT2_ADDR);
	printf("pwm=%d\r\n",LSD_CMOS.LED_PWM);
	return 1;
}
//
// "write pwm="						,_WPWMf			},
//
int8_t _WPWMf(uint8_t* s){
	uint32_t num;
	Get_Num(s,'=');
	LSD_CMOS.LED_PWM=str_to_num;
	fmc_str.fmc_buffer[DATA_PWM]=LSD_CMOS.LED_PWM;
	FMC_WRITE_BUFFER(FMC_START_ADDR, fmc_str.fmc_buffer,6);//初始化标志
	printf("ok\r\n");
	return 1;
}
//
//	"laser init\r\n"				,_LAINf			},
//
int8_t _LAINf(uint8_t* s){
	LASER_ON();
	Laser_Init.led_init_flag=1;
	LSD_CMOS.LSD_START=1;
	printf("laser init start\r\n");
	return 1;
}
//
//"app updata\r\n"				,_UPDTf			},
//
int8_t _UPDTf(uint8_t* s){
	timer_disable(TIMER13);
	timer_disable(TIMER2);
	FMC_ERASE_PAGE(FMC_START_ADDR);
	Execute_user_Program();
	return 1;
}
//
//	"temp?\r\n"							,_TEMPf			},
//
int8_t _TEMPf(uint8_t* s){
	uint16_t temp;
	temp = Get_Temp();
	printf("temp = %d\r\n",temp);
	return 1;
}

//
// "EP="										,_SETEf			},
//
int8_t _SETEf(uint8_t* s)
{
	Get_Num(s,'=');
	if(str_to_num==0)
	{
		EP_L();
	}
	else
	{
		
		EP_H();
	}
	printf("OK\r\n");
	return 1;
}

//
//	"get temp\r\n"					,_GETTf			},
//
int8_t _GETTf(uint8_t* s){
	double temp;
	temp=readtemp();
//	printf("%0.4lf",temp);
	sprintf(p_buf,"temp,OK,%.4lf",temp);
	V_STR_Printf(p_buf);
	return 1;
}

//
// "auto-dodge\r\n"				,_AUDOf			},
//
int8_t _AUDOf(uint8_t* s){
	LSD_CMOS.LSD_AUTO=1;
	sprintf(p_buf,"auto-dodge,OK");
	V_STR_Printf(p_buf);
	return 1;
}
int8_t _SATLf(uint8_t* s){
	Get_Num(s,'=');
	
	if(str_to_num<1600)
	{
		str_to_num=1600;
	}
	if(str_to_num>2600)
	{
		str_to_num=2600;
	}
	printf("timer2 autoreload = %d\r\n",str_to_num);
	LSD_CMOS.Line_Period = str_to_num;
	fmc_str.fmc_buffer[DATA_TIM2_P]=LSD_CMOS.Line_Period;
	FMC_WRITE_BUFFER(FMC_START_ADDR, fmc_str.fmc_buffer,6);//初始化标志
	timer_autoreload_value_config(TIMER2,LSD_CMOS.Line_Period);
	return 1;
}
int8_t _RATLf(uint8_t* s){
//	uint32_t *data;
//	FMC_FLASH_Read(data,FMC_DAT3_ADDR);
	printf("TIM2 autoreload=%d\r\n",LSD_CMOS.Line_Period);
	return 1;
}
int8_t _DCALf(uint8_t* s)
{
	LSD_CMOS.LSD_AUTO=0;
	sprintf(p_buf,"disable calibration,OK");
	V_STR_Printf(p_buf);
	return 1;
}
/*
//	"EP_high="							,_EPHIf			},
int8_t _EPHIf(uint8_t* s){
	Get_Num(s,'=');
	LSD_CMOS.EP_HI=str_to_num;
	if(LSD_CMOS.EP_HI>2150){
		LSD_CMOS.EP_HI=2150;
	}
	printf("EP_HI=%d\r\n",LSD_CMOS.EP_HI);
	return 1;
}
//	"EP_low="								,_EPLOf			},	
int8_t _EPLOf(uint8_t* s){
	Get_Num(s,'=');
	LSD_CMOS.EP_LO=str_to_num;
	if(LSD_CMOS.EP_LO>2150){
		LSD_CMOS.EP_LO=2150;
	}
	printf("EP_LO=%d\r\n",LSD_CMOS.EP_LO);
	return 1;
}
*/
