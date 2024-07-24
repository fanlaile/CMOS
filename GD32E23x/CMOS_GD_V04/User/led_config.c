#include "led_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
extern LSD_Struct LSD_CMOS;

LASER_STR Laser_Init={
	0,
	0,
	0,
	0,
};


void Laser_Calibration(void)
{
	if(Laser_Init.led_result)
	{
		printf("laser init ok , pwm = %d\r\n",LSD_CMOS.LED_PWM);
		Laser_Init.led_init_flag=0;
		Laser_Init.led_result=0;
	}
	else
	{

		LSD_CMOS.LSD_START=1;
	}
}






















