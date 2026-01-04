#include "buffer.h"
#include "shared.h"
#include "peripheral-ic.h"

#include <xc.h>

void __attribute__((interrupt, no_auto_psv)) _IC1Interrupt(void) {
    // Reset the interruption flag:
    IFS0bits.IC1IF = 0;  

    // Count the number of interruptions:
    bufferWrite(&sharedBuffer, BUTTON_1_PUSHED);
}

void __attribute__((interrupt, no_auto_psv)) _IC2Interrupt(void) {
    // Reset the interruption flag:
    IFS0bits.IC2IF = 0;  

    // Count the number of interruptions:
    bufferWrite(&sharedBuffer, BUTTON_2_PUSHED);
}

void initializeIC() {
    // IC1 and IC2 have to be inputs:
    TRISDbits.TRISD8 = 1;
    TRISDbits.TRISD9 = 1;

    // In this case we don't actually care about measuring time between inputs
    // so there is no need to configure specifically a timer.
    
    // Enable Input Capture 2:
    IC2CONbits.ICSIDL = 1;  // Input capture module will halt in CPU Idle mode
    IC2CONbits.ICTMR = 1;   // Timer 2 contents are captured on capture event
    IC2CONbits.ICI = 0;     // Interrupt on every capture event
    IC2CONbits.ICM = 0b010; // Capture mode, every falling edge
    IEC0bits.IC2IE = 1;     // Enable interrupts for Input Capture
    IPC1bits.IC2IP = 1;     // Input capture has the lowest priority interrupts.

    // Enable Input Capture 1:
    IC1CONbits.ICSIDL = 1;  // Input capture module will halt in CPU Idle mode
    IC1CONbits.ICTMR = 1;   // Timer 2 contents are captured on capture event
    IC1CONbits.ICI = 0;     // Interrupt on every capture event
    IC1CONbits.ICM = 0b010; // Capture mode, every falling edge
    IEC0bits.IC1IE = 1;     // Enable interrupts for Input Capture
    IPC0bits.IC1IP = 1;     // Input capture has the lowest priority interrupts.    
}
