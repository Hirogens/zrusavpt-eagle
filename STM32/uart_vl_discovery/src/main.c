/*********************************************************************
 *
 * Code testing the basic functionality of STM32 on VL discovery kit
 * The code displays message via UART1 using printf mechanism in interrupt
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 *********************************************************************
 * FileName:    main.c
 * Depends:
 * Processor:   STM32F100RBT6B
 *
 * Author               Date       Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Kubik                13.11.2010 Initial code
 * Kubik                14.11.2010 Added debug code
 * Kubik                14.11.2010 Added interrupt driven printf to UARTx
 ********************************************************************/

//-------------------------------------------------------------------
// Includes
#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "STM32_Discovery.h"
#include "debug.h"
#include "uart_buff.h"

//-------------------------------------------------------------------
// Defines

//---------------------------------------------------------------------------
// Static variables

//---------------------------------------------------------------------------
// Local functions

// Redirecting of printf to UARTx - this works for Atollic
int _write_r(void *reent, int fd, char *ptr, size_t len) {
    size_t counter = len;

    while(counter-- > 0) {
        UartSendByte((uint8_t) (*ptr));
        ptr++;
    }

    return len;
}

//---------------------------------------------------------------------------
// main
int main(void) {
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_ClocksTypeDef RCC_ClockFreq;
    NVIC_InitTypeDef NVIC_InitStructure;

    UartBufferInit();

    //
    // Clock initialization
    //

    // Output SYSCLK clock on MCO pin
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    RCC_MCOConfig(RCC_MCO_SYSCLK);

    //
    // NVIC configuration
    //

    // Configure the NVIC Preemption Priority Bits
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    // Enable the USARTx Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //BUGBUG
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //
    // Configure peripherals used - basically enable their clocks to enable them
    //

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    //
    // Configure LEDs - we use them as an indicator the platform is alive somehow
    //

    // PC8 is blue LED
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // PC9 is green LED - some setting inherited from blue LED config code!
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Blue LED on - we're running!
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);

    //
    // GPIO initialization
    //

    // Configure USART Tx as alternate function push-pull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART Rx as input floating
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //
    // UART initialization
    //

    // Prepare parameters first - traditional 115.2 / 8b / no parity, no flow control
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    // Configure and enable UARTx - note that interrupts are not enabled here yet!
    USART_Init(UARTx, &USART_InitStructure);
    USART_Cmd(UARTx, ENABLE);

    //
    // Main program loop
    //

    // Show welcome message
    printf("UART test " __DATE__ "\r\n");

    RCC_GetClocksFreq(&RCC_ClockFreq);
    printf("SYSCLK = %ld  HCLK = %ld  PCLK1 = %ld  PCLK2 = %ld ADCCLK = %ld\r\n", RCC_ClockFreq.SYSCLK_Frequency,
                                                RCC_ClockFreq.HCLK_Frequency,
                                                RCC_ClockFreq.PCLK1_Frequency,
                                                RCC_ClockFreq.PCLK2_Frequency,
                                                RCC_ClockFreq.ADCCLK_Frequency);


    // Green LED on as we reached end of program
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);

    // The following just tests DEBUG macro - if DEBUG is #defined, the text will be displayed
    DEBUG(("DEBUG test output: %d\r\n", 1));

    // and loop forever
    while(1);
}
