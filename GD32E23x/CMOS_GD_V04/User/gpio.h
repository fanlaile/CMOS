/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gd32e23x.h"



/* USER CODE BEGIN Private defines */

#define EP_H()	gpio_bit_write(GPIOA,GPIO_PIN_2,1)
#define EP_L()	gpio_bit_write(GPIOA,GPIO_PIN_2,0)
#define SP_H()	gpio_bit_write(GPIOA,GPIO_PIN_3,1)
#define SP_L()	gpio_bit_write(GPIOA,GPIO_PIN_3,0)
#define CP_H()	gpio_bit_write(GPIOA,GPIO_PIN_4,1)
#define CP_L()	gpio_bit_write(GPIOA,GPIO_PIN_4,0)

//#define LASER_ON()	gpio_bit_write(GPIOA,GPIO_PIN_6,1)
//#define LASER_OFF()	gpio_bit_write(GPIOA,GPIO_PIN_6,0)




void gpio_config(void);

#endif //__USER_H__
