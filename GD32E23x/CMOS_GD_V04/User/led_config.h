
#ifndef __LED_CONFIG_H
#define __LED_CONFIG_H

#include "gd32e23x.h"

typedef struct{
	uint8_t led_init_flag;
	uint16_t led_start_flag;
	uint16_t led_end_flag;
	uint8_t led_result;
//	uint16_t led_
}LASER_STR;


extern LASER_STR Laser_Init;
void Laser_Calibration(void);

#endif /* __LED_CONFIG_H */
