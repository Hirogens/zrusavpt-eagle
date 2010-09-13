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
#include "sd_raw.h"
#include "partition.h"
#include "fat.h"

#define UART0_Rx_Pin (0x0001<<8)        /* TQFP 64: pin N° 63 , TQFP 144 pin N° 143 */
#define UART0_Tx_Pin (0x0001<<9)        /* TQFP 64: pin N° 64 , TQFP 144 pin N° 144 */

#define UART1_Rx_Pin (0x0001<<10)       /* TQFP 64: pin N° 1  , TQFP 144 pin N° 1 */
#define UART1_Tx_Pin (0x0001<<11)       /* TQFP 64: pin N° 2  , TQFP 144 pin N° 3 */

#define UART2_Rx_Pin (0x0001<<13)       /* TQFP 64: pin N° 5  , TQFP 144 pin N° 9 */
#define UART2_Tx_Pin (0x0001<<14)       /* TQFP 64: pin N° 6  , TQFP 144 pin N° 10 */

#define UART3_Rx_Pin (0x0001<<1)        /* TQFP 64: pin N° 52 , TQFP 144 pin N° 123 */
#define UART3_Tx_Pin (0x0001<<0)        /* TQFP 64: pin N° 53 , TQFP 144 pin N° 124 */

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
u8 bBuffer[4] = { 'S', 'T', 'R', '7' };


int putchar(int ch) {
    UART_ByteSend(UARTX, (u8 *) & ch);
    return ch;
}


//#define SPI_CS_LOW()    GPIO_BitWrite(GPIO0, 12, 0)
//#define SPI_CS_HIGH()    {for (i=0; i<0x0008; i++); GPIO_BitWrite(GPIO0, 12, 1);}


//---------------------------------------------------------------------
void mmcSendCmd(const char cmd, unsigned long data, const char crc) {
    char frame[6];
    char temp;
    int i;

    frame[0] = (cmd | 0x40);
    for(i = 3; i >= 0; i--) {
        temp = (char) (data >> (8 * i));
        frame[4 - i] = (temp);
    }
    frame[5] = (crc);
    for(i = 0; i < 6; i++)
        BSPI_WordSend(BSPI1, frame[i]);
}


char mmcGetResponse(void) {
    //Response comes 1-8bytes after command
    //the first bit will be a 0
    //followed by an error code
    //data will be 0xff until response
    int i = 0;

    char response;

    while(i <= 64) {
        BSPI_WordSend(BSPI1, 0xff);
        if(response == 0x00)
            break;
        if(response == 0x01)
            break;
        i++;
    }
    return response;
}

#define trigger() {u16 i; GPIO_BitWrite(GPIO1, 9, 1); for(i = 0; i < 32; i++); GPIO_BitWrite(GPIO1, 9, 0);}
#define LED_OFF     GPIO_BitWrite(GPIO1, 8, 1);
#define LED_ON     GPIO_BitWrite(GPIO1, 8, 0);


uint8_t print_disk_info(const struct fat_fs_struct * fs) {
    if(!fs)
        return 0;

    struct sd_raw_info disk_info;

    if(!sd_raw_get_info(&disk_info))
        return 0;

    printf("manuf:  0x%2.2X\r\n", disk_info.manufacturer);
    printf("oem:    %s\r\n", disk_info.oem);
    printf("prod:   %s\r\n", disk_info.product);
    printf("rev:    %2.2X\r\n", disk_info.revision);
    printf("serial: 0x%8.8X\r\n", disk_info.serial);
    printf("date:   %d/%d\r\n", disk_info.manufacturing_month, disk_info.manufacturing_year);
    printf("size:   %dMB\r\n", disk_info.capacity / 1024 / 1024);
    printf("copy:   %d\r\n", disk_info.flag_copy);
    printf("wr.pr.: %d %d\r\n", disk_info.flag_write_protect_temp, disk_info.flag_write_protect);
    printf("format: %d\r\n", disk_info.format);
    printf("free:   %d / %d\r\n", fat_get_fs_free(fs), fat_get_fs_size(fs));

    return 1;
}


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
    UART_FifoConfig(UARTX, DISABLE);
    /* Reset the UART_RxFIFO */
    UART_FifoReset(UARTX, UART_RxFIFO);
    /* Reset the UART_TxFIFO */
    UART_FifoReset(UARTX, UART_TxFIFO);
    /* Disable Loop Back */
    UART_LoopBackConfig(UARTX, DISABLE);
    /* Configure the UARTX as following:
       - Baudrate = 9600 Bps
       - No parity
       - 8 data bits
       - 1 stop bit */
    UART_Config(UARTX, 9600, UART_NO_PARITY, UART_1_StopBits, UARTM_8D);
    /* Enable Rx */
    UART_RxConfig(UARTX, ENABLE);


    printf("T_MMC " __DATE__ "\r\n");

//    printf("Configuring SPI\r\n");

/* GPIO configuration ------------------------------------------------------*/
/* Configure MOSIx, MISOx, and SCLKx pins as Alternate function Push Pull */
    GPIO_Config(GPIO0, 0x0070, GPIO_AF_PP);

/* Configure nSSx pins mode as Input Weak PU/PD */
    GPIO_Config(GPIO0, 0x0080, GPIO_IPUPD_WP);
    GPIO_BitWrite(GPIO0, 3, 1);
    GPIO_BitWrite(GPIO0, 7, 1);

    SPI_CS_HIGH();
    GPIO_Config(GPIO0, 1 << 12, GPIO_OUT_PP);

/* -------------------------------------------
Configure BSPI1 as a Master
------------------------------------------- */
/* Initialize BSPI1 */
    BSPI_Init(BSPI1);

/* Configure Baud rate Frequency: ---> APB1/6 */
    BSPI_ClockDividerConfig(BSPI1, 6);

/* Enable BSPI1 */
    BSPI_Enable(BSPI1, ENABLE);

/* Configure BSPI1 as a Master */
    BSPI_MasterEnable(BSPI1, ENABLE);

/* Configure the clock to be active low */
    BSPI_ClkActiveHigh(BSPI1, DISABLE);

/* Enable capturing the first Data sample on the first edge of SCK */
    BSPI_ClkFEdge(BSPI1, DISABLE);

/* Set the word length to 8 bit */
    BSPI_8bLEn(BSPI1, ENABLE);

/*  Configure the depth of transmit to 1 word/byte */
    BSPI_TrFifoDepth(BSPI1, 1);
/* Point on the word to transit */

//    LED_ON;

    if(!sd_raw_init()) {
        printf("MMC/SD initialization failed\n");
        sd_raw_init();
    }

    /* open first partition */
    struct partition_struct *partition = partition_open(sd_raw_read,
                                                        sd_raw_read_interval,
#if SD_RAW_WRITE_SUPPORT
                                                        sd_raw_write,
                                                        sd_raw_write_interval,
#else
                                                        0,
                                                        0,
#endif
                                                        0);

    if(!partition) {
        /* If the partition did not open, assume the storage device
         * is a "superfloppy", i.e. has no MBR.
         */
        partition = partition_open(sd_raw_read, sd_raw_read_interval,
#if SD_RAW_WRITE_SUPPORT
                                   sd_raw_write, sd_raw_write_interval,
#else
                                   0, 0,
#endif
                                   -1);
        if(!partition) {
            printf("opening partition failed\n");
        }
    }

    /* open file system */
    struct fat_fs_struct *fs = fat_open(partition);

    if(!fs) {
        printf("opening filesystem failed\n");
        while(1);
    }

    /* open root directory */
    struct fat_dir_entry_struct directory;

    fat_get_dir_entry_of_path(fs, "/", &directory);

    struct fat_dir_struct *dd = fat_open_dir(fs, &directory);

    if(!dd) {
        printf("opening root directory failed\n");
        while(1);
    }

    /* print some card information as a boot message */
    print_disk_info(fs);

    /* print directory listing */
    struct fat_dir_entry_struct dir_entry;

    while(fat_read_dir(dd, &dir_entry)) {
        uint8_t spaces = sizeof(dir_entry.long_name) - strlen(dir_entry.long_name) + 4;

        printf("%s", dir_entry.long_name);
        putchar(dir_entry.attributes & FAT_ATTRIB_DIR ? '/' : ' ');
        while(spaces--)
            putchar(' ');
        printf("%d\n\r", dir_entry.file_size);
    }

    while(1);

}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
