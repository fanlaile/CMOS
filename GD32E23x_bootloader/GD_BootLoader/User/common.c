/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Src/common.c 
  * @author  MCD Application Team
  * @brief   This file provides all the common functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/** @addtogroup STM32F4xx_IAP_Main
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
uint32_t FlashDestination = ApplicationAddress;
uint8_t tab_1024[1024] = {0};
uint8_t file_name[FILE_NAME_LENGTH];
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Convert an Integer to a string
  * @param  p_str: The string output pointer
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t *p_str, uint32_t intnum)
{
  uint32_t i, divider = 1000000000, pos = 0, status = 0;

  for (i = 0; i < 10; i++)
  {
    p_str[pos++] = (intnum / divider) + 48;

    intnum = intnum % divider;
    divider /= 10;
    if ((p_str[pos-1] == '0') & (status == 0))
    {
      pos = 0;
    }
    else
    {
      status++;
    }
  }
}

/**
  * @brief  Convert a string to an integer
  * @param  p_inputstr: The string to be converted
  * @param  p_intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if ((p_inputstr[0] == '0') && ((p_inputstr[1] == 'x') || (p_inputstr[1] == 'X')))
  {
    i = 2;
    while ( ( i < 11 ) && ( p_inputstr[i] != '\0' ) )
    {
      if (ISVALIDHEX(p_inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(p_inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }

    /* valid result */
    if (p_inputstr[i] == '\0')
    {
      *p_intnum = val;
      res = 1;
    }
  }
  else /* max 10-digit decimal input */
  {
    while ( ( i < 11 ) && ( res != 1 ) )
    {
      if (p_inputstr[i] == '\0')
      {
        *p_intnum = val;
        /* return 1 */
        res = 1;
      }
      else if (((p_inputstr[i] == 'k') || (p_inputstr[i] == 'K')) && (i > 0))
      {
        val = val << 10;
        *p_intnum = val;
        res = 1;
      }
      else if (((p_inputstr[i] == 'm') || (p_inputstr[i] == 'M')) && (i > 0))
      {
        val = val << 20;
        *p_intnum = val;
        res = 1;
      }
      else if (ISVALIDDEC(p_inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(p_inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }
  }

  return res;
}



/**
  * @brief  Transmit a byte to the HyperTerminal
  * @param  param The byte to be sent
  * @retval NULL
  */
void Serial_PutByte( uint8_t ch )
{
    usart_data_transmit(IAP_USART, ch);
    while(RESET == usart_flag_get(IAP_USART, USART_FLAG_TBE));
}

/**
  * @brief  串口发送一个字符
  * @param  C:需发送的字符
  * @retval none
  */
void SerialPutChar(uint8_t c)
{
   
		usart_data_transmit(USART0, (uint8_t) c);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
}
/**
  * @brief  串口发送一个字符串
  * @param  *s:需发送的字符串
  * @retval none
  */
void SerialPutString(uint8_t *s)
{
    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
}

/**
  * @brief  计算Falsh页
  * @param  Size:文件长度
  * @retval 页的数量
  */
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}
/**
  * @brief  跳转到用户程序执行
  * @param  void
  * @retval none
  */
uint8_t Execute_user_Program(void)
{
	__disable_irq();
	
	
//	NVIC_SystemReset();
//    if (((*(__IO uint32_t *)ApplicationAddress) & 0x2FFE0000) == 0x20000000)//判断用户是否已经下载程序，防止跑飞
//    {
//				SerialPutString((uint8_t*)"jump to new\r\n");
        //跳转至用户代码
        JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);
        Jump_To_Application = (pFunction)JumpAddress;
        //初始化用户程序的堆栈指针
        __set_MSP(*(__IO uint32_t *)ApplicationAddress);
        Jump_To_Application();
//    }
//    else
//    {
//				SerialPutString((uint8_t*)"jump error\r\n");
//        return 1;
//    }   
    return 0;
}
/**
  * @brief  测试超级终端是否有按键按下
  * @param  key:按键
  * @retval 1：正确
  *         0：错误
  */
uint32_t SerialKeyPressed(uint8_t *key)
{

  //  if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
		if(RESET !=usart_flag_get(USART0, USART_FLAG_RBNE))
    {
			  *key =usart_data_receive(USART0);
//				*key = (uint8_t)USART1->DR;
        return 1;
    }
    else
    {
        return 0;
    }
}
/**
  * @brief  通过超级中断回去键码
  * @param  key:按键
  * @retval 按下的键码
  */
uint8_t GetKey(void)
{
    uint8_t key = 0;

    //等待按键按下
    while (1)
    {
        if (SerialKeyPressed((uint8_t *)&key))
            break;
    }
    return key;
}
/****************** flash write **************/
/*!
    \brief      FLASH写
    @Address    操作地址
    @Size       数据长度
*/
uint8_t FLASH_ErasePage(uint32_t adr)
{
       
    uint32_t Address = adr;
	
    /* 解锁 */
    fmc_unlock();
	/* step1: erase pages */
    fmc_page_erase(Address);
    /* step2: 操作FMC前先清空STAT 状态寄存器，非常必要*/
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
    fmc_lock();
    return 0;
}
/**
  * @brief   通过串口接收一个文件
  * @param  
  * @retval None.
  */
void SerialDownload(void)
{
    uint8_t Number[10] = "          ";
    int32_t Size = 0;

    SerialPutString((uint8_t*)"Waiting for the file to be sent ... (press 'a' to abort)\n\r");
    Size = Ymodem_Receive(&tab_1024[0]);
		delay_1ms(500);
    if (Size > 0)
    {
        SerialPutString((uint8_t*)"\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
        SerialPutString(file_name);
        Int2Str(Number, Size);
        SerialPutString((uint8_t*)"\n\r Size: ");
        SerialPutString(Number);
        SerialPutString((uint8_t*)" Bytes\r\n");
        SerialPutString((uint8_t*)"-------------------\r\n");
			FLASH_ProgramWord(FLASH_LAST_PAGE,0x12341234);
//        FLASH_ErasePage(FLASH_LAST_PAGE);               //擦除IAP升级标志位存放页
    }
    else if (Size == -1)
    {
        SerialPutString((uint8_t*)"\n\n\rThe image size is higher than the allowed space memory!\n\r");
    }
    else if (Size == -2)
    {
        SerialPutString((uint8_t*)"\n\n\rVerification failed!\n\r");
    }
    else if (Size == -3)
    {
        SerialPutString((uint8_t*)"\r\n\nAborted by user.\n\r");
    }
    else
    {
        SerialPutString((uint8_t*)"\n\rFailed to receive the file!\n\r");
    }
}
/**
  * @brief  显示菜单栏在超级终端
  * @param  void
  * @retval none
  */
void Main_Menu(void)
{
    uint8_t key = 0;
    BlockNbr = (FlashDestination - 0x08000000) >> 12;



    UserMemoryMask = ((uint32_t)0x80000000);


    

    while (1)
    {
        SerialPutString((uint8_t*)"  Download Image To the Flash Enter  1\r\n");
        SerialPutString((uint8_t*)"  Execute The New Program Enter      3\r\n");
       
        SerialPutString((uint8_t*)"  ====================================\r\n");
        key = GetKey();

        if (key == 0x31)
        {
            SerialDownload();
        }
        else if (key == 0x33)
        {
            SerialPutString((uint8_t*)"\r\n Execute user Program\r\n");
//            FLASH_ErasePage(FLASH_LAST_PAGE);               //擦除IAP升级标志位存放页
						if (FLASH_data_check(FLASH_LAST_PAGE, FLASH_DATA_VAL))
						{
						 
								if(Execute_user_Program())
								{
										return;
								}
						}
        }
        else if ((key == 0x34) )
        {
            
        }
        else
        {
            
            SerialPutString((uint8_t*)"Invalid Number ! ==> The number should be either 1, 3 or 4\r\n");
            
        }
    }
}

