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
 * FileName:    uart_buff.c
 * Depends:
 * Processor:   STM32F100RBT6B
 *
 * Author               Date       Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Kubik                14.11.2010 Initial code
 ********************************************************************/

#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "STM32_Discovery.h"
#include "debug.h"
#include "uart_buff.h"

static uint8_t buffer[UART_BUFFER_SIZE];
volatile static uint8_t start, end;

//-------------------------------------------------------------------
inline void UartBufferInit(void) {

    start = end = 0;
}

//-------------------------------------------------------------------
inline bool UartBufferEmpty(void) {

    return (start == end) ? TRUE : FALSE;
}

//-------------------------------------------------------------------
inline bool UartBufferFull(void) {

    return (((end + 1) % UART_BUFFER_SIZE) == start) ? TRUE : FALSE;
}

//-------------------------------------------------------------------
bool PutIntoBuffer(uint8_t c) {

    if(!UartBufferFull()) {
        buffer[end] = c;
        end = (end + 1) % UART_BUFFER_SIZE;
        return TRUE;
    } else {
        return FALSE;
    }
}

//-------------------------------------------------------------------
bool GetFromBuffer(uint8_t *c) {

    if(!UartBufferEmpty()) {
        *c = buffer[start];
        start = (start + 1) % UART_BUFFER_SIZE;
        return TRUE;
    } else {
        return FALSE;
    }
}

//-------------------------------------------------------------------
// UartSendByte
//
// This needs a bit of explanation:
// We start with UART interrupt disabled. If this is the first character (buffer
// empty, nothing being sent right now), just send it and enable interrupt, so the
// interrupt handler will be called later to check buffer for other characters to
// send.
// If UART is busy, that means we can't use it and have to use buffer, so
// make sure we have some space in buffer first.
// The interrupt routine disables
// interrupt if it has nothing more to send.

void UartSendByte(uint8_t c) {

    //
    // We have a couple of cases here:
    // - if buffer is empty and UART can send, then send directly
    // - if UART is busy, then put the char into buffer
    //

    // If the buffer is empty and UART can accept a character, send the character directly and enable UART TXE interrupt
    if(UartBufferEmpty() && (USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == SET)) {
        USART_ITConfig(UARTx, USART_IT_TXE, ENABLE);
        USART_SendData(UARTx, c);
        USART_ITConfig(UARTx, USART_IT_TXE, ENABLE);
    // else if UART is busy, use buffer - if there is some space in buffer, just put the char into it, else wait
    } else if(USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == RESET) {
        while(UartBufferFull()) {};
        PutIntoBuffer(c);
    }
}
