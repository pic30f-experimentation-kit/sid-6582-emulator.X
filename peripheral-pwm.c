#include "peripheral-pwm.h"
#include "sid.h"

#include <xc.h>

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0;             //Clear interrupt flag    
    OC2RS = computeSid();          //Store into DC buffer register
    if (OC2RS != 0) {
        OC2CONbits.OCM = 0b100;    // PWM single pulse mode.
    }
}

void initializePWM() {    
    //we will use OC2
    TRISBbits.TRISB9 = 0; // OC2 is output
    ADPCFGbits.PCFG9 = 1; // AN9/RB9 is configured as digital
    
    //We will use Timer2
    OC2CONbits.OCTSEL = 0;
    
    //Set PWM period.
    T2CONbits.TCKPS = 0; //Pre scaler 1:1
    PR2 = PWM_PERIOD;    //Period register
    OC2R = 0;            // Immediate edge raising.
    
    //Enable interrupts -> interrupt when timer for period is reached
    IPC1bits.T2IP = 2; //priority 2 for time interrupts
    IFS0bits.T2IF = 0; //clear interrupt flag for timer 2
    IEC0bits.T2IE = 1; //Enable interrupts for timer 2
    T2CONbits.TON = 1; //Start Timer 2           
}
