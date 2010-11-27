/*********************************************************************
 *
 * Code testing the basic functionality of STM32 on VL discovery kit
 * The code displays message via UART1 using printf mechanism
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
 * Kubik                15.11.2010 Debug now goes to UART2
 ********************************************************************/

//-------------------------------------------------------------------
// Includes
#include <stddef.h>
#include <stdio.h>
#include <sys/unistd.h>
#include "stm32f10x.h"
#include "STM32_Discovery.h"
#include "debug.h"
#include "uart.h"

//-------------------------------------------------------------------
// Defines

//---------------------------------------------------------------------------
// Static variables

//---------------------------------------------------------------------------
// Local functions

// Redirecting of printf to UARTx - this works for Atollic
// fd selects stdout or stderr - that's UARTx or UARTd
int _write_r(void *reent, int fd, char *ptr, size_t len) {
	size_t counter = len;
	USART_TypeDef *Usart;

	if(fd == STDOUT_FILENO) {			// stdout goes to UARTx
		Usart = UARTx;
	} else if(fd == STDERR_FILENO) {
		Usart = UARTd;					// stderr goes to UARTd
	} else {
		return len;
	}

	while(counter-- > 0) {				// Send the character from the buffer to UART
		while (USART_GetFlagStatus(Usart, USART_FLAG_TXE) == RESET);
		USART_SendData(Usart, (uint8_t) (*ptr));
		ptr++;
	}

	return len;
}


/*******************************************************************************
* Function Name  : delay
* Description    : Inserts a time delay.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void delay(void)
{
  vu32 i = 0;

  for(i = 0xFF; i != 0; i--)
  {
  }
}


//---------------------------------------------------------------------------
// Local functions

int main(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_ClocksTypeDef RCC_ClockFreq;

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
	// Configure peripherals used - basically enable their clocks to enable them
	//

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

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

    // none used

	//
	// UART initialization
	//

    InitializeUarts();

    //
    // Main program loop
    //

    // Show welcome message
    iprintf("UART test " __DATE__ "\r\n");

    RCC_GetClocksFreq(&RCC_ClockFreq);
    iprintf("SYSCLK = %ld  HCLK = %ld  PCLK1 = %ld  PCLK2 = %ld ADCCLK = %ld\r\n", RCC_ClockFreq.SYSCLK_Frequency,
												RCC_ClockFreq.HCLK_Frequency,
												RCC_ClockFreq.PCLK1_Frequency,
												RCC_ClockFreq.PCLK2_Frequency,
												RCC_ClockFreq.ADCCLK_Frequency);


    // Testing debug output, it goes to UART2
    DEBUG(("DEBUG test output: %d\r\n", 1));

    //
    // Configuring quadrature encoder using TIM4 and pins PB6 & PB7
    //

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_SetAutoreload(TIM4, 0xffff);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_Cmd(TIM4, ENABLE);
    uint16_t tim4_counter = 0;

    iprintf("\r\n");
    while(1) {
    	tim4_counter = TIM_GetCounter(TIM4);
    	iprintf("\r%5d ", tim4_counter >> 2);
    	delay();
    	delay();
    	delay();
    	delay();
    }

    // Green LED on as we reached end of program
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);

    // and loop forever
    while(1);
}
