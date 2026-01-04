#include "buffer.h"

#ifndef SHARED_H
#define	SHARED_H

typedef enum {
    NOTHING = 0,
    BUTTON_1_PUSHED,
    BUTTON_2_PUSHED,
    ADC_1_CONVERSION,
    ADC_2_CONVERSION,
    ADC_3_CONVERSION
} SharedEvents;

typedef struct {
    int adc1Value;
    int adc2Value;
    int adc3Value;
} Shared;

/**
 * A shared data, so interrupts can provide data to main thread.
 */
extern Shared sharedData;

/**
 * A shared buffer, so interrupts can notify of changes in shared data to main thread.
 */
extern Buffer sharedBuffer;

#endif	/* SHARED_H */

