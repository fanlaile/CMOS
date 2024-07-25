/*
 * ymodem.c
 *
 *  Created on: 2017年9月21日
 *      Author: Administrator
 *    Function:和Ymodem.c的相关的协议文件。负责从超级终端接收数据(使用Ymodem协议)，并将数据加载到内部RAM中。
 *             如果接收数据正常，则将数据编程到Flash中；如果发生错误，则提示出错。
 */
#include "ymodem.h"

extern uint8_t file_name[FILE_NAME_LENGTH];
//用户程序Flash偏移
extern uint32_t FlashDestination;
uint16_t PageSize = PAGE_SIZE;
uint32_t EraseCounter = 0x0;
uint32_t NbrOfPage = 0;


uint32_t RamSource;
extern uint8_t tab_1024[1024];
uint8_t FLASHStatus=0;
uint8_t receive_data[1024];

/**
  * @brief   从发送端接收一个字节
  * @param  c: 接收字符
  *         timeout: 超时时间
  * @retval 0：成功接收
  *         1：时间超时
  */
static int32_t Receive_Byte(uint8_t *c, uint32_t timeout)
{
    while (timeout-- > 0 )
    {
        if (SerialKeyPressed(c) == 1)
        {
            return 0;
        }
    }
    return -1;
}
uint8_t GD_UART_Receive(uint32_t usart, uint8_t *val, uint16_t num, uint32_t timeout)
{
		uint8_t key = 0;
		uint16_t length = 0;
    //等待按键按下
    while (timeout-- > 0 && length++ >=num)
    {
        if (SerialKeyPressed((uint8_t *)&val[length]))
            break;
    }
    return 0;
}
/**
  * @brief   发送一个字符
  * @param  c: 发送的字符
  * @retval 0：成功发送
  */
static uint32_t Send_Byte(uint8_t c)
{
    SerialPutChar(c);
    return 0;
}
/**
  * @brief  Update CRC16 for input byte
  * @param  crc_in input value 
  * @param  input byte
  * @retval None
  */
uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte)
{
  uint32_t crc = crc_in;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  
  while(!(in & 0x10000));

  return crc & 0xffffu;
}
/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
  * @retval None
  */
uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = p_data+size;

  while(p_data < dataEnd)
    crc = UpdateCRC16(crc, *p_data++);
 
  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}
/**
  * @brief   从发送端接收一个数据包
  * @param  data ：数据指针
  *         length：长度
  *         timeout ：超时时间
  * @retval 0: 正常返回
  *        -1: 超时或者数据包错误
  *         1: 用户取消
  */
static int32_t Receive_Packet(uint8_t *data, int32_t *length, uint32_t timeout)
{
    uint16_t i, packet_size;
    uint8_t c;
	uint32_t crc;
	uint8_t status;
    *length = 0;
    if (Receive_Byte(&c, timeout) != 0)
    {
        return -1;
    }
    switch (c)
    {
			case SOH:
					packet_size = PACKET_SIZE;
					break;
			case STX:
					packet_size = PACKET_1K_SIZE;
					break;
			case EOT:
					return 0;
			case CA:
					if ((Receive_Byte(&c, timeout) == 0) && (c == CA))
					{
							*length = -1;
							return 0;
					}
					else
					{
							return -1;
					}
			case ABORT1:
			case ABORT2:
					return 1;
			default:
					return -1;
			

			
    }
    *data = c;
    for (i = 1; i < (packet_size + PACKET_OVERHEAD); i++)
    {
//				if(i==3)
//				{
//					data[3]=1;
//				}
//        else 
				if (Receive_Byte(data + i, timeout) != 0)
        {
            return -1;
        }
    }
    if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
    {
        return -1;
    }
    *length = packet_size;
    return 0;
}
/**
  * @brief   通过 ymodem协议接收一个文件
  * @param  buf: 首地址指针
  * @retval 文件长度
  */
int32_t Ymodem_Receive(uint8_t *buf)
{
    uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH], *file_ptr, *buf_ptr;
    int32_t i, j, packet_length, session_done, file_done, packets_received, errors, session_begin;// size = 0;
	uint32_t size = 0;

    //初始化Flash地址变量
    FlashDestination = ApplicationAddress;

    for (session_done = 0, errors = 0, session_begin = 0;;)
    {
        for (packets_received = 0, file_done = 0, buf_ptr = buf;;)
        {
            switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))
            {
							case 0:
                errors = 0;
                switch (packet_length)
                {
                    //发送端终止
									case -1:
                    Send_Byte(ACK);
                    return 0;
                    //结束传输
									case 0:
                    Send_Byte(ACK);
                    file_done = 1;
                    break;
                    //正常的数据包
									default:
                    if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
                    {
                        Send_Byte(NAK);
                    }
                    else
                    {
                        if (packets_received == 0)
                        {
                            //文件名数据包
                            if (packet_data[PACKET_HEADER] != 0)
                            {
                                //文件名数据包有效数据区域
                                for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
                                {
                                    file_name[i++] = *file_ptr++;
                                }
                                file_name[i++] = '\0';
                                for (i = 0, file_ptr++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
                                {
                                    file_size[i++] = *file_ptr++;
                                }
                                file_size[i++] = '\0';
                                Str2Int(file_size, &size);

                                //测试数据包是否过大
                                if (size > (FLASH_SIZE - 1))
                                {
                                    //结束
                                    Send_Byte(CA);
                                    Send_Byte(CA);
                                    return -1;
                                }

                                //计算需要擦除Flash的页
                                NbrOfPage = FLASH_PagesMask(size);

                                //擦除Flash
                                for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
                                {
                                    FLASHStatus = FLASH_ErasePage(FlashDestination + (PageSize * EraseCounter));
                                }
                                Send_Byte(ACK);
                                Send_Byte(CRC16);
                            }
                            //文件名数据包空，结束传输
                            else
                            {
                                Send_Byte(ACK);
                                file_done = 1;
                                session_done = 1;
                                break;
                            }
                        }
                        //数据包
                        else
                        {
                            memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
													memcpy(receive_data, packet_data + PACKET_HEADER, packet_length);
//                            RamSource = (uint32_t)&buf_ptr+24;
//														receive_data
//                            for (j = 0; (j < packet_length) && (FlashDestination < ApplicationAddress + size); j += 4)
//                            {
//															
//                                //把接收到的数据编写到Flash中
////                                FLASH_ProgramWord(FlashDestination, *(uint32_t *)RamSource);
//																/* 解锁 */
//																fmc_unlock();
//															
//																/* step2: 操作FMC前先清空STAT 状态寄存器，非常必要*/
//																fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
//																fmc_word_program(FlashDestination, *(uint32_t*)RamSource);
//																fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGAERR | FMC_FLAG_PGERR );
//                                if (*( __IO uint32_t* )FlashDestination != *(uint32_t*)RamSource)
//                                {
//                                    //结束
//                                    Send_Byte(CA);
//                                    Send_Byte(CA);
//                                    return -2;
//                                }
//                                FlashDestination += 4;
//                                RamSource += 4;
//                            }
//                            Send_Byte(ACK);
//														RamSource = (uint32_t) & packet_data[PACKET_DATA_INDEX];
														RamSource = (uint32_t) & receive_data;
														if (iap_write_32bit_data(FlashDestination, packet_length/4, (int32_t*) RamSource) == FLASHIF_OK)
														{
															FlashDestination += packet_length;
															Serial_PutByte(ACK);
														}
														else /* An error occurred while writing to Flash memory */
														{
															/* End session */
															Serial_PutByte(CA);
															Serial_PutByte(CA);
															return -2;
														}
                        }
                        packets_received++;
                        session_begin = 1;
										}
                }
                break;
            case 1:
                Send_Byte(CA);
                Send_Byte(CA);
                return -3;
            default:
                if (session_begin > 0)
                {
                    errors++;
                }
                if (errors > MAX_ERRORS)
                {
                    Send_Byte(CA);
                    Send_Byte(CA);
                    return 0;
                }
                Send_Byte(CRC16);
                break;
            }
            if (file_done != 0)
            {
                break;
            }
        }
        if (session_done != 0)
        {
            break;
        }
    }
    return (int32_t)size;
}
/**
  * @brief   通过 ymodem协议检测响应
  * @param  c: 测试字符
  * @retval none
  */
int32_t Ymodem_CheckResponse(uint8_t c)
{
    return 0;
}
/**
  * @brief   准备第一个数据包
  * @param  data：数据包
  *         fileName ：文件名
  *         length ：长度
  * @retval none
  */
void Ymodem_PrepareIntialPacket(uint8_t *data, const uint8_t *fileName, uint32_t *length)
{
    uint16_t i, j;
    uint8_t file_ptr[10];

    //制作头3个数据包
    data[0] = SOH;
    data[1] = 0x00;
    data[2] = 0xff;
    //文件名数据包有效数据
    for (i = 0; (fileName[i] != '\0') && (i < FILE_NAME_LENGTH); i++)
    {
        data[i + PACKET_HEADER] = fileName[i];
    }

    data[i + PACKET_HEADER] = 0x00;

    Int2Str(file_ptr, *length);
    for (j = 0, i = i + PACKET_HEADER + 1; file_ptr[j] != '\0';)
    {
        data[i++] = file_ptr[j++];
    }

    for (j = i; j < PACKET_SIZE + PACKET_HEADER; j++)
    {
        data[j] = 0;
    }
}
/**
  * @brief   准备数据包
  * @param  SourceBuf：数据源缓冲
  *         data：数据包
  *         pktNo ：数据包编号
  *         sizeBlk ：长度
  * @retval none
  */
void Ymodem_PreparePacket(uint8_t *SourceBuf, uint8_t *data, uint8_t pktNo, uint32_t sizeBlk)
{
    uint16_t i, size, packetSize;
    uint8_t *file_ptr;

    //制作头3个数据包
    packetSize = sizeBlk >= PACKET_1K_SIZE ? PACKET_1K_SIZE : PACKET_SIZE;
    size = sizeBlk < packetSize ? sizeBlk : packetSize;
    if (packetSize == PACKET_1K_SIZE)
    {
        data[0] = STX;
    }
    else
    {
        data[0] = SOH;
    }
    data[1] = pktNo;
    data[2] = (~pktNo);
    file_ptr = SourceBuf;

    //文件名数据包有效数据
    for (i = PACKET_HEADER; i < size + PACKET_HEADER; i++)
    {
        data[i] = *file_ptr++;
    }
    if (size <= packetSize)
    {
        for (i = size + PACKET_HEADER; i < packetSize + PACKET_HEADER; i++)
        {
            data[i] = 0x1A; //结束
        }
    }
}

/**
  * @brief  计算YModem数据包的总大小
  * @param  data ：数据
  *         size ：长度
  * @retval 数据包的总大小
  */
uint8_t CalChecksum(const uint8_t *data, uint32_t size)
{
    uint32_t sum = 0;
    const uint8_t *dataEnd = data + size;
    while (data < dataEnd)
        sum += *data++;
    return sum & 0xffu;
}
/**
  * @brief  通过ymodem协议传输一个数据包
  * @param  data ：数据地址指针
  *         length：长度
  * @retval none
  */
void Ymodem_SendPacket(uint8_t *data, uint16_t length)
{
    uint16_t i;
    i = 0;
    while (i < length)
    {
        Send_Byte(data[i]);
        i++;
    }
}
/**
  * @brief  通过ymodem协议传输一个文件
  * @param  buf ：数据地址指针
  *         sendFileName ：文件名
  *         sizeFile：文件长度
  * @retval 0：成功
  */
uint8_t Ymodem_Transmit(uint8_t *buf, const uint8_t *sendFileName, uint32_t sizeFile)
{

    uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
    uint8_t FileName[FILE_NAME_LENGTH];
    uint8_t *buf_ptr, tempCheckSum;
    uint16_t tempCRC, blkNumber;
    uint8_t receivedC[2], CRC16_F = 0, i;
    uint32_t errors, ackReceived, size = 0, pktSize;

    errors = 0;
    ackReceived = 0;
    for (i = 0; i < (FILE_NAME_LENGTH - 1); i++)
    {
        FileName[i] = sendFileName[i];
    }
    CRC16_F = 1;

    //准备第一个数据包
    Ymodem_PrepareIntialPacket(&packet_data[0], FileName, &sizeFile);

    do
    {
        //发送数据包
        Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);
        //发送CRC校验
        if (CRC16_F)
        {
            tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
            Send_Byte(tempCRC >> 8);
            Send_Byte(tempCRC & 0xFF);
        }
        else
        {
            tempCheckSum = CalChecksum(&packet_data[3], PACKET_SIZE);
            Send_Byte(tempCheckSum);
        }

        //等待响应
        if (Receive_Byte(&receivedC[0], 10000) == 0)
        {
            if (receivedC[0] == ACK)
            {
                //数据包正确传输
                ackReceived = 1;
            }
        }
        else
        {
            errors++;
        }
    } while (!ackReceived && (errors < 0x0A));

    if (errors >= 0x0A)
    {
        return errors;
    }
    buf_ptr = buf;
    size = sizeFile;
    blkNumber = 0x01;

    //1024字节的数据包发送
    while (size)
    {
        //准备下一个数据包
        Ymodem_PreparePacket(buf_ptr, &packet_data[0], blkNumber, size);
        ackReceived = 0;
        receivedC[0] = 0;
        errors = 0;
        do
        {
            //发送下一个数据包
            if (size >= PACKET_1K_SIZE)
            {
                pktSize = PACKET_1K_SIZE;
            }
            else
            {
                pktSize = PACKET_SIZE;
            }
            Ymodem_SendPacket(packet_data, pktSize + PACKET_HEADER);
            //发送CRC校验
            if (CRC16_F)
            {
                tempCRC = Cal_CRC16(&packet_data[3], pktSize);
                Send_Byte(tempCRC >> 8);
                Send_Byte(tempCRC & 0xFF);
            }
            else
            {
                tempCheckSum = CalChecksum(&packet_data[3], pktSize);
                Send_Byte(tempCheckSum);
            }

            //等待响应
            if ((Receive_Byte(&receivedC[0], 100000) == 0) && (receivedC[0] == ACK))
            {
                ackReceived = 1;
                if (size > pktSize)
                {
                    buf_ptr += pktSize;
                    size -= pktSize;
                    if (blkNumber == (FLASH_IMAGE_SIZE / 1024))
                    {
                        return 0xFF; //错误
                    }
                    else
                    {
                        blkNumber++;
                    }
                }
                else
                {
                    buf_ptr += pktSize;
                    size = 0;
                }
            }
            else
            {
                errors++;
            }
        } while (!ackReceived && (errors < 0x0A));
        //如果没响应10次就返回错误
        if (errors >= 0x0A)
        {
            return errors;
        }
    }
    ackReceived = 0;
    receivedC[0] = 0x00;
    errors = 0;
    do
    {
        Send_Byte(EOT);
        //发送 (EOT);
        //等待回应
        if ((Receive_Byte(&receivedC[0], 10000) == 0) && receivedC[0] == ACK)
        {
            ackReceived = 1;
        }
        else
        {
            errors++;
        }
    } while (!ackReceived && (errors < 0x0A));

    if (errors >= 0x0A)
    {
        return errors;
    }
    //准备最后一个包
    ackReceived = 0;
    receivedC[0] = 0x00;
    errors = 0;

    packet_data[0] = SOH;
    packet_data[1] = 0;
    packet_data[2] = 0xFF;

    for (i = PACKET_HEADER; i < (PACKET_SIZE + PACKET_HEADER); i++)
    {
        packet_data[i] = 0x00;
    }

    do
    {
        //发送数据包
        Ymodem_SendPacket(packet_data, PACKET_SIZE + PACKET_HEADER);
        //发送CRC校验
        tempCRC = Cal_CRC16(&packet_data[3], PACKET_SIZE);
        Send_Byte(tempCRC >> 8);
        Send_Byte(tempCRC & 0xFF);

        //等待响应
        if (Receive_Byte(&receivedC[0], 10000) == 0)
        {
            if (receivedC[0] == ACK)
            {
                //包传输正确
                ackReceived = 1;
            }
        }
        else
        {
            errors++;
        }

    } while (!ackReceived && (errors < 0x0A));
    //如果没响应10次就返回错误
    if (errors >= 0x0A)
    {
        return errors;
    }

    do
    {
        Send_Byte(EOT);
        //发送 (EOT);
        //等待回应
        if ((Receive_Byte(&receivedC[0], 10000) == 0) && receivedC[0] == ACK)
        {
            ackReceived = 1;
        }
        else
        {
            errors++;
        }
    } while (!ackReceived && (errors < 0x0A));

    if (errors >= 0x0A)
    {
        return errors;
    }
    return 0; //文件传输成功
}
