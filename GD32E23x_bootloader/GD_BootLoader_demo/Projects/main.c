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

/* USER CODE BEGIN PV */


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	uint8_t key = 0;
	/*  configure SysTick */
	systick_config();
	delay_1ms(100);
	/* configure EXTI, TIMER */
	Usart_GPIO_Init();
	Usart_Init();
  delay_1ms(100);
	Main_Menu();
//	if (FLASH_data_check(FLASH_LAST_PAGE, FLASH_DATA_VAL))
//  {
//     Upgrade_program:
//        fmc_unlock();
//        SerialPutString((uint8_t*)"\r\n Upgrade the program with Ymodem \r\n");
//        Main_Menu();
//   }
//	else
//	{
//				SerialPutString((uint8_t*)"\r\n No user Program, Enter 9 to upgrade\r\n");
//        key = GetKey();
//        if (key == 0x39)
//        {
//            goto Upgrade_program;
//        }
//	}
    
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
        check_val = 0;
    }
    return check_val;
}

