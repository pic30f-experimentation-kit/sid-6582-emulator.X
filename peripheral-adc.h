#ifndef PERIPHERAL_ADC_H
#define	PERIPHERAL_ADC_H

#define ADC_MAX 4095
#define ADC_MIN 0
#define VREF 5

#define MAXVOLTAGE = 5;
#define MINVOLTAGE = 0;

/**
 * Initializes ADC peripheral.
 */
void initializeADC();

#endif /* PERIPHERAL_ADC_H */
