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


#define SPI_CS_LOW()    GPIO_BitWrite(GPIO0, 12, 0)
#define SPI_CS_HIGH()    {for (i=0; i<0x0008; i++); GPIO_BitWrite(GPIO0, 12, 1);}


//---------------------------------------------------------------------
void mmcSendCmd (const char cmd, unsigned long data, const char crc)
{
  char frame[6];
  char temp;
  int i;

  frame[0]=(cmd|0x40);
  for(i=3;i>=0;i--){
    temp=(char)(data>>(8*i));
    frame[4-i]=(temp);
  }
  frame[5]=(crc);
  for(i=0;i<6;i++)
    BSPI_WordSend(BSPI1, frame[i]);
}


char mmcGetResponse(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i=0;

  char response;

  while(i<=64)
  {
   BSPI_WordSend(BSPI1, 0xff);
   if(response==0x00)break;
   if(response==0x01)break;
   i++;
  }
  return response;
}

#define trigger() {u16 i; GPIO_BitWrite(GPIO1, 9, 1); for(i = 0; i < 32; i++); GPIO_BitWrite(GPIO1, 9, 0);}
#define LED_OFF     GPIO_BitWrite(GPIO1, 8, 1);
#define LED_ON     GPIO_BitWrite(GPIO1, 8, 0);

int main(void) {
    u16 i, b;

#ifdef DEBUG
    debug();
#endif

// Trigger
    GPIO_BitWrite(GPIO1, 9, 0);
    GPIO_Config(GPIO1, 1 << 9, GPIO_OUT_PP);

// LED
    LED_OFF;
    GPIO_Config(GPIO1, 1 << 8, GPIO_OUT_PP);


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

//    printf("Configuring SPI\r\n");

/* GPIO configuration ------------------------------------------------------*/
/* Configure MOSIx, MISOx, and SCLKx pins as Alternate function Push Pull */
    GPIO_Config (GPIO0, 0x0070, GPIO_AF_PP);

/* Configure nSSx pins mode as Input Weak PU/PD */
    GPIO_Config (GPIO0, 0x0080, GPIO_IPUPD_WP);
    GPIO_BitWrite(GPIO0, 3, 1);
    GPIO_BitWrite(GPIO0, 7, 1);

    SPI_CS_HIGH();
    GPIO_Config (GPIO0, 1 << 12, GPIO_OUT_PP);

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

/* Configure the clock to be active low */
    BSPI_ClkActiveHigh(BSPI1,DISABLE);

/* Enable capturing the first Data sample on the first edge of SCK */
    BSPI_ClkFEdge(BSPI1,DISABLE);

/* Set the word length to 8 bit */
    BSPI_8bLEn(BSPI1,ENABLE);

/*  Configure the depth of transmit to 1 word/byte */
    BSPI_TrFifoDepth(BSPI1,1);
/* Point on the word to transit */

    LED_ON;
    SPI_CS_HIGH();
    for(i=0;i<=9;i++) {
        BSPI_WordSend(BSPI1, 0xFF);
    }

    sd_raw_init();
    while(1);

    trigger();

    SPI_CS_LOW();

    mmcSendCmd(0x00,0,0x95);
    if(mmcGetResponse()!=0x01) {
       printf("no response\r\n");
    }

    char response=0x01;

    while(response==0x01) {
        printf("Sending Command 1");
        SPI_CS_HIGH();
        BSPI_WordSend(BSPI1, 0xff);
        SPI_CS_LOW();
        mmcSendCmd(0x01,0x00,0xff);
        response = mmcGetResponse();
    }
    SPI_CS_HIGH();                //!!!!!!!!!!!
    BSPI_WordSend(BSPI1, 0xff);
    printf("MMC INITIALIZED AND SET TO SPI MODE PROPERLY.\r\n");
    LED_OFF;

    while(1);// trigger();

    while(1) {
        SPI_CS_LOW();
        BSPI_WordSend(BSPI1, 0xCC);
        for (i=0; i<0x0008; i++);
        SPI_CS_HIGH();
//        putchar('.');
    }



}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
