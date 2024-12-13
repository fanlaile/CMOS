/****************************************************************************
 * @file     adc.c
 * @version  V1.00
 * @brief    control driver source file
 * @author	 fan
 * @note
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/
#include "systick.h"
#include "adc.h"
#include <stdio.h>

LSD_Struct LSD_CMOS={ 
	30,
	0,
	0,
	0,
	0,
	0,
	0,
	{0},
	{0},
	0,
	1,
	0,
	0,
	0,
	3000,
	0,				//pWd
	LSD_SIZE,  //pRd
	0,
};


uint16_t Dummy_Val[4];
uint16_t ADC_Value_Inject;
dma_parameter_struct dma_data_parameter;

void ADC_Init(void)
{
//  Clock I/O Init
	rcu_periph_clock_enable(RCU_ADC);
	rcu_periph_clock_enable(RCU_DMA);
	rcu_periph_clock_enable(RCU_GPIOA);
	
	gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0); // use A0

  /* ADCCLK = APB2/4 */
	rcu_adc_clock_config(RCU_ADCCK_APB2_DIV4);// ADC Clock 72/4 = 18MHz
	delay_1ms(10);
// DMA Init	
	dma_deinit(DMA_CH0);    
	dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);
	dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
	dma_data_parameter.memory_addr  = (uint32_t)(&Dummy_Val);
	dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE ;
	dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
	dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
	dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
	dma_data_parameter.number       = 4U;
	dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
	dma_init(DMA_CH0, &dma_data_parameter);

	dma_circulation_disable(DMA_CH0);
	dma_memory_to_memory_disable(DMA_CH0);

	/* enable DMA transfer complete interrupt */
  dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);
	
//	dma_channel_enable(DMA_CH0);
	
// ADC Init
	adc_deinit(); //复位ADC
	/* ADC continous function enable */
//	adc_special_function_config(ADC_SCAN_MODE, ENABLE);
	
	//连续转换开启，，，，！！！！！！这里不能开，开了在FLASH编程保存数据时会导致adc数据错位
	adc_special_function_config(ADC_CONTINUOUS_MODE,ENABLE);	
	/* ADC data alignment config */
	adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
	/* ADC分辨率 12B */
	adc_resolution_config(ADC_RESOLUTION_12B);
	/* ADC channel length config */
	adc_channel_length_config(ADC_REGULAR_CHANNEL, 1); // use only 1 CH
	/* ADC inserted channel config */
	adc_regular_channel_config(0U, ADC_CHANNEL_0, ADC_SAMPLETIME_1POINT5); // CH0

	adc_interrupt_enable(ADC_INT_EOIC);

//	/* ADC DMA function enable */
		adc_dma_mode_enable();

/* ADC external trigger source config */
//	adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
	adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_T2_TRGO);	
		
	/* ADC interrupt */
	//    nvic_irq_enable(ADC_CMP_IRQn, 1);

	/* enable ADC interface */
	adc_enable();

	delay_1ms(10);
	/* ADC calibration and reset calibration */
	adc_calibration_enable();
	delay_1ms(10);

/* enable DMA transfer complete interrupt */
  dma_interrupt_enable(DMA_CH0, DMA_INT_FTF);

	/* enable DMA channel 3 */
//  dma_channel_enable(DMA_CH0);
//
	/* ADC external trigger enable */
	adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

/* ADC software trigger enable */
//	adc_software_trigger_enable(ADC_REGULAR_CHANNEL);

}

/*!
    \brief      this function handles DMA_Channel0_IRQHandler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/

/**
 * @brief  This function handles ADC global interrupt request.
 */
void DMA_Channel0_IRQHandler(void)
//void	ADC_IRQHandler(void)
{
		if (dma_flag_get(DMA_CH0, DMA_FLAG_FTF ) != RESET)
		{
				DMA_ConvCpltCallback();
//				adc_flag_clear(ADC_FLAG_STRC);
//        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_G);
				dma_flag_clear(DMA_CH0, DMA_FLAG_FTF);
		}
/*
    if(dma_interrupt_flag_get(DMA_CH0, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_G);
    }
*/		
		
}

/*!
    \brief      this function handles ADC Start with DMA
    \param[in]  uint16_t *pData, uint16_t length
    \param[out] none
    \retval     none
*/

void ADC_Start_DMA( uint16_t *pData, uint16_t length)
{
		/* Enable ADC */
		adc_disable();

		/* disable DMA channel 0 */
    dma_interrupt_flag_clear(DMA_CH0, DMA_INT_FLAG_G);
    dma_channel_disable(DMA_CH0);
#if 1
		dma_data_parameter.memory_addr  = (uint32_t)pData;
#if 1
	dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);
	dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
	dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//	dma_data_parameter.memory_addr  = (uint32_t)(&ADC_Value_Inject);
	dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE ;
	dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
	dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
  dma_data_parameter.number       = length;
	dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
//	dma_init(DMA_CH0, &dma_data_parameter);
#endif
	dma_init(DMA_CH0, &dma_data_parameter);
#endif	
//	  DMA_CH0->MADDR = (uint32_t)pData;
//    DMA_CH0->TXNUM = length;
//		DMA_CH0->PADDR = (uint32_t)&ADC_RDATA;

//
//		DMA_ConfigInt(DMA_CH1, DMA_INT_TXC, ENABLE); //|DMA_INT_HTX
//
//		ADC_ConfigInt( ADC, ADC_INT_ENDC, ENABLE);
//
    dma_channel_enable(DMA_CH0);

		adc_dma_mode_enable();

		/* Enable ADC */
		adc_enable();
		/* Check ADC start flag */
		while( adc_flag_get(ADC_FLAG_STRC) == RESET ) 
			;
}

/*!
    \brief      this function handles ADC Start with DMA
    \param[in]  uint16_t *pData, uint16_t length
    \param[out] none
    \retval     none
*/

void ADC_Stop_DMA()
{
		adc_disable();
		adc_dma_mode_disable();
	
    dma_channel_disable(DMA_CH0);
}

/**
 * @brief    AD采集
 *
 * @param    None
 *
 * @returns  None
 *
 * @details 
 *
 */

uint16_t adc_get_result(void)
{
	adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
//	while( !dma_flag_get(DMA_CH0, DMA_FLAG_FTF));
//	/* clear channel0 transfer complete flag */
//	dma_flag_clear(DMA_CH0, DMA_FLAG_FTF);
	return  ADC_Value_Inject;
}

/**********************************************************
 * 函 数 名 称：Get_ADC_Value
 * 函 数 功 能：读取ADC值
 * 传 入 参 数：ADC_CHANNEL_x=要采集的通道
 * 函 数 返 回：测量到的值
 * 作       者：LiangXia
 * 备       注：无
 **********************************************************/
unsigned int Get_ADC_Value(void)
{
    unsigned int adc_value = 0;
    // 设置采集通道
//    adc_regular_channel_config(0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    // 开始软件转换
    adc_software_trigger_enable( ADC_REGULAR_CHANNEL);
    // 等待 ADC0 采样完成
    while(!adc_flag_get( ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    adc_flag_clear( ADC_FLAG_EOC);
    // 读取采样值
    adc_value = adc_regular_data_read();
    // 返回采样值
    return adc_value;
}
/**********************************************************
 * 函 数 名 称：Get_ADC_Value
 * 函 数 功 能：读取ADC值
 * 传 入 参 数：ADC_CHANNEL_x=要采集的通道
 * 函 数 返 回：测量到的值
 * 作       者：LiangXia
 * 备       注：无
 **********************************************************/
unsigned int Get_Temp(void)
{
    unsigned int adc_value = 0;
    // 设置采集通道
    adc_regular_channel_config(0, ADC_CHANNEL_16, ADC_SAMPLETIME_1POINT5);
    // 开始软件转换
    adc_software_trigger_enable( ADC_REGULAR_CHANNEL);
    // 等待 ADC0 采样完成
    while(!adc_flag_get( ADC_FLAG_EOC));
    /* clear the end of conversion flag */
    adc_flag_clear( ADC_FLAG_EOC);
    // 读取采样值
    adc_value = adc_regular_data_read();
    // 返回采样值
    return adc_value;
}

