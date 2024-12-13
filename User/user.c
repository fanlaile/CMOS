/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "user.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h> 
#include "systick.h"
uint16_t cnt=0;
extern uint8_t arount_cnt;
extern uint8_t show_flag;
extern LSD_Struct LSD_CMOS;
extern uint16_t adc_Buf;
extern char p_buf[];
uint16_t adc_Buf_fake;
uint16_t cmos_min_val=0xffff;
uint16_t cmos_min_index=0;

Filter_CNT filter_cnt={
	4000,
	0,
	3,
	200,
	
	2400,
	0,
	3,
	
	1600,
	0,
};

/**
  * @brief CMOS RUN CP & SP
  * @retval None
  */
void LSD_Calibration(void){
	if(filter_cnt.F_Valid_None_Cnt == 0 )
	{
		LSD_CMOS.LED_PWM+=20;
		LSD_CMOS.Line_Period+=200;
		if(LSD_CMOS.LED_PWM>100)
		{
			LSD_CMOS.LED_PWM=100;
		}
		if(LSD_CMOS.Line_Period>2600)
		{
			LSD_CMOS.Line_Period=2600;
		}
		if(LSD_CMOS.LED_PWM==100 && LSD_CMOS.Line_Period==2600)
		{
//				LSD_CMOS.LSD_AUTO=0;
			
			if(LSD_CMOS.LSD_ERR_CNT>=5)
			{
				sprintf(p_buf,"DISTANCE,ERROR,0");
				V_STR_Printf(p_buf);
			}
			else
			{
				LSD_CMOS.LSD_ERR_CNT++;
			}
		}
//			printf("pwm = %d ;;; autoreload = %d\r\n",LSD_CMOS.LED_PWM,LSD_CMOS.Line_Period);
		filter_cnt.F_Valid_None_Cnt=0;
		filter_cnt.F_Maximum_CNT=0;
		filter_cnt.F_Valid_CNT=0;
		TIM2_Reinit();
		return ;
	}
	else if(filter_cnt.F_Maximum_CNT > filter_cnt.F_Maximum_More)
	{
		LSD_CMOS.LED_PWM = 5;
		LSD_CMOS.Line_Period = 2000;
//			printf("pwm = %d ;;; autoreload = %d\r\n",LSD_CMOS.LED_PWM,LSD_CMOS.Line_Period);
		filter_cnt.F_Valid_None_Cnt=0;
		filter_cnt.F_Maximum_CNT=0;
		filter_cnt.F_Valid_CNT=0;
		TIM2_Reinit();
		return ;
	}
	
	else if(filter_cnt.F_Valid_CNT < filter_cnt.F_Valid_Permit )
	{
		LSD_CMOS.LED_PWM+=1;
		LSD_CMOS.Line_Period+=10;
		if(LSD_CMOS.LED_PWM>100)
		{
			LSD_CMOS.LED_PWM=100;
		}
		if(LSD_CMOS.Line_Period>2600)
		{
			LSD_CMOS.Line_Period=2600;
		}
		if(LSD_CMOS.LED_PWM==100 && LSD_CMOS.Line_Period==2600)
		{
//				LSD_CMOS.LSD_AUTO=0;
			LSD_CMOS.LSD_ERR_CNT++;
			if(LSD_CMOS.LSD_ERR_CNT>=5)
			{
				sprintf(p_buf,"DISTANCE,ERROR,0");
				V_STR_Printf(p_buf);
			}
		}
//			printf("pwm = %d ;;; autoreload = %d\r\n",LSD_CMOS.LED_PWM,LSD_CMOS.Line_Period);
		filter_cnt.F_Valid_None_Cnt=0;
		filter_cnt.F_Maximum_CNT=0;
		filter_cnt.F_Valid_CNT=0;
		TIM2_Reinit();
		return ;
	}
	else if(filter_cnt.F_Maximum_CNT > filter_cnt.F_Maximum_Permit)
	{
		LSD_CMOS.LED_PWM-=1;
		LSD_CMOS.Line_Period-=10;
		if(LSD_CMOS.LED_PWM<1)
		{
			LSD_CMOS.LED_PWM=1;
		}
		if(LSD_CMOS.Line_Period<1600)
		{
			LSD_CMOS.Line_Period=1600;
		}
		if(LSD_CMOS.LED_PWM==1 && LSD_CMOS.Line_Period==1600)
		{
//				LSD_CMOS.LSD_AUTO=0;
			LSD_CMOS.LSD_ERR_CNT++;
			if(LSD_CMOS.LSD_ERR_CNT>=5)
			{
				sprintf(p_buf,"DISTANCE,ERROR,0");
				V_STR_Printf(p_buf);
			}
		}
//			printf("pwm = %d ;;; autoreload = %d\r\n",LSD_CMOS.LED_PWM,LSD_CMOS.Line_Period);
		filter_cnt.F_Valid_None_Cnt=0;
		filter_cnt.F_Maximum_CNT=0;
		filter_cnt.F_Valid_CNT=0;
		TIM2_Reinit();
		return ;
	}
	
	else
	{
		LSD_CMOS.LSD_ERR_CNT=0;
//			LSD_CMOS.LSD_AUTO=0;
//			printf("pwm = %d ;;; autoreload = %d\r\n",LSD_CMOS.LED_PWM,LSD_CMOS.Line_Period);
	}
}

/**
  * @brief CMOS ADC
  * @retval None
  */
void CMOS_ADC(uint8_t arount){
	int sum_count[14]={0};
	int sum_max=0,sum_min=1048575;
	uint8_t hun_index=0, ten_index=0, uni_index=0;
	
//	if(arount_cnt!=1)
//	{
//		return;
//	}
	
	if(show_flag==1)
	{	//打印波形
		for(uint16_t i=80; i<LSD_SIZE-70; i++)
		{
			printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
		}
		CMOS_turnoff();
		show_flag=0;
		return ;
	}
	for(uint16_t i=0; i<14; i++)
	{
		for(uint16_t m=0; m<100; m++)
		{
			sum_count[i]+=LSD_CMOS.LSD_ADC[(m+80)+(i*100)];
			if(LSD_CMOS.LSD_ADC[(m+80)+(i*100)] >= filter_cnt.F_ADC_Maximum)
			{
				filter_cnt.F_Maximum_CNT++;
			}
			if(LSD_CMOS.LSD_ADC[(m+80)+(i*100)] >= filter_cnt.F_ADC_Valid)
			{
				filter_cnt.F_Valid_CNT++;
			}
			if(LSD_CMOS.LSD_ADC[(m+80)+(i*100)] >= filter_cnt.F_Valid_None_Num)
			{
				filter_cnt.F_Valid_None_Cnt++;
			}
//			if(sum_max<LSD_CMOS.LSD_ADC[(m+80)+(i*100)])
//			{
//				sum_max=LSD_CMOS.LSD_ADC[(m+80)+(i*100)];
//				hun_index=i;
//			}
//			if(sum_min>LSD_CMOS.LSD_ADC[(m+80)+(i*100)])
//			{
//				sum_min=LSD_CMOS.LSD_ADC[(m+80)+(i*100)];
//	//			printf("sum_min = %d \r\n",sum_min/100);
//			}
			
		}
		if(sum_max<sum_count[i])
		{
			sum_max=sum_count[i];
			hun_index=i;
		}
		if(sum_min>sum_count[i])
		{
			sum_min=sum_count[i];
//			printf("sum_min = %d \r\n",sum_min/100);
		}
	}
	//TIM2_Reinit()
	
	if(LSD_CMOS.LSD_AUTO)
	{
		LSD_Calibration();
	}
	if(LSD_CMOS.LSD_ERR_CNT>=5)
	{
		return;
	}
	filter_cnt.F_Valid_None_Cnt=0;
	filter_cnt.F_Maximum_CNT=0;
	filter_cnt.F_Valid_CNT=0;
	
	
//	if((sum_max - sum_min) < 1000)//波形较矮的话，可能会导致判定失败
//	{
//		
//		LSD_CMOS.LSD_VALUE[arount_cnt]=-1;
////		printf("max = %d ;; min = %d\r\n",sum_max/100,sum_min/100);
//		return;
//	}
	sum_max=0;
	sum_min=1048575;
	memset(sum_count,0x00,sizeof(sum_count));
	for(uint16_t i=0; i<10; i++)
	{
		for(uint16_t m=0; m<10; m++)
		{
			sum_count[i]+=LSD_CMOS.LSD_ADC[(m+80)+(i*10)+(hun_index*100)];
		}
		if(sum_max<sum_count[i])
		{
			sum_max=sum_count[i];
			ten_index=i;
		}
	}
	sum_max=0;
	memset(sum_count,0x00,sizeof(sum_count));
	for(uint16_t i=0; i<10; i++)
	{
		for(uint16_t m=0; m<3; m++)
		{
			sum_count[i]+=LSD_CMOS.LSD_ADC[(m+i+80)+(ten_index*10)+(hun_index*100)];
		}
//		sum_count[i]+=LSD_CMOS.LSD_ADC[(i+7)+(ten_index*10)+(hun_index*100)];
		
		if(sum_max<sum_count[i])
		{
			sum_max=sum_count[i];
			uni_index=i;
			if(arount_cnt==1 && show_flag==1)
			{	//打印波形
				printf("sum_max = %d,i=%d",sum_max,i);
			}
		}
	}
	sum_max=0;
	memset(sum_count,0x00,sizeof(sum_count));
	LSD_CMOS.LSD_VALUE[1]=(hun_index*100)+(ten_index*10)+uni_index;
//	printf("lsd = %f\r\n",LSD_CMOS.LSD_VALUE[1]);
	CMOS_DIS(1);
	if(LSD_CMOS.LSD_START == 1)
	{
		LSD_CMOS.LSD_START=0;
		CMOS_turnoff();
	}


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
//	LSD_CMOS.LSD_VALUE[2]-=200;
	lsd_sum=LSD_CMOS.LSD_VALUE[1];

	lsd_sum=(1400-lsd_sum)*0.57;
	if(lsd_sum>0 && lsd_sum<1400 )
	{
		if(LSD_CMOS.LSD_RESULT)
		{
			LSD_CMOS.LSD_ORIGIN = LSD_CMOS.LSD_VALUE[1];
			fmc_str.fmc_buffer[DATA_ORIGIN]=LSD_CMOS.LSD_ORIGIN;
			FMC_WRITE_BUFFER(FMC_START_ADDR,fmc_str.fmc_buffer,6);
			
			printf("set origin ok\r\n");
			LSD_CMOS.LSD_RESULT=0;
		}
		
			sprintf(p_buf,"DISTANCE,OK,%.2f",lsd_sum);
			V_STR_Printf(p_buf);
	}
	else{
			sprintf(p_buf,"DISTANCE,ERROR,0");
			V_STR_Printf(p_buf);
//		printf("\r\nerror = %.f\r\n",LSD_CMOS.LSD_VALUE[2]);
	}

}




