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
  * @brief  ���ڷ���һ���ַ�
  * @param  C:�跢�͵��ַ�
  * @retval none
  */
void SerialPutChar(uint8_t c)
{
   
		usart_data_transmit(USART0, (uint8_t) c);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
}
/**
  * @brief  ���ڷ���һ���ַ���
  * @param  *s:�跢�͵��ַ���
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
  * @brief  ����Falshҳ
  * @param  Size:�ļ�����
  * @retval ҳ������
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
  * @brief  ��ת���û�����ִ��
  * @param  void
  * @retval none
  */
uint8_t Execute_user_Program(void)
{
	__disable_irq();
    if (((*(__IO uint32_t *)ApplicationAddress) & 0x2FFE0000) == 0x20000000)//�ж��û��Ƿ��Ѿ����س��򣬷�ֹ�ܷ�
    {
        //��ת���û�����
        JumpAddress = *(__IO uint32_t *)(ApplicationAddress + 4);
        Jump_To_Application = (pFunction)JumpAddress;
        //��ʼ���û�����Ķ�ջָ��
        __set_MSP(*(__IO uint32_t *)ApplicationAddress);
        Jump_To_Application();
    }
    else
    {
        return 1;
    }   
    return 0;
}
/**
  * @brief  ���Գ����ն��Ƿ��а�������
  * @param  key:����
  * @retval 1����ȷ
  *         0������
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
  * @brief  ͨ�������жϻ�ȥ����
  * @param  key:����
  * @retval ���µļ���
  */
uint8_t GetKey(void)
{
    uint8_t key = 0;

    //�ȴ���������
    while (1)
    {
        if (SerialKeyPressed((uint8_t *)&key))
            break;
    }
    return key;
}
uint8_t GD_UART_Receive(uint32_t usart, uint8_t *val, uint8_t num, uint32_t timeout)
{
		uint8_t key = 0;

    //�ȴ���������
    while (1)
    {
        if (SerialKeyPressed((uint8_t *)&val))
            break;
    }
    return 0;
}
/****************** flash write **************/
/*!
    \brief      FLASHд
    @Address    ������ַ
    @Size       ���ݳ���
*/
uint8_t FLASH_ErasePage(uint32_t adr)
{
       
    uint32_t Address = adr;
	
    /* ���� */
    fmc_unlock();
	/* step1: erase pages */
    fmc_page_erase(Address);
    /* step2: ����FMCǰ�����STAT ״̬�Ĵ������ǳ���Ҫ*/
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
    fmc_lock();
    return 0;
}
