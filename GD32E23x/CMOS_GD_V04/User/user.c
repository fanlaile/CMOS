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
extern uint8_t show_flag;
extern LSD_Struct LSD_CMOS;
extern uint16_t adc_Buf;
extern char p_buf[];
uint16_t adc_Buf_fake;
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
			adc_Buf=Get_ADC_Value();

			
			if(cnt<LSD_SIZE && LSD_CMOS.LSD_INT==2 && LSD_CMOS.SP_STATUS){

				LSD_CMOS.LSD_ADC[(cnt/2)+200]+=adc_Buf;
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
			adc_Buf_fake=Get_ADC_Value();
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
	int smooth_sum=0;
	
	for(uint16_t k=0;k<200;k++)
	{
		LSD_CMOS.LSD_ADC[k]=6500-200+k;
		LSD_CMOS.LSD_ADC[1423-k]=7000-200+k;
//		LSD_CMOS.LSD_ADC[1000+k]=7200;
	}

	for(uint16_t i=200;i<1224;i++){
		for(uint16_t m=1;m<10;m++){
			smooth_sum+=LSD_CMOS.LSD_ADC[i+m];
		}
		LSD_CMOS.LSD_ADC[i]=smooth_sum/10;
		smooth_sum=0;
	}

	
	for(uint16_t i=0;i<1124;i++){
		if(arount_cnt==3 && show_flag==1){
		  printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
		}
		
		if(arount_cnt==3)
		{
			if(Laser_Init.led_start_flag==0)
			{
				if(LSD_CMOS.LSD_ADC[i]<6000)
				{
					Laser_Init.led_start_flag=i;
//					printf("sf%d=%d\r\n",i,LSD_CMOS.LSD_ADC[i]);
				}
			}
			else if(Laser_Init.led_end_flag==0 )
			{
				if(LSD_CMOS.LSD_ADC[i]>6000 )
				{
					Laser_Init.led_end_flag=i;
//					printf("ef%d=%d\r\n",i,LSD_CMOS.LSD_ADC[i]);
				}
			}
		}
		
		for(uint16_t m=0;m<smooth_length;m++){
			left_sum+=LSD_CMOS.LSD_ADC[i+m];
			right_sum+=LSD_CMOS.LSD_ADC[i+m+smooth_length];
		}
		abs_val=abs(left_sum+right_sum);
		left_sum=0;
		right_sum=0;


		if(abs_minn>abs_val){
			lsd_index=i;
			LSD_CMOS.LSD_VALUE[arount_cnt]=i;
			abs_minn=abs_val;
		}
		abs_val=0;
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
	LSD_CMOS.LSD_VALUE[2]-=200;
	lsd_sum=LSD_CMOS.LSD_VALUE[2]-LSD_CMOS.LSD_ORIGIN;

	if(LSD_CMOS.LSD_VALUE[2]>0 && LSD_CMOS.LSD_VALUE[2]<800)
	{
		if(LSD_CMOS.LSD_RESULT)
		{
			LSD_CMOS.LSD_ORIGIN = LSD_CMOS.LSD_VALUE[DATA_ORIGIN];
			fmc_str.fmc_buffer[1]=LSD_CMOS.LSD_ORIGIN;
			FMC_WRITE_BUFFER(FMC_START_ADDR,fmc_str.fmc_buffer,6);
			
			printf("set origin ok\r\n");
			LSD_CMOS.LSD_RESULT=0;
		}
		if(Laser_Init.led_init_flag)
		{
			printf("led start=%d,led end=%d\r\n",Laser_Init.led_start_flag,Laser_Init.led_end_flag);
			if((Laser_Init.led_end_flag-Laser_Init.led_start_flag)<150 )
			{
				if(LSD_CMOS.LED_PWM<100)
				{
					LSD_CMOS.LED_PWM+=5;
				}
				Laser_Init.led_result=0;
				
				printf("pwm++=%d\r\n",LSD_CMOS.LED_PWM);
			}
			else if((Laser_Init.led_end_flag-Laser_Init.led_start_flag)>200)
			{
				if(LSD_CMOS.LED_PWM>0)
				{
					LSD_CMOS.LED_PWM-=5;
				}
				Laser_Init.led_result=0;
				
				printf("pwm--=%d\r\n",LSD_CMOS.LED_PWM);
			}
			else
			{
				Laser_Init.led_result=1;
				fmc_str.fmc_buffer[2]=LSD_CMOS.LED_PWM;
				FMC_WRITE_BUFFER(FMC_START_ADDR, fmc_str.fmc_buffer,6);//初始化标志
			}
			
			Laser_Init.led_end_flag=0;
			Laser_Init.led_start_flag=0;
		}
			sprintf(p_buf,"DISTANCE,OK,%.2f",lsd_sum);
			V_STR_Printf(p_buf);
	}
	else{
			sprintf(p_buf,"DISTANCE,ERROR,0");
			V_STR_Printf(p_buf);
	}

}




