/****************************************************************************
 * @file    	main.c
 * @version  	V1.00
 * @brief    	
 * @author		FAN
 * @note	
 * Copyright (C) 2023 Creality 3D
*****************************************************************************/

#include "gd32e23x.h"
#include "gd32e230f_start.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

char send_buf[128];
/* USER CODE BEGIN PV */


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	nvic_vector_table_set(NVIC_VECTTAB_FLASH,0x0000);
	__set_PRIMASK(0);
	uint8_t key = 0;
	/*  configure SysTick */
	systick_config();
	delay_1ms(100);
	/* configure EXTI, TIMER */
	Usart_GPIO_Init();
	Usart_Init();
  delay_1ms(100);
	
	if (FLASH_data_check(FLASH_LAST_PAGE, FLASH_DATA_VAL))
  {
     
        Execute_user_Program();
   }
	else
	{			
			
				fmc_unlock();
        SerialPutString((uint8_t*)"\r\n Upgrade the program with Ymodem \r\n");
        Main_Menu();
	}
    
	while(1)
	{
//		printf("hhh\r\n");
//		delay_1ms(500);
	}
}

void jump_to_app(uint32_t addr)
{
	uint32_t app_address;
	void* (*application)(void);
    
	/* test if user code is programmed starting from address */
	if (((*(__IO uint32_t*)addr) & 0x2FFE0000) == 0x20000000)
	{
		app_address = *(__IO uint32_t*) (addr + 4);
		application = (void* (*)(void)) app_address;

		/* initialize user application's stack pointer */
		__set_MSP(*(__IO uint32_t*) addr);
		
        
		rcu_deinit();
		
		/* jump to user application */
		application();
	}
}
/**
  * @brief  检验指定地址的数据是否符合
  * @param  faddr：指定地址
  *         value: 要比较的值
  * @retval 读到的数据
  */
uint8_t FLASH_data_check(uint32_t faddr, uint16_t value)
{
    uint16_t data_first = 0;
    uint8_t check_val = 0;

    data_first = *( __IO uint16_t *)faddr;
    if (data_first == value)
    {
			
        check_val = 1;
    }
    else
    {
//				sprintf(send_buf,"aaaflag=%x\r\n",data_first);
//				SerialPutString((uint8_t*)send_buf);
        check_val = 0;
    }
    return check_val;
}

