#include "sid-controller.h"
#include "sid.h"
#include "shared.h"
#include "buffer.h"
#include "test.h"

static FunctionType functionType;

void sidControllerInitialize() {
    functionType = SAWTOOTH;
    set_CONTROL_REG(functionType);
}

typedef struct {
    unsigned lo: 8;
    unsigned hi: 8;
} HiLo;

typedef union {
    HiLo hiLo;
    unsigned int raw;
} HiLoRaw;


void sidControllerProcessSharedEvent(int sharedEvent) {
    HiLoRaw hiLoRaw;
    
    switch(sharedEvent) {
        case ADC_1_CONVERSION:
            hiLoRaw.raw = sharedData.adc1Value << 4;
            set_FREQ_HI(hiLoRaw.hiLo.hi);
            set_FREQ_LO(hiLoRaw.hiLo.lo);
            break;

        case ADC_2_CONVERSION:
            hiLoRaw.raw = sharedData.adc2Value;
            set_PW_HI(hiLoRaw.hiLo.hi);
            set_PW_LO(hiLoRaw.hiLo.lo);
            break;
            
        case BUTTON_1_PUSHED:
            switch(functionType) {
                case SAWTOOTH:
                    functionType = TRIANGULAR;
                    break;
                case TRIANGULAR:
                    functionType = PULSE;
                    break;
                case PULSE:
                    functionType = NOISE;
                    break;
                default:
                    functionType = SAWTOOTH;
            }
            set_CONTROL_REG(functionType);
            break;
            
        default:
            break;
    }
}

#ifdef TEST
void sid_controller_can_set_frecuency_based_on_ADC_1_CONVERSION() {
    sharedData.adc1Value = 0x0FED;
    sidControllerProcessSharedEvent(ADC_1_CONVERSION);
    assertEquals("SC_ADC_1h", 0xFE, get_FREQ_HI());
    assertEquals("SC_ADC_1l", 0xD0, get_FREQ_LO());
}

void sid_controller_can_set_pulse_width_based_on_ADC_2_CONVERSION() {
    sharedData.adc2Value = 0x0876;
    sidControllerProcessSharedEvent(ADC_2_CONVERSION);
    assertEquals("SC_ADC_2h", 0x08, get_PW_HI());
    assertEquals("SC_ADC_2l", 0x76, get_PW_LO());
}

void sid_controller_can_change_function_based_on_BUTTON_1() {
    sidControllerInitialize();
    assertEquals("SC_BUT_1", SAWTOOTH, get_CONTROL_REG());

    sidControllerProcessSharedEvent(BUTTON_1_PUSHED);
    assertEquals("SC_BUT_2", TRIANGULAR, get_CONTROL_REG());

    sidControllerProcessSharedEvent(BUTTON_1_PUSHED);
    assertEquals("SC_BUT_3", PULSE, get_CONTROL_REG());

    sidControllerProcessSharedEvent(BUTTON_1_PUSHED);
    assertEquals("SC_BUT_4", NOISE, get_CONTROL_REG());

    sidControllerProcessSharedEvent(BUTTON_1_PUSHED);
    assertEquals("SC_BUT_5", SAWTOOTH, get_CONTROL_REG());
}

void testSidController() {
    sid_controller_can_set_frecuency_based_on_ADC_1_CONVERSION();
    sid_controller_can_set_pulse_width_based_on_ADC_2_CONVERSION();
    sid_controller_can_change_function_based_on_BUTTON_1();
}
#endif
