/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 16/05/2003
* Description        : This program demonstrates how to use the UART with the
*                      STR71x software library.
********************************************************************************
* History:
* 13/01/06 : V3.1
* 24/05/05 : V3.0
* 30/11/04 : V2.0
* 16/05/03 : Created
*******************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include <stdio.h>
#include "71x_lib.h"
#include "gpio.h"

#define UART0_Rx_Pin (0x0001<<8)   /* TQFP 64: pin N° 63 , TQFP 144 pin N° 143 */
#define UART0_Tx_Pin (0x0001<<9)   /* TQFP 64: pin N° 64 , TQFP 144 pin N° 144 */

#define UART1_Rx_Pin (0x0001<<10)  /* TQFP 64: pin N° 1  , TQFP 144 pin N° 1 */
#define UART1_Tx_Pin (0x0001<<11)  /* TQFP 64: pin N° 2  , TQFP 144 pin N° 3 */

#define UART2_Rx_Pin (0x0001<<13)  /* TQFP 64: pin N° 5  , TQFP 144 pin N° 9 */
#define UART2_Tx_Pin (0x0001<<14)  /* TQFP 64: pin N° 6  , TQFP 144 pin N° 10 */

#define UART3_Rx_Pin (0x0001<<1)   /* TQFP 64: pin N° 52 , TQFP 144 pin N° 123 */
#define UART3_Tx_Pin (0x0001<<0)   /* TQFP 64: pin N° 53 , TQFP 144 pin N° 124 */

#define Use_UART2

   #ifdef Use_UART0
     #define  UARTX  UART0
     #define  UARTX_Rx_Pin  UART0_Rx_Pin
     #define  UARTX_Tx_Pin  UART0_Tx_Pin
     #define  UARTX_Periph  UART0_Periph
   #endif /* Use_UART0 */

   #ifdef Use_UART1
     #define  UARTX  UART1
     #define  UARTX_Rx_Pin  UART1_Rx_Pin
     #define  UARTX_Tx_Pin  UART1_Tx_Pin
     #define  UARTX_Periph  UART1_Periph
   #endif /* Use_UART1 */

   #ifdef Use_UART2
     #define  UARTX  UART2
     #define  UARTX_Rx_Pin  UART2_Rx_Pin
     #define  UARTX_Tx_Pin  UART2_Tx_Pin
     #define  UARTX_Periph  UART2_Periph
   #endif /* Use_UART2 */

   #ifdef Use_UART3
     #define  UARTX  UART3
     #define  UARTX_Rx_Pin  UART3_Rx_Pin
     #define  UARTX_Tx_Pin  UART3_Tx_Pin
     #define  UARTX_Periph  UART3_Periph
   #endif /* Use_UART3 */


u16 i;
u16 UARTStatus;
u8 bBuffer[4]={'S','T','R','7'};


int putchar(int ch) {
    UART_ByteSend(UARTX, (u8 *) &ch);
    return ch;
}

int main(void)
{
  #ifdef DEBUG
  debug();
  #endif
/* GPIO peripheral configuration -------------------------------------------*/

  /* Configure the GPIO pins */
  GPIO_Config(GPIO0, UARTX_Tx_Pin, GPIO_AF_PP);
  GPIO_Config(GPIO0, UARTX_Rx_Pin, GPIO_IN_TRI_CMOS);

/* UART peripheral configuration -------------------------------------------*/

  /*  Configure the UART X */
  /*  Turn UARTX on */
  UART_OnOffConfig(UARTX, ENABLE);
  /* Disable FIFOs */
  UART_FifoConfig (UARTX, DISABLE);
  /* Reset the UART_RxFIFO */
  UART_FifoReset  (UARTX , UART_RxFIFO);
  /* Reset the UART_TxFIFO */
  UART_FifoReset  (UARTX , UART_TxFIFO);
  /* Disable Loop Back */
  UART_LoopBackConfig(UARTX , DISABLE);
                                         /* Configure the UARTX as following:
                                            - Baudrate = 9600 Bps
                                            - No parity
                                            - 8 data bits
                                            - 1 stop bit */
  UART_Config(UARTX,9600,UART_NO_PARITY,UART_1_StopBits,UARTM_8D);
   /* Enable Rx */
  UART_RxConfig(UARTX ,ENABLE);


    printf("T_MMC " __DATE__ "\r\n");

    printf("Configuring SPI\r\n");

//-----------------------------------

/* Configure GPI00 on mode Alternate function Push Pull */
//   GPIO_Config (GPIO0, 0x0070, GPIO_AF_PP); //AF
//   GPIO_BitWrite(GPIO0, 7, 1);
//   GPIO_Config (GPIO0, 0x0080, GPIO_IN_TRI_CMOS); //AF
//   GPIO_Config (GPIO1, 0x0100, GPIO_OUT_PP);
   //IN_TRI_CMOS
//   GPIO_Config (GPIO0, 0x0080, GPIO_AF_PP);

//
///* -------------------------------------------
//Configure BSPI1 as a Master
//------------------------------------------- */
///* Initialize BSPI1 */
//   BSPI_Init   ( BSPI1 ) ;
//
///* Configure Baud rate Frequency: ---> APB1/6 */
////   BSPI_ClockDividerConfig ( BSPI1, 6);
//      BSPI_ClockDividerConfig ( BSPI1, 32);
//
///* Enable BSPI1 */
//   BSPI_Enable ( BSPI1 , ENABLE );
//
///* Configure BSPI1 as a Master */
//   BSPI_MasterEnable ( BSPI1,ENABLE);//
//
///* Configure the clock to be active low */
//   BSPI_ClkActiveHigh(BSPI1,DISABLE); //
//
///* Enable capturing the first Data sample on the first edge of SCK */
//   BSPI_ClkFEdge(BSPI1,DISABLE);
//
///* Set the word length to 8 bit */
//   BSPI_8bLEn(BSPI1, ENABLE);
//
///*  Configure the depth of transmit to 1 word */
//   BSPI_TrFifoDepth(BSPI1, 1);
///* Point on the word to transit */
//
//   BSPI1->CLK = 0x0020;
//   BSPI1->CSR1 = 0x0102; //00000001 00000010
//   BSPI1->CSR2 = 0x0000; //00000000 00000000
//   BSPI1->CSR1 = 0x0103; //00000001 00000011
//
// /* Configure GPI00 on mode Alternate function Push Pull */
//    GPIO_Config (GPIO0, 0x00FF, GPIO_AF_PP);
//
// /* -------------------------------------------
// Configure BSPI0 as a Master
// ------------------------------------------- */
// /* Enable the BSPI0 interface */
//    BSPI_BSPI0Conf(ENABLE);
//
// /* Initialize BSPI0 */
//    BSPI_Init   ( BSPI0 ) ;
//
// /* Configure Baud rate Frequency: ---> APB1/6 */
//    BSPI_ClockDividerConfig ( BSPI0, 6);
//
// /* Enable BSPI0 */
//    BSPI_Enable ( BSPI0 , ENABLE );
//
// /* Configure BSPI0 as a Master */
//    BSPI_MasterEnable ( BSPI0,ENABLE);
//
// /* Configure the clock to be active high */
//    BSPI_ClkActiveHigh(BSPI0,ENABLE);
//
// /* Enable capturing the first Data sample on the first edge of SCK */
//    BSPI_ClkFEdge(BSPI0,DISABLE);
//
// /* Set the word length to 16 bit */
//    BSPI_8bLEn(BSPI0,ENABLE);
//
// /*  Configure the depth of transmit to 9 words */
//    BSPI_TrFifoDepth(BSPI0,9);
// /* Point on the word to transit */



//   GPIO_Config (GPIO0, 0x0080, GPIO_IPUPD_WP);
//   GPIO_BitWrite(GPIO0, 7, 0);

/* Configure GPI00 on mode Alternate function Push Pull */
//    GPIO_Config (GPIO0, 0x007F, GPIO_AF_PP);
//    GPIO_Config (GPIO0, 0x0080, GPIO_HI_AIN_TRI);
//   GPIO_BitWrite(GPIO0, 3, 1);
//   GPIO_Config (GPIO0, 0x0008, GPIO_OUT_PP);


  /* GPIO configuration ------------------------------------------------------*/
  /* Configure MOSIx, MISOx, and SCLKx pins as Alternate function Push Pull */
  GPIO_Config (GPIO0, 0x0077, GPIO_AF_PP);

  /* Configure nSSx pins mode as Input Tristate CMOS */
  GPIO_Config (GPIO0, 0x0088, GPIO_IN_TRI_CMOS);

/* -------------------------------------------
Configure BSPI0 as a Master
------------------------------------------- */
/* Enable the BSPI0 interface */
   BSPI_BSPI0Conf(ENABLE);

/* Initialize BSPI0 */
   BSPI_Init   ( BSPI0 ) ;

/* Configure Baud rate Frequency: ---> APB1/6 */
   BSPI_ClockDividerConfig ( BSPI0, 6);

/* Enable BSPI0 */
   BSPI_Enable ( BSPI0 , ENABLE );

/* Configure BSPI0 as a Master */
   BSPI_MasterEnable ( BSPI0,ENABLE);

/* Configure the clock to be active high */
   BSPI_ClkActiveHigh(BSPI0,ENABLE);

/* Enable capturing the first Data sample on the first edge of SCK */
   BSPI_ClkFEdge(BSPI0,DISABLE);

/* Set the word length to 16 bit */
   BSPI_8bLEn(BSPI0,ENABLE);

/*  Configure the depth of transmit to 9 words */
   BSPI_TrFifoDepth(BSPI0,9);
/* Point on the word to transit */


/* -------------------------------------------
Configure BSPI1 as a Master
------------------------------------------- */
/* Initialize BSPI1 */
   BSPI_Init   ( BSPI1 ) ;

/* Configure Baud rate Frequency: ---> APB1/6 */
   BSPI_ClockDividerConfig ( BSPI1, 6);

/* Enable BSPI1 */
   BSPI_Enable ( BSPI1 , ENABLE );

/* Configure BSPI1 as a Master */
   BSPI_MasterEnable ( BSPI1,ENABLE);

/* Configure the clock to be active high */
   BSPI_ClkActiveHigh(BSPI1,ENABLE);

/* Enable capturing the first Data sample on the first edge of SCK */
   BSPI_ClkFEdge(BSPI1,DISABLE);

/* Set the word length to 16 bit */
   BSPI_8bLEn(BSPI1,ENABLE);

/*  Configure the depth of transmit to 9 words */
   BSPI_TrFifoDepth(BSPI1,9);
/* Point on the word to transit */





   while(1) {
    BSPI_WordSend(BSPI0, 0xAA);     // Sends one byte (BSPI0 is cfg'ed as 8bit, thus only one byte)
    BSPI_WordSend(BSPI1, 0xCC);
    putchar('.');
   }

//    while(1) {
//        GPIO_BitWrite(GPIO0, 4, 1);
//        GPIO_BitWrite(GPIO0, 4, 0);
//        GPIO_BitWrite(GPIO0, 5, 1);
//        GPIO_BitWrite(GPIO0, 5, 0);
//        GPIO_BitWrite(GPIO0, 6, 1);
//        GPIO_BitWrite(GPIO0, 6, 0);
//        GPIO_BitWrite(GPIO0, 7, 1);
//        GPIO_BitWrite(GPIO0, 7, 0);
//        GPIO_BitWrite(GPIO1, 8, 1);
//        GPIO_BitWrite(GPIO1, 8, 0);
//    }

//-----------------------------------

    while(1);


//   while(1)
//   {
//     for(i=0;i<4;i++)
//     {
//       UART_ByteSend(UARTX, (u8 *)&bBuffer[i]);
//       /* wait until the data transmission is finished */
//       while(!((UART_FlagStatus(UARTX)) & UART_TxEmpty));
//     }
//
//     for(i=0;i<4;i++)
//     {
//     	/*  wait until data is received */
//       while(!(UART_FlagStatus(UARTX) & UART_RxBufFull));
//       /* Get the received data, set the guard time to 0xFF */
//       UARTStatus = UART_ByteReceive(UARTX, (u8 *)&bBuffer[i], 0xFF);
//     }
//    }
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
