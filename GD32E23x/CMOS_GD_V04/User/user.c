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
//uint16_t curve[64];
uint16_t lsd_index=0;

uint8_t ep_flag=1;
/**
  * @brief CMOS RUN CP & SP
  * @retval None
  */
void CMOS_CLK(void){
	
	EP_L();
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
			#ifdef ENABLE_EP
			if(i==4100)
			{
//				EP_H();
			}
			if( i==4200)
			{
				EP_H();
			}
			#endif
			adc_Buf_fake=Get_ADC_Value();
		}
	}//FORJ结束启动定时器，



	
}

/**
  * @brief CMOS ADC
  * @retval None
  */
void CMOS_ADC(uint8_t arount){
	int left_sum=0,right_sum=0,abs_val=0x7FFFFFFF,sum_min=0x7FFFFFFF,sum_max=0,sum_max_2=0,act_val=0;
	int abs_minn=0x7FFFFFFF;
	uint16_t sum_min_index=0,sum_max_index=0,sum_max_index_2=0;
	uint16_t sum_max_error=0,sum_min_error=0;
	uint16_t edge_start=0,edge_end=0;
	uint8_t laser_too_high=0;
	uint16_t no_peak=0,laser_too_low=0;
	int smooth_sum=0;
	
	for(uint16_t k=0;k<200;k++)						//首尾填充数据
	{
		LSD_CMOS.LSD_ADC[1224+k]=LSD_CMOS.LSD_ADC[1220];
//		LSD_CMOS.LSD_ADC[1024+k]=8000;
//		LSD_CMOS.LSD_ADC[1000+k]=7200;
	}
/*
	*求相加最小
	*
	*
	*
	*
	*/
	for(uint16_t n=200;n<1224;n++){				//数据平滑处理
		for(uint16_t m=0;m<70;m++){
			smooth_sum+=LSD_CMOS.LSD_ADC[n+m];
		}
		LSD_CMOS.LSD_ADC[n]=smooth_sum/70;
		smooth_sum=0;
	}
//	
	

/*
*
*求相减绝对值最小
*此算法适用于无曝光模式
*
*/
#ifndef ENABLE_EP
	for(uint16_t i=200; i<1024; i++)
	{
//		if(arount_cnt==2 && show_flag==1){	//打印波形
//		  printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
//		}
		if((LSD_CMOS.LSD_ADC[i+1] - LSD_CMOS.LSD_ADC[i])<0 && (LSD_CMOS.LSD_ADC[i+1] - LSD_CMOS.LSD_ADC[i])>-5)
		{
			
			no_peak++;
		}
		if(LSD_CMOS.LSD_ADC[i]<8000)
		{
			laser_too_high=1;
//			printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
		}
		if(LSD_CMOS.LSD_ADC[i]>7000 )
		{
			laser_too_low=1;
		}
		for(uint16_t m=0;m<smooth_length;m++){	
			left_sum+=LSD_CMOS.LSD_ADC[i+m];
			right_sum+=LSD_CMOS.LSD_ADC[i+m+smooth_length];
		}
		act_val=left_sum-right_sum;
		if(arount_cnt==2 && show_flag==1){	//打印波形
		  printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
		}
		left_sum=0;
		right_sum=0;
		if(sum_max<act_val && sum_max_error<100 && act_val>100)
		{
			sum_max=act_val;
			sum_max_index=i;
		}
		else if(sum_max>act_val && sum_max!=0)
		{
			sum_max_error++;
		}
		if(sum_min>act_val)
		{
			sum_min=act_val;
			sum_min_index=i;
		}
		act_val=0;
	}
	if(arount_cnt==2 && show_flag==1){	//打印波形
		  printf("sum_max_index=%d**%d  sum_min_index=%d**%d  no_peak=%d  sum_max_error=%d\r\n",sum_max_index,sum_max,sum_min_index,sum_min,no_peak,sum_max_error);
		
	}
	sum_max_error=0;
	
	
	for(uint16_t i=200; i<1024; i++)
	{
		if(laser_too_high==0 || no_peak>120 || (sum_max-sum_min)<25000)
		{
			LSD_CMOS.LSD_VALUE[arount_cnt]=0;
//			if(arount_cnt==2 && show_flag==1){	//打印波形
//				printf("too high = %d  max-min = %d\r\n",laser_too_high,(sum_max-sum_min));
//			}
			break;
		}
		
		for(uint16_t m=0;m<smooth_length;m++){	
			left_sum+=LSD_CMOS.LSD_ADC[i+m];
			right_sum+=LSD_CMOS.LSD_ADC[i+m+smooth_length];
		}
		if(i>sum_max_index && i<sum_min_index && i<(sum_max_index+200))
		{
			abs_val=abs(left_sum-right_sum);
			
			if(abs_minn>abs_val){
				lsd_index=i;
				LSD_CMOS.LSD_VALUE[arount_cnt]=i;
				abs_minn=abs_val;
			}
			if((left_sum-right_sum)<0)
			{
				break;
			}
			
		}
		left_sum=0;
		right_sum=0;
		abs_val=0;
	}
	sum_max=0;
	sum_min=0x7FFFFFFF;
	sum_max_index=0;
	sum_min_index=0;
	
#else
	#ifndef BORDER_VALUE

/*
*此算法适用于有曝光的模式
*
*
*
*
*/
	for(uint16_t i=200; i<1024; i++)
	{
//		if(arount_cnt==2 && show_flag==1){	//打印波形
//		  printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
//		}
		if((LSD_CMOS.LSD_ADC[i+1] - LSD_CMOS.LSD_ADC[i])<0 && (LSD_CMOS.LSD_ADC[i+1] - LSD_CMOS.LSD_ADC[i])>-5)
		{
			
			no_peak++;
		}
		if(LSD_CMOS.LSD_ADC[i]>7000   )
		{
//			laser_too_high=1;
//			printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
		}
		if(LSD_CMOS.LSD_ADC[i]>2300 )
		{
			laser_too_low=1;
		}
		for(uint16_t m=0;m<smooth_length;m++){	
			left_sum+=LSD_CMOS.LSD_ADC[i+m];
			right_sum+=LSD_CMOS.LSD_ADC[i+m+smooth_length];
		}
		act_val=left_sum-right_sum;
		if(arount_cnt==2 && show_flag==1){	//打印波形
		  printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
		}
		left_sum=0;
		right_sum=0;
		if(sum_max<act_val && sum_max_error<50 && act_val>500)
		{
			sum_max=act_val;
			sum_max_index=i;
		}
		else if(sum_max>act_val && sum_max!=0)
		{
			sum_max_error++;
		}
		if(sum_max_error>=50 && act_val>500 && sum_max_2<act_val)
		{
			sum_max_2=act_val;
			sum_max_index_2=i;
		}
		
		if(sum_min>act_val )
		{
			sum_min=act_val;
			sum_min_index=i;
		}
		
		act_val=0;
	}
	if(arount_cnt==2 && show_flag==1){	//打印波形
		  printf("sum_max_index=%d**%d   sum_max_index_2=%d**%d   sum_min_index=%d**%d    no_peak=%d\r\n",sum_max_index,sum_max,sum_max_index_2,sum_max_2,sum_min_index,sum_min,no_peak);
		
	}
	sum_max_error=0;
	if(sum_max_index<sum_min_index && sum_max_index_2>sum_min_index)
	{
		sum_max_index=sum_max_index_2;
	}
	
	
	for(uint16_t i=200; i<1024; i++)
	{
		if( (sum_max-sum_min)<20000 || laser_too_low==0 || laser_too_high==1 || no_peak>300)
		{
			LSD_CMOS.LSD_VALUE[arount_cnt]=0;
//			if(arount_cnt==2 && show_flag==1){	//打印波形
//				printf("too high = %d  max-min = %d\r\n",laser_too_high,(sum_max-sum_min));
//			}
			break;
		}
		
		for(uint16_t m=0;m<smooth_length;m++){	
			left_sum+=LSD_CMOS.LSD_ADC[i+m];
			right_sum+=LSD_CMOS.LSD_ADC[i+m+smooth_length];
		}
		if(i<sum_max_index && i>sum_min_index && i<(sum_min_index+200))
		{
			abs_val=abs(left_sum-right_sum);
			
			if(abs_minn>abs_val){
				lsd_index=i;
				LSD_CMOS.LSD_VALUE[arount_cnt]=i;
				abs_minn=abs_val;
			}
			else
			{
				break;
			}
			if((left_sum-right_sum)>0)
			{
				break;
			}
			
		}
		left_sum=0;
		right_sum=0;
		abs_val=0;
	}
	sum_max=0;
	sum_min=0x7FFFFFFF;
	sum_max_index=0;
	sum_min_index=0;
	#endif

	
	#ifdef BORDER_VALUE
	uint16_t border_1=0,border_2=0,border_3=0,border_4=0;
	
	for(uint16_t i=200; i<1024; i++)
	{

		if(LSD_CMOS.LSD_ADC[i]>3200 && border_1==0 )
		{
			border_1=i;

		}
		else if(LSD_CMOS.LSD_ADC[i]<3200 && border_2==0 && border_1!=0)
		{
			border_2=i;
		}
		else if(LSD_CMOS.LSD_ADC[i]>3200 && border_3==0 && border_2!=0)
		{
			border_3=i;
		}
		else if(LSD_CMOS.LSD_ADC[i]<3200 && border_4==0 && border_3!=0)
		{
			border_4=i;
		}
		
		
		if(arount_cnt==2 && show_flag==1){	//打印波形
		  printf("%d\r\n",LSD_CMOS.LSD_ADC[i]);
		}
		
	}
	if(arount_cnt==2 && show_flag==1){	//打印波形
		  printf("B1=%d   B2=%d   B3=%d  B4=%d\r\n",border_1,border_2,border_3,border_4);
		
	}
	LSD_CMOS.LSD_VALUE[arount_cnt]=(border_2+border_3)/2;
	#endif
	

#endif
	
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
	lsd_sum=LSD_CMOS.LSD_VALUE[2];

	if(LSD_CMOS.LSD_VALUE[2]>0 && LSD_CMOS.LSD_VALUE[2]<800)
	{
		if(LSD_CMOS.LSD_RESULT)
		{
			LSD_CMOS.LSD_ORIGIN = LSD_CMOS.LSD_VALUE[2];
			fmc_str.fmc_buffer[DATA_ORIGIN]=LSD_CMOS.LSD_ORIGIN;
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
				fmc_str.fmc_buffer[DATA_PWM]=LSD_CMOS.LED_PWM;
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
//		printf("\r\nerror = %.f\r\n",LSD_CMOS.LSD_VALUE[2]);
	}

}




