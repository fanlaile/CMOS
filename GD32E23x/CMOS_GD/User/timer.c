/****************************************************************************
 * @file     adc.c
 * @version  V1.00
 * @brief    control driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "systick.h"
#include "timer.h"
#include <stdio.h>




void PWM_timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
	
	rcu_periph_clock_enable(RCU_GPIOA);
	
//	rcu_periph_clock_enable(RCU_AF);
	
  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
  gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);
    
    rcu_periph_clock_enable(RCU_TIMER2);
	
	

    timer_deinit(TIMER2);
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 71;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    
		timer_init(TIMER2,&timer_initpara);
		timer_prescaler_config(TIMER2, 1, TIMER_PSC_RELOAD_NOW);//设置预分频器为立即加载模式
		timer_enable(TIMER2);
  /* CH1,CH2 and CH3 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2,TIMER_CH_0,&timer_ocintpara);

    /* CH1 configuration in PWM mode1,duty cycle 25% */
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,0);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_PWM0);
		
		timer_primary_output_config(TIMER2, ENABLE);       //使能输出比较
//		timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
//    timer_primary_output_config(TIMER2, ENABLE);
     /* auto-reload preload enable */
//    timer_auto_reload_shadow_enable(TIMER2);
    /* auto-reload preload enable */
//    timer_enable(TIMER2);
}







