/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_H__
#define __USER_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "gd32e23x.h"

#define smooth_length 100

//#define ENABLE_EP
//#define BORDER_VALUE
typedef struct{
	uint16_t F_ADC_Maximum;		//ADC����ֵ
	uint16_t F_Maximum_CNT;		//cmos����ֵ����
	uint16_t F_Maximum_Permit;	//cmos��������ֵ
	uint16_t F_Maximum_More;	//cmos����̫��
	
	uint16_t F_ADC_Valid;		//ADC��Чֵ
	uint16_t F_Valid_CNT;		//CMOS��Чֵ����
	uint16_t F_Valid_Permit;	//cmosy��Ч����ֵ
	
	uint16_t F_Valid_None_Num;		//cmos��Чֵ
	uint16_t F_Valid_None_Cnt;		//cmos��Чֵ����
	
}Filter_CNT;



/* USER CODE BEGIN Private defines */
void CMOS_CLK(void);
void CMOS_ADC(uint8_t arount);
void CMOS_DIS(uint8_t arount);

#endif //__USER_H__
