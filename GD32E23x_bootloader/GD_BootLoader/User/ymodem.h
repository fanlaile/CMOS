
/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Inc/ymodem.h 
  * @author  MCD Application Team
  * @brief   This file provides all the software function headers of the ymodem.c 
  *          file.
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __YMODEM_H_
#define __YMODEM_H_

/* Includes ------------------------------------------------------------------*/
#include "gd32e23x.h"
//#include "gd32e23x_fmc.h"
/* Exported types ------------------------------------------------------------*/
//extern fmc_state_enum fmc_state;
#define FLASH_COMPLETE 0
/**
  * @brief  Comm status structures definition
  */
typedef enum
{
  COM_OK       = 0x00,
  COM_ERROR    = 0x01,
  COM_ABORT    = 0x02,
  COM_TIMEOUT  = 0x03,
  COM_DATA     = 0x04,
  COM_LIMIT    = 0x05
} COM_StatusTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Packet structure defines */
#define PACKET_HEADER_SIZE      ((uint32_t)3)
#define PACKET_DATA_INDEX       ((uint32_t)4)
#define PACKET_START_INDEX      ((uint32_t)1)
#define PACKET_NUMBER_INDEX     ((uint32_t)2)
#define PACKET_CNUMBER_INDEX    ((uint32_t)3)
#define PACKET_TRAILER_SIZE     ((uint32_t)2)
#define PACKET_OVERHEAD_SIZE    (PACKET_HEADER_SIZE + PACKET_TRAILER_SIZE - 1)
//#define PACKET_SIZE             ((uint32_t)128)
//#define PACKET_1K_SIZE          ((uint32_t)1024)

/* /-------- Packet in IAP memory ------------------------------------------\
 * | 0      |  1    |  2     |  3   |  4      | ... | n+4     | n+5  | n+6  | 
 * |------------------------------------------------------------------------|
 * | unused | start | number | !num | data[0] | ... | data[n] | crc0 | crc1 |
 * \------------------------------------------------------------------------/
 * the first byte is left unused for memory alignment reasons                 */

//#define FILE_NAME_LENGTH        ((uint32_t)64)
//#define FILE_SIZE_LENGTH        ((uint32_t)16)

//#define SOH                     ((uint8_t)0x01)  /* start of 128-byte data packet */
//#define STX                     ((uint8_t)0x02)  /* start of 1024-byte data packet */
//#define EOT                     ((uint8_t)0x04)  /* end of transmission */
//#define ACK                     ((uint8_t)0x06)  /* acknowledge */
//#define NAK                     ((uint8_t)0x15)  /* negative acknowledge */
//#define CA                      ((uint32_t)0x18) /* two of these in succession aborts transfer */
//#define CRC16                   ((uint8_t)0x43)  /* 'C' == 0x43, request 16-bit CRC */
#define NEGATIVE_BYTE           ((uint8_t)0xFF)

//#define ABORT1                  ((uint8_t)0x41)  /* 'A' == 0x41, abort by user */
//#define ABORT2                  ((uint8_t)0x61)  /* 'a' == 0x61, abort by user */

//#define NAK_TIMEOUT             ((uint32_t)0x100000)
//#define DOWNLOAD_TIMEOUT        ((uint32_t)5000) /* Five second retry delay */
//#define MAX_ERRORS              ((uint32_t)5)




#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER )
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  //128字节数据包开始
#define STX                     (0x02)  //1024字节的数据包开始
#define EOT                     (0x04)  //结束传输
#define ACK                     (0x06)  //回应
#define NAK                     (0x15)  //没回应
#define CA                      (0x18)  //这两个相继中止转移
#define CRC16                   (0x43)  //'C' == 0x43, 需要 16-bit CRC 

#define ABORT1                  (0x41)  //'A' == 0x41, 用户终止 
#define ABORT2                  (0x61)  //'a' == 0x61, 用户终止

#define NAK_TIMEOUT             (0x100000)
#define MAX_ERRORS              (5)


int32_t Ymodem_Receive (uint8_t *);
uint8_t Ymodem_Transmit (uint8_t *,const  uint8_t* , uint32_t );










/* Exported functions ------------------------------------------------------- */
//COM_StatusTypeDef Ymodem_Receive(uint32_t *p_size);
//COM_StatusTypeDef Ymodem_Transmit(uint8_t *p_buf, const uint8_t *p_file_name, uint32_t file_size);

#endif  /* __YMODEM_H_ */

/*******************(C)COPYRIGHT STMicroelectronics ********END OF FILE********/
