#include "peripheral-pwm.h"
#include "test.h"
#include "buffer.h"
#include "sid.h"

#include <xc.h>

#pragma config FOSC_FOSFPR = FRC_PLL16     // FRC w/PLL 16x 
#pragma config FOSC_FCKSMEN = CSW_FSCM_OFF // Sw Disabled, Mon Disabled 
#pragma config FICD_ICS = ICS_PGD          // Use PGC/EMUC and PGD/EMUD
#pragma config FWDT_WDT = WDT_OFF          // Disable watchdog

#ifndef TEST

int main() {
    // Initialization of variables
    resetSidStatus();
    set_CONTROL_REG(TRIANGULAR);
    set_PW_HI(0x08);
    set_PW_LO(0x00);
    set_FREQ_HI(0x11);
    set_FREQ_LO(0x25);
            
    // Initialization of modules
    initializePWM();
    
    // Forever...
    for(;;) {
    }
}

#else
int main() {
    testInitialize();
    testBuffer();
    testSid();
    testReport();
    while(1);
}
#endif
