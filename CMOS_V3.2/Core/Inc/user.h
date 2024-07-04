/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_H__
#define __USER_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define smooth_length 50

/* USER CODE BEGIN Private defines */
void CMOS_CLK(void);
void CMOS_ADC(uint8_t arount);
void CMOS_DIS(uint8_t arount);

#endif //__USER_H__
