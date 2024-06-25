/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "user.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 

extern LSD_Struct LSD_CMOS;
extern uint16_t adc_Buf;
extern char p_buf[];

/**
  * @brief CMOS RUN CP & SP
  * @retval None
  */
void CMOS_CLK(void){
	
	uint16_t cnt=0,odd_cnt=0;
	
	for(uint16_t i=0; i<4400;i++)
	{
		if(i%2)
		{
			if(i==1){
				SP_H();
				LSD_CMOS.SP_STATUS=1;
			}
			CP_L();
			adc_Buf=adc_get_result();
			if(cnt<LSD_SIZE && LSD_CMOS.LSD_INT && LSD_CMOS.SP_STATUS)
			{
				
//				LSD_CMOS.LSD_ADC[cnt++]=(adc_Buf>>4)&0xfff;
				LSD_CMOS.LSD_ADC[cnt]+=adc_Buf;
				
			}
			if((odd_cnt++)%2)
			{
				cnt++;
			}
		}
		else
		{
			CP_H();
			if(i==4)
			{
				SP_L();
			}
		}
	}
}

/**
  * @brief CMOS ADC
  * @retval None
  */
void CMOS_ADC(uint8_t arount){
	
	
	for(uint16_t k=0;k<LSD_SIZE;k++)
	{
//		sprintf(p_buf,"adc[%d] = %d\r\n",k,LSD_CMOS.LSD_ADC[k]);
//		printf("%s",p_buf);
//		HAL_Delay(10);
		if(LSD_CMOS.LSD_ADC[k]>4000 &&k>0 && LSD_CMOS.CMOS_START==0)
		{
			LSD_CMOS.LSD_OFFSET=0;
			LSD_CMOS.CMOS_START=1;
		}
		if(LSD_CMOS.LSD_ADC[k]<4500 && LSD_CMOS.CMOS_START)
		{
			LSD_CMOS.CMOS_START=0;
			LSD_CMOS.LSD_VALUE[arount] = k-LSD_CMOS.LSD_OFFSET;

//			printf("lsd[%d] = %.2f \r\n",arount_cnt,LSD_CMOS.LSD_VALUE[arount_cnt]);
			break;
		}
	}
}

/**
  * @brief CMOS DIS
  * @retval None
  */
void CMOS_DIS(uint8_t arount){
	float lsd_max=0,lsd_min=0xffff,lsd_sum=0;
	
	for(uint8_t n=0;n<10;n++)
		{
			if(lsd_max<LSD_CMOS.LSD_VALUE[n+3]){
				lsd_max = LSD_CMOS.LSD_VALUE[n+3];
			}
			if(lsd_min>LSD_CMOS.LSD_VALUE[n+3]){
				lsd_min = LSD_CMOS.LSD_VALUE[n+3];
			}
			lsd_sum+=LSD_CMOS.LSD_VALUE[n+3];
		}
		lsd_sum=((lsd_sum-lsd_max-lsd_min)/8) - 50;
		if(lsd_sum>=0 && lsd_sum<800)
		{
			sprintf(p_buf,"DISTANCE,OK,%.2f",lsd_sum);
			V_STR_Printf(p_buf);
		}
		else{
			sprintf(p_buf,"DISTANCE,ERROR,0");
			V_STR_Printf(p_buf);
		}
//		printf("lsd = %.2f max=%.2f min=%.2f\r\n",lsd_sum,lsd_max,lsd_min);
//		sprintf(p_buf,"DISTANCE,OK,%.2f",lsd_sum);
//		V_STR_Printf(p_buf);
}




