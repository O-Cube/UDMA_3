//****************************************************************************************
// Author: Obed Oyandut
// Date  : 09.08.2022
// Version: 1
//****************************************************************************************
// This Program works on TIVA TM4C1294XL Evaluation Board.
// The program performs a memory scatter using udma. It copies parts of an array
// into five other arrays using udma.
//****************************************************************************************
//                     ! IMPORTANT !
// This program runs endless. Stop with the "Red Square Button"
// in Debug Mode (Terminate = CTRL + F2)
//****************************************************************************************
// Include the Header File for controller tm4c1294ncpdt

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//************************************************************************************************************************
// Length of control table and sub arrays
//************************************************************************************************************************

#define BASE 256
#define LEN 10

//************************************************************************************************************************
// Text: Extract of the Raven by Edgar Allan Poe.
// Five characters of array line1 is copied into line2 up to line6.
//************************************************************************************************************************


volatile char line1[] = "Once upon a midnight dreary, while I pondered, weak and weary,";
volatile char line2[LEN];
volatile char line3[LEN];
volatile char line4[LEN];
volatile char line5[LEN];
volatile char line6[LEN];


volatile char destBuffer[LEN];

//************************************************************************************************************************
// The control table is declared as unsigned int. This means the length is 256. Channel 30 is used. The offset of the pri-
//mary control structure of channel 30 is 120.
//************************************************************************************************************************

volatile unsigned int utable[BASE];

//************************************************************************************************************************
// The atable array is copied into the alternate control section of the base control table. It is then executed by the udma
//*************************************************************************************************************************

volatile unsigned int atable[] = { (unsigned int)&line1[4], (unsigned int)&line2[4], 0x00000045, 0x00000000,
                                   (unsigned int)&line1[9], (unsigned int)&line3[4], 0x00000045, 0x00000000,
                                   (unsigned int)&line1[14], (unsigned int)&line4[4], 0x00000045, 0x00000000,
                                   (unsigned int)&line1[19], (unsigned int)&line5[4], 0x00000045, 0x00000000,
                                   (unsigned int)&line1[24], (unsigned int)&line6[4], 0x00000042, 0x00000000 };


//************************************************************************************************************************
// Interrupt is executed after udma transfer is done.
//************************************************************************************************************************

void udmaInterrupt(void) {

    line2[5] = '\0';
    printf("Line2: %s.\n", line2);
    line3[5] = '\0';
    printf("Line3: %s.\n", line3);
    line4[5] = '\0';
    printf("Line4: %s.\n", line4);
    line5[5] = '\0';
    printf("Line5: %s.\n", line5);
    line6[5] = '\0';
    printf("Line6: %s.\n", line6);
}

//************************************************************************************************************************
// Prints a notification if udma encounters error.
//************************************************************************************************************************

void udmaInterruptError(void) {

    UDMA_ERRCLR_R |= 0x01;
    printf("Udma encounters an error...\n");
}

//*************************************************************************************************************************
// Configure and enable channel 30
//*************************************************************************************************************************

void dmaconfig(void) {
    SYSCTL_RCGCDMA_R |= 0x01;
    while(!(SYSCTL_PRDMA_R & 0x01));
    UDMA_CFG_R |= 0x01;
    UDMA_PRIOSET_R |= (0x01<<30);
    UDMA_ALTCLR_R |= (0x01<<30);
    UDMA_USEBURSTCLR_R |= 0x40000000;
    UDMA_REQMASKCLR_R |= 0x40000000;
    UDMA_CTLBASE_R |= (unsigned int)utable;
    UDMA_ENASET_R |= 0x40000000;
    UDMA_SWREQ_R |= 0x40000000;
}

//**************************************************************************************************************************
// Enable udma interrupts in NVIC
//**************************************************************************************************************************

void enableNVIC(void) {

    NVIC_EN1_R |= (0x03<<44-32);

}



//**************************************************************************************************************************
// The udma process is triggered by the primary control structure. Udma copies content into alternate control structure and
// performs the copy operations.
//**************************************************************************************************************************

void udmaTransfer(void) {

    utable[120] = (unsigned int)&atable[19];
    utable[121] = (unsigned int)&utable[251];
    utable[122] = 0xAA008134;

    enableNVIC();
    dmaconfig();
}

void main(void) {

    udmaTransfer();
    while(1) {

    }
}
