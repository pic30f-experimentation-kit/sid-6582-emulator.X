#include "peripheral-pwm.h"
#include "peripheral-adc.h"
#include "test.h"
#include "buffer.h"
#include "sid.h"
#include "shared.h"
#include "peripheral-ic.h"
#include "sid-controller.h"

#include <xc.h>

#pragma config FOSC_FOSFPR = FRC_PLL16     // FRC w/PLL 16x 
#pragma config FOSC_FCKSMEN = CSW_FSCM_OFF // Sw Disabled, Mon Disabled 
#pragma config FICD_ICS = ICS_PGD          // Use PGC/EMUC and PGD/EMUD
#pragma config FWDT_WDT = WDT_OFF          // Disable watchdog

#ifndef TEST

int main() {
    // Initialize functions
    sidInitialize();
    sidControllerInitialize();

    // Set CPU base level of interrupts:
    CORCONbits.IPL3 = 0;    // Any interruption with level higher...
    SRbits.IPL = 0;         // ... than 0 is serviced.

    // Initialize peripherals
    initializePWM();
    initializeADC();
    initializeIC();
    
    // Forever...
    for(;;) {
        while(bufferIsEmpty(&sharedBuffer));
        int event = bufferRead(&sharedBuffer);
        sidControllerProcessSharedEvent(event);
    }
}

#else
int main() {
    testInitialize();
    testBuffer();
    testSid();
    testSidController();
    testReport();
    while(1);
}
#endif
