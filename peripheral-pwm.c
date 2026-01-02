#include "peripheral-pwm.h"
#include "sid.h"

#include <xc.h>

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0;    // Clear interrupt flag    
    OC1RS = computeSid(); // Store into DC buffer register
}

void initializePWM() {    
    // We generate PWM with OC1
    TRISBbits.TRISB9 = 0;   // OC1 is output
    ADPCFGbits.PCFG9 = 1;   // AN9/RB9 is configured as digital
    
    // We use Timer2
    OC1CONbits.OCTSEL = 0;  // Select Timer 2 for OC1
    OC1R = 0x00;            // Immediate edge raising.
    
    // Set PWM period.
    T2CONbits.TCKPS = 0;    // Pre-scaler to 1:1
    PR2 = PWM_PERIOD;       // Period register
    
    // Enable interrupts -> interrupt when timer for period is reached
    IPC1bits.T2IP = 2;      // Priority 2 for time interrupts
    IFS0bits.T2IF = 0;      // Clear interrupt flag for timer 2
    IEC0bits.T2IE = 1;      // Enable interrupts for timer 2
    
    // Start timer 2:
    T2CONbits.TON = 1;
    
    // Start PWM 1: 
    OC1CONbits.OCM = 0b110;
}