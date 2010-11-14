/*********************************************************************
 *
 * UART printf support - circular buffer and characted sending
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 *********************************************************************
 * FileName:    uart_buff.h
 * Depends:
 * Processor:   STM32F100RBT6B
 *
 * Author               Date       Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Kubik                14.11.2010 Initial code
 ********************************************************************/

#ifndef _UART_BUFF_H_
#define _UART_BUFF_H_

#define UART_BUFFER_SIZE    256
#define UARTx USART1

inline void UartBufferInit(void);
inline bool UartBufferEmpty(void);
inline bool UartBufferFull(void);
bool PutIntoBuffer(uint8_t c);
bool GetFromBuffer(uint8_t *c);
void UartSendByte(uint8_t c);

#endif
