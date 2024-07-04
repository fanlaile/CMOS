/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "user.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
uint16_t cnt=0;
extern uint8_t arount_cnt;

extern LSD_Struct LSD_CMOS;
extern uint16_t adc_Buf;
extern char p_buf[];
uint16_t cmos_min_val=0xffff;
uint16_t cmos_min_index=0;
//uint16_t curve[64];
uint16_t lsd_index=0;
/**
  * @brief CMOS RUN CP & SP
  * @retval None
  */
void CMOS_CLK(void){
	
	
	for(uint16_t i=0; i<4244;i++){
		if(i%2){
			if(i==1){
				SP_H();
				LSD_CMOS.SP_STATUS=1;
			}
			CP_L();
			adc_Buf=adc_get_result();
			
				
			if(cnt<LSD_SIZE && LSD_CMOS.LSD_INT==2 && LSD_CMOS.SP_STATUS){
				
				LSD_CMOS.LSD_ADC[cnt/2]+=adc_Buf;
				cnt++;
			}
			
			if(LSD_CMOS.LSD_INT==1)
			{
				LSD_CMOS.LSD_INT=2;
			}
		}
		else{
			CP_H();
			if(i==4){
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
	int left_sum=0,right_sum=0,abs_val=0x7FFFFFFF;
	int abs_minn=0x7FFFFFFF;

	for(uint16_t i=10;i<(LSD_SIZE/2)-20;i++){
		for(uint16_t m=1;m<7;m++){
			LSD_CMOS.LSD_ADC[i]+=LSD_CMOS.LSD_ADC[i+m];
		}
		LSD_CMOS.LSD_ADC[i]/=7;
		
	}
	for(uint16_t i=10;i<(LSD_SIZE/2)-20;i++){
		for(uint16_t m=1;m<7;m++){
			LSD_CMOS.LSD_ADC[i]+=LSD_CMOS.LSD_ADC[i+m];
		}
		LSD_CMOS.LSD_ADC[i]/=7;
		
	}
	for(uint16_t i=10;i<(LSD_SIZE/2)-20;i++){
		for(uint16_t m=1;m<7;m++){
			LSD_CMOS.LSD_ADC[i]+=LSD_CMOS.LSD_ADC[i+m];
		}
		LSD_CMOS.LSD_ADC[i]/=7;
		
	}
	
	
	for(uint16_t i=10;i<(LSD_SIZE/2)-100;i++){
//		if(arount_cnt==3){
//		  printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
//		}

		for(uint16_t m=0;m<smooth_length;m++){
			left_sum+=LSD_CMOS.LSD_ADC[i+m];
			right_sum+=LSD_CMOS.LSD_ADC[i+m+smooth_length];
		}
		abs_val=abs(left_sum-right_sum);

		if((LSD_CMOS.LSD_ADC[i]>(LSD_CMOS.LSD_ADC[i+smooth_length-1])+500))
		{
			if(abs_minn>abs_val){
				lsd_index=i;
				LSD_CMOS.LSD_VALUE[arount_cnt]=i;
				abs_minn=abs_val;
			}
		}
	}		

//	printf("index = %.1f\r\n",LSD_CMOS.LSD_VALUE[arount_cnt]);
}
//冒泡排序 
void BubbleSort(float* arr, uint8_t  n)
{
	uint8_t end = n;
	while (end)
	{
		int flag = 0;
		for (int i = 1; i < end; ++i)
		{
			if (arr[i - 1] > arr[i])
			{
				uint16_t tem = arr[i];
				arr[i] = arr[i - 1];
				arr[i - 1] = tem;
				flag = 1;
			}
		}
		if (flag == 0)
		{
			break;
		}
		--end;
	}
}

void InsertSort(float* a, int n)
{
	for (int i = 0; i < n-1; i++)
	{
		int end = i;
		int tmp = a[end + 1];

		while (end >= 0)
		{
			if (tmp < a[end])
			{
				a[end + 1] = a[end];
				end--;
			}
			else
			{
				break;//找到正确顺序的插入位置
			}
		}
		a[end + 1] = tmp;
	}
}


/**
  * @brief CMOS DIS
  * @retval None
  */
void CMOS_DIS(uint8_t arount){
	float lsd_sum=0;
	
//	InsertSort(LSD_CMOS.LSD_VALUE,5);
//	printf("%.1f,%.1f,%.1f,%.1f,%.1f",LSD_CMOS.LSD_VALUE[0],LSD_CMOS.LSD_VALUE[1],LSD_CMOS.LSD_VALUE[2],LSD_CMOS.LSD_VALUE[3],LSD_CMOS.LSD_VALUE[4]);
	lsd_sum=(LSD_CMOS.LSD_VALUE[1]+LSD_CMOS.LSD_VALUE[2]+LSD_CMOS.LSD_VALUE[3]+LSD_CMOS.LSD_VALUE[4])/4;
	lsd_sum=LSD_CMOS.LSD_VALUE[2];
	lsd_sum=lsd_sum-10;
	if(lsd_sum>0 && lsd_sum<900)
	{
			sprintf(p_buf,"DISTANCE,OK,%.2f",lsd_sum);
			V_STR_Printf(p_buf);
	}
	else{
			sprintf(p_buf,"DISTANCE,ERROR,0");
			V_STR_Printf(p_buf);
	}

}




