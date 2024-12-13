/****************************************************************************
 * @file     gpio.c
 * @version  V1.00
 * @brief    control driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/

#include "gpio.h"

/*! 
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_CFGCMP);

	/*配置IO口*/
	// PA2 > EP, PA3 > SP :  EP_H(); EP_L();
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_2|GPIO_PIN_3);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2|GPIO_PIN_3);
	
	GPIO_BC(GPIOA) = GPIO_PIN_2|GPIO_PIN_3;//GPIO_BOP为置高，GPIO_BC为置低
	
	/* connect EXTI line to GPIO pin */
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN1);
/* configure EXTI line */
	exti_init(EXTI_1, EXTI_INTERRUPT, EXTI_TRIG_RISING);
	exti_interrupt_flag_clear(EXTI_1);
	nvic_irq_enable(EXTI0_1_IRQn, 1U);

}













