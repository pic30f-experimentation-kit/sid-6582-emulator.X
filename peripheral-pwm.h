#ifndef PERIPHERAL_PWM_H
#define	PERIPHERAL_PWM_H

// At selected pre-scaler and Fosc...
#define PWM_PERIOD       0xFF   // So duty cycle is exactly 8bits

#ifndef __ASSEMBLER__
/**
 * Initializes PWM peripheral.
 */
void initializePWM();  

#endif /* __ASSEMBLER__ */

#endif /* PERIPHERAL_PWM_H */

