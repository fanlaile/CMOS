/****************************************************************************
 * @file     timer.c
 * @version  V1.00
 * @brief    control driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "systick.h"
#include "timer.h"
#include <stdio.h>
extern uint16_t adc_Buf;
extern LSD_Struct LSD_CMOS;

uint16_t index;

void timer2_config(void)		// PA6 Timer2 CH0 - Laser On Period
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
	
	rcu_periph_clock_enable(RCU_GPIOA);
	
//	rcu_periph_clock_enable(RCU_AF);
	
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);
    
    rcu_periph_clock_enable(RCU_TIMER2);
	
//		LSD_CMOS.Line_Period = 1600;//2145;  // 1288000/500hz = 
	  LSD_CMOS.Laser_Exp = 500;
	  uint16_t Laser_Start = LSD_CMOS.Line_Period - 0; // LSD_CMOS.Laser_Exp;
    timer_deinit(TIMER2);
    /* TIMER0 configuration */
//    timer_initpara.prescaler         = 14*6-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = LSD_CMOS.Line_Period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
 		timer_init(TIMER2,&timer_initpara);
		timer_prescaler_config(TIMER2, (14*4-1), TIMER_PSC_RELOAD_NOW);//设置预分频器为立即加载模式
// Set Interrupt
//    timer_master_slave_mode_config(TIMER2, TIMER_MASTER_SLAVE_MODE_ENABLE);
		timer_master_output_trigger_source_select(TIMER2, TIMER_TRI_OUT_SRC_UPDATE);

//		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP); //清除中断标志位,否则一开始就会进一次中断
    nvic_irq_enable(TIMER2_IRQn, 0);                       //设置中断优先级
 
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);          //使能更新中断 

//		timer_enable(TIMER2);
  /* CH1,CH2 and CH3 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.outputnstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2, TIMER_CH_0, &timer_ocintpara);

    /* CH1 configuration in PWM mode1,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0, Laser_Start);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_PWM0);
		
		timer_primary_output_config(TIMER2, ENABLE);       //使能输出比较
//		timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
//    timer_primary_output_config(TIMER2, ENABLE);
     /* auto-reload preload enable */
//    timer_auto_reload_shadow_enable(TIMER2);
    /* auto-reload preload enable */
    timer_enable(TIMER2);
}

void timer13_config(void)   // CP PA4 Timer 13 CH0 
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
	
  	rcu_periph_clock_enable(RCU_GPIOA);
//	
////	rcu_periph_clock_enable(RCU_AF);
//	
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_4);
  gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_4);
    
    rcu_periph_clock_enable(RCU_TIMER13);
	
    timer_deinit(TIMER13);
		timer_struct_para_init(&timer_initpara);
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 3;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 13;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER13,&timer_initpara);
		
		timer_prescaler_config(TIMER13, 3, TIMER_PSC_RELOAD_NOW);//设置预分频器为立即加载模式
		
//		timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP); //清除中断标志位,否则一开始就会进一次中断
//    nvic_irq_enable(TIMER13_IRQn, 0);                       //设置中断优先级
 
//    timer_interrupt_enable(TIMER13, TIMER_INT_UP);          //使能更新中断 

		timer_enable(TIMER13);
  /* CH1,CH2 and CH3 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.outputnstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER13,TIMER_CH_0,&timer_ocintpara);

    /* CH1 configuration in PWM mode1,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER13,TIMER_CH_0,7);
    timer_channel_output_mode_config(TIMER13,TIMER_CH_0,TIMER_OC_MODE_PWM0);
		
//		timer_primary_output_config(TIMER13, ENABLE);       //使能输出比较
//		timer_channel_output_shadow_config(TIMER13,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
    timer_primary_output_config(TIMER13, ENABLE);
     /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER13);
    /* auto-reload preload enable */
//    timer_disable(TIMER13);
}

/*
void TIMER13_IRQHandler(void){    
	static uint8_t cp_status=0;
	static uint16_t cnt=0;
	if(timer_interrupt_flag_get(TIMER13,TIMER_INT_FLAG_UP) == SET){
		timer_interrupt_flag_clear(TIMER13,TIMER_INT_FLAG_UP); //清除中断标志位   
		if((LSD_CMOS.LSD_START || LSD_CMOS.CMOS_START ) && LSD_CMOS.CMOS_TIMER==0)
		{
			if(cp_status==0)
			{
				if(cnt==0)
				{
					SP_H();
				}
				if(cnt==LSD_CMOS.EP_HI)
				{
//					EP_H();
				}
				CP_H();
//				adc_Buf=Get_ADC_Value();
				cp_status=1;
			}
			else
			{
				if(cnt>=2)
				{
					SP_L();
				}
				if(cnt==LSD_CMOS.EP_LO)
				{
//					EP_L();
				}
				CP_L();
				adc_Buf=adc_get_result();//Get_ADC_Value();
				cp_status=0;
				cnt++;
			}
			if((cnt%2) == 1 && cp_status==0 && LSD_CMOS.LSD_INT && index<LSD_SIZE)
			{
				LSD_CMOS.LSD_INT=2;
				LSD_CMOS.LSD_ADC[index++]=adc_Buf;
			}
			
			
	//		cnt++;
			if(cnt>=2150)
			{
				cnt=0;
				index=0;
				LSD_CMOS.LSD_INT=0;
				LSD_CMOS.CMOS_TIMER=1;
	//			printf("start\r\n");
			}
		}
	}
}
*/ 

void TIMER2_IRQHandler(void){    

//	timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_TRG); //清除中断标志位   
	if(timer_interrupt_flag_get(TIMER2,TIMER_INT_FLAG_UP) == SET){

		if((LSD_CMOS.LSD_START || LSD_CMOS.CMOS_START ) ) {
				uint8_t i = 0;
				while( i++ < 6) {
					SP_H();
					__NOP();
					__NOP();
					__NOP();
					SP_H();
				}
		}
		SP_L();
		SP_L();

		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP); //清除中断标志位   
	}
}
