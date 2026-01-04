#include "sid.h"
#include "test.h"
#include "random.h"

#define FN_BIT_SHIFTING 3

#define NOISE_BIT_VALUE 0x200000

uint8_t computeTriangularSignal(SignalGenerator *signalGenerator) {
    signalGenerator->status.counter += signalGenerator->properties.frecuency;
    signalGenerator->status.counter += signalGenerator->properties.frecuency;
    uint32_t signal;
    if (signalGenerator->status.counter & 0x1000000) {
        signal = ~signalGenerator->status.counter;
    } else {
        signal = signalGenerator->status.counter;
    }
    signal &= 0xFFFFFF;
    return signal >> 16;
}

uint8_t computeSawtoothSignal(SignalGenerator *signalGenerator) {
    signalGenerator->status.counter += signalGenerator->properties.frecuency;
    uint32_t signal = signalGenerator->status.counter & 0xFFFFFF;
    return signal >> 16;
}

uint8_t computePulseSignal(SignalGenerator *signalGenerator) {
    signalGenerator->status.counter += signalGenerator->properties.frecuency;
    uint32_t signal = signalGenerator->status.counter & 0xFFFFFF;
    if (signal > signalGenerator->properties.pulseWidth) {
        return 255;
    }
    return 0;
}

uint8_t computeNoiseSignal(SignalGenerator *signalGenerator) {
    signalGenerator->status.counter += signalGenerator->properties.frecuency;
    uint32_t noiseBitValue = signalGenerator->status.counter & NOISE_BIT_VALUE;
    if (noiseBitValue != signalGenerator->status.noiseBitValue) {
        signalGenerator->status.noiseBitValue = noiseBitValue;
        signalGenerator->status.noiseValue = wyhash8();
    }
    return signalGenerator->status.noiseValue;
}

uint8_t computeTestSignal(SignalGenerator *signalGenerator) {
    return (signalGenerator->properties.frecuency >> (FN_BIT_SHIFTING + 8)) & 0xFF;
}

uint8_t computeSignal(SignalGenerator *signalGenerator) {
    switch (signalGenerator->properties.functionType) {
        case TRIANGULAR:
            return computeTriangularSignal(signalGenerator);
        case SAWTOOTH:
            return computeSawtoothSignal(signalGenerator);
        case PULSE:
            return computePulseSignal(signalGenerator);
        case NOISE:
            return computeNoiseSignal(signalGenerator);
        case XTEST:
            return computeTestSignal(signalGenerator);
    }
    return 0;
}


uint8_t computeChannel(SidChannel *sidChannel) {
    return computeSignal(&(sidChannel->signalGenerator));
}

static SidChannel sidChannel;

void sidInitialize() {
    sidChannel.signalGenerator.status.counter = 0;
    sidChannel.signalGenerator.status.noiseBitValue = 0;
    sidChannel.signalGenerator.status.noiseValue = 0;
}

uint8_t computeSid() {
    return computeChannel(&sidChannel);
}

void set_FREQ_LO(uint8_t value) {
    uint32_t frequency = sidChannel.signalGenerator.properties.frecuency >> FN_BIT_SHIFTING;
    frequency &= 0xFF00;
    frequency |= value;
    sidChannel.signalGenerator.properties.frecuency = frequency << FN_BIT_SHIFTING;
}

uint8_t get_FREQ_LO() {
    uint32_t frequency = sidChannel.signalGenerator.properties.frecuency >> FN_BIT_SHIFTING;
    return (uint8_t) frequency;
}

void set_FREQ_HI(uint8_t value) {
    uint32_t v = value;
    v <<= 8;
    uint32_t frequency = sidChannel.signalGenerator.properties.frecuency >> FN_BIT_SHIFTING;
    frequency &= 0x00FF;
    frequency |= v;
    sidChannel.signalGenerator.properties.frecuency = frequency << FN_BIT_SHIFTING;
}

uint8_t get_FREQ_HI() {
    uint32_t frequency = sidChannel.signalGenerator.properties.frecuency >> FN_BIT_SHIFTING;
    frequency >>= 8;
    return (uint8_t) frequency;
}

void set_PW_LO(uint8_t value) {
    uint32_t v = value;
    v <<= 12;
    uint32_t pulseWidth = sidChannel.signalGenerator.properties.pulseWidth;
    //              00HLL000
    pulseWidth &= 0x00F00000;
    pulseWidth |= v;
    sidChannel.signalGenerator.properties.pulseWidth = pulseWidth;
}

uint8_t get_PW_LO() {
    uint32_t pulseWidth = sidChannel.signalGenerator.properties.pulseWidth >> 12;
    pulseWidth &= 0xFF;
    return (uint8_t) pulseWidth;
}

void set_PW_HI(uint8_t value) {
    uint32_t v = value;
    v <<= 20;
    uint32_t pulseWidth = sidChannel.signalGenerator.properties.pulseWidth;
    //              00HLL000
    pulseWidth &= 0x000FF000;
    pulseWidth |= v;
    sidChannel.signalGenerator.properties.pulseWidth = pulseWidth;
}

uint8_t get_PW_HI() {
    uint32_t pulseWidth = sidChannel.signalGenerator.properties.pulseWidth >> 20;
    pulseWidth &= 0x0F;
    return (uint8_t) pulseWidth;
}

void set_CONTROL_REG(uint8_t value) {
    if (value & TRIANGULAR) {
        sidChannel.signalGenerator.properties.functionType = TRIANGULAR;
    }
    if (value & SAWTOOTH) {
        sidChannel.signalGenerator.properties.functionType = SAWTOOTH;
    }
    if (value & PULSE) {
        sidChannel.signalGenerator.properties.functionType = PULSE;
    }
    if (value & NOISE) {
        sidChannel.signalGenerator.properties.functionType = NOISE;
    }
    if (value & XTEST) {
        sidChannel.signalGenerator.properties.functionType = XTEST;
    }
}

uint8_t get_CONTROL_REG() {
   return (uint8_t) sidChannel.signalGenerator.properties.functionType;
}

void set_ATACK_DECAY(uint8_t value) {
    // TODO
}

void set_SUSTAIN_RELEASE(uint8_t value) {
    // TODO
}

#ifdef TEST

#include <stdio.h>
// The FN value that is handy for most tests is the one that
// increases the output of exactly 1 unit each call.
// As the internal counter is 24 bits and the output is its most significant byte,
// then setting to 1 the most significant byte obtains the FN_TEST_VALUE:
#define FN_VALUE_FOR_TEST 0x00010000  

// The bit shifting is required to move up the actual frequency so it agrees with specifications,
// considering that the microcontroller is slower than the original SID.
// When we test, we have to reverse the move up, so we attain the actual test frequency.
#define FREQUENCY_FOR_TEST (FN_VALUE_FOR_TEST >> FN_BIT_SHIFTING)


#define NOISE_CHANGE (NOISE_BIT_VALUE / FN_VALUE_FOR_TEST)

static char testId[20];

void sid_can_compute_sawtooth_signal() {
    SignalGenerator signalGenerator;
    signalGenerator.properties.functionType = SAWTOOTH;
    signalGenerator.properties.frecuency = FN_VALUE_FOR_TEST;
    signalGenerator.status.counter = 0;

    for(int n = 1; n <= 255; n++) {
        sprintf(testId, "SIDSWT_%03d", n);
        assertEquals(testId, n, computeSignal(&signalGenerator));
    }
    assertEquals("SIDSWT_888", 0, computeSignal(&signalGenerator));
    assertEquals("SIDSWT_999", 1, computeSignal(&signalGenerator));
}

void sid_can_set_the_frequency_using_registers() {
    uint32_t actual;
    
    set_FREQ_HI(0xBB);
    set_FREQ_LO(0xAA);
    actual = sidChannel.signalGenerator.properties.frecuency;
    actual >>= FN_BIT_SHIFTING;
    assertEqualULongs("SIR_FR1", 0xBBAA, actual);
    assertEquals("SIR_FR1h", 0xBB, get_FREQ_HI());
    assertEquals("SIR_FR1l", 0xAA, get_FREQ_LO());

    set_FREQ_HI(0x80);
    set_FREQ_LO(0xFF);
    actual = sidChannel.signalGenerator.properties.frecuency;
    actual >>= FN_BIT_SHIFTING;
    assertEqualULongs("SIR_FR2", 0x80FF, actual);
    assertEquals("SIR_FR2h", 0x80, get_FREQ_HI());
    assertEquals("SIR_FR2l", 0xFF, get_FREQ_LO());

    set_FREQ_HI(0xFF);
    set_FREQ_LO(0x10);
    actual = sidChannel.signalGenerator.properties.frecuency;
    actual >>= FN_BIT_SHIFTING;
    assertEqualULongs("SIR_FR3", 0xFF10, actual);
    assertEquals("SIR_FR3h", 0xFF, get_FREQ_HI());
    assertEquals("SIR_FR3l", 0x10, get_FREQ_LO());

    set_FREQ_HI(0x00);
    set_FREQ_LO(0x01);
    actual = sidChannel.signalGenerator.properties.frecuency;
    actual >>= FN_BIT_SHIFTING;
    assertEqualULongs("SIR_FR4", 0x0001, actual);
    assertEquals("SIR_FR4h", 0x00, get_FREQ_HI());
    assertEquals("SIR_FR4l", 0x01, get_FREQ_LO());

    set_FREQ_HI(0x01);
    set_FREQ_LO(0x00);
    actual = sidChannel.signalGenerator.properties.frecuency;
    actual >>= FN_BIT_SHIFTING;
    assertEqualULongs("SIR_FR5", 0x0100, actual);
    assertEquals("SIR_FR5h", 0x01, get_FREQ_HI());
    assertEquals("SIR_FR5l", 0x00, get_FREQ_LO());
}

void sid_can_set_the_pulse_width_using_registers() {
    uint32_t actual;
    
    set_PW_HI(0x0B);
    set_PW_LO(0xAA);
    actual = sidChannel.signalGenerator.properties.pulseWidth;
    assertEqualULongs("SIR_PW1", 0xBAA000, actual);
    assertEquals("SIR_PW1h", 0x0B, get_PW_HI());
    assertEquals("SIR_PW1l", 0xAA, get_PW_LO());

    set_PW_HI(0x08);
    set_PW_LO(0xFF);
    actual = sidChannel.signalGenerator.properties.pulseWidth;
    assertEqualULongs("SIR_PW2", 0x8FF000, actual);
    assertEquals("SIR_PW2h", 0x08, get_PW_HI());
    assertEquals("SIR_PW2l", 0xFF, get_PW_LO());

    set_PW_HI(0x0F);
    set_PW_LO(0x10);
    actual = sidChannel.signalGenerator.properties.pulseWidth;
    assertEqualULongs("SIR_PW3", 0xF10000, actual);
    assertEquals("SIR_PW3h", 0x0F, get_PW_HI());
    assertEquals("SIR_PW3l", 0x10, get_PW_LO());

    set_PW_HI(0x00);
    set_PW_LO(0x01);
    actual = sidChannel.signalGenerator.properties.pulseWidth;
    assertEqualULongs("SIR_PW4", 0x001000, actual);
    assertEquals("SIR_PW4h", 0x00, get_PW_HI());
    assertEquals("SIR_PW4l", 0x01, get_PW_LO());

    set_PW_HI(0x01);
    set_PW_LO(0x00);
    actual = sidChannel.signalGenerator.properties.pulseWidth;
    assertEqualULongs("SIR_PW5", 0x100000, actual);
    assertEquals("SIR_PW5h", 0x01, get_PW_HI());
    assertEquals("SIR_PW5l", 0x00, get_PW_LO());
}

void sid_can_compute_sawtooth_signal_using_registers() {
    sidInitialize();
    set_FREQ_LO(FREQUENCY_FOR_TEST & 0xFF);
    set_FREQ_HI(FREQUENCY_FOR_TEST >> 8);
    set_CONTROL_REG(SAWTOOTH);
    assertEquals("SRSWT", SAWTOOTH, get_CONTROL_REG());
    
    sidInitialize();
    for(int n = 1; n <= 255; n++) {
        sprintf(testId, "SIRSWT_%03d", n);
        assertEquals(testId, n, computeSid());
    }
    assertEquals("SIRSWT_888", 0, computeSid());
    assertEquals("SIRSWT_999", 1, computeSid());
}

void sid_can_compute_triangular_signal() {
    SignalGenerator signalGenerator;
    signalGenerator.properties.functionType = TRIANGULAR;
    signalGenerator.properties.frecuency = FN_VALUE_FOR_TEST;
    signalGenerator.status.counter = 0;

    int expected = 0;
    for(int n = 0; n < 127; n++) {
        sprintf(testId, "SIDTR1_%03d", n);
        expected += 2;
        assertEquals(testId, expected, computeSignal(&signalGenerator));
    }

    expected =255 ;
    assertEquals("SIDTRI_255", expected, computeSignal(&signalGenerator));

    for(int n = 128; n < 255; n++) {
        sprintf(testId, "SIDTR2_%03d", n);
        expected -= 2;
        assertEquals(testId, expected, computeSignal(&signalGenerator));
    }
    assertEquals("SIDTRI_888", 0, computeSignal(&signalGenerator));
    assertEquals("SIDTRI_999", 2, computeSignal(&signalGenerator));
}

void sid_can_compute_triangular_signal_using_registers() {
    sidInitialize();
    set_FREQ_LO(FREQUENCY_FOR_TEST & 0xFF);
    set_FREQ_HI(FREQUENCY_FOR_TEST >> 8);
    set_CONTROL_REG(TRIANGULAR);
    assertEquals("SIRTR", TRIANGULAR, get_CONTROL_REG());

    int expected = 0;
    for(int n = 0; n < 127; n++) {
        sprintf(testId, "SIRTR1_%03d", n);
        expected += 2;
        assertEquals(testId, expected, computeSid());
    }

    expected =255 ;
    assertEquals("SIRTRI_255", expected, computeSid());

    for(int n = 128; n < 255; n++) {
        sprintf(testId, "SIRTR2_%03d", n);
        expected -= 2;
        assertEquals(testId, expected, computeSid());
    }
    assertEquals("SIRTRI_888", 0, computeSid());
    assertEquals("SIRTRI_999", 2, computeSid());
}

void sid_can_compute_pulse_signal() {
    SignalGenerator signalGenerator;
    signalGenerator.properties.functionType = PULSE;
    signalGenerator.properties.frecuency = FN_VALUE_FOR_TEST;
    signalGenerator.properties.pulseWidth = 0x800000;
    signalGenerator.status.counter = 0;

    for(int n = 0; n <= 127; n++) {
        sprintf(testId, "SIDPUL_%03d", n);
        assertEquals(testId, 0, computeSignal(&signalGenerator));
    }

    for(int n = 129; n < 256; n++) {
        sprintf(testId, "SIDPUL_%03d", n);
        assertEquals(testId, 255, computeSignal(&signalGenerator));
    }
    assertEquals("SIDPUL_999", 0, computeSignal(&signalGenerator));
}

void sid_can_compute_pulse_signal_using_registers() {
    sidInitialize();
    set_FREQ_LO(FREQUENCY_FOR_TEST & 0xFF);
    set_FREQ_HI(FREQUENCY_FOR_TEST >> 8);
    set_PW_HI(0x08);
    set_PW_LO(0x00);
    set_CONTROL_REG(PULSE);
    assertEquals("SIRPUL", PULSE, get_CONTROL_REG());

    for(int n = 0; n <= 127; n++) {
        sprintf(testId, "SIRPUL_%03d", n);
        assertEquals(testId, 0, computeSid());
    }

    for(int n = 129; n < 256; n++) {
        sprintf(testId, "SIRPUL_%03d", n);
        assertEquals(testId, 255, computeSid());
    }
    assertEquals("SIRPUL_999", 0, computeSid());
}

void sid_can_compute_test_signal() {
    SignalGenerator signalGenerator;
    signalGenerator.properties.functionType = XTEST;
    signalGenerator.properties.frecuency = 0xAABBCC << FN_BIT_SHIFTING;

    assertEquals("SIDTST", 0xBB, computeSignal(&signalGenerator));
}

void sid_can_compute_test_signal_using_registers() {
    sidInitialize();
    set_FREQ_LO(0xCC);
    set_FREQ_HI(0xBB);
    set_CONTROL_REG(XTEST);

    assertEquals("SIRTST", 0xBB, computeSid());
}

void sid_can_compute_noise_signal() {
    SignalGenerator signalGenerator;
    signalGenerator.properties.frecuency = FN_VALUE_FOR_TEST;
    signalGenerator.properties.functionType = NOISE;
    signalGenerator.status.counter = 0;
    signalGenerator.status.noiseBitValue = 0;
    signalGenerator.status.noiseValue = 0;


    for(int n = 1; n < NOISE_CHANGE; n++) {
        sprintf(testId, "SIDNOI_%03d", n);
        assertEquals(testId, 0, computeSignal(&signalGenerator));
    }
    int noiseValue = computeSignal(&signalGenerator);
    assertNotZero("SIDNOI_XXX", noiseValue);

    for(int n = 2; n < NOISE_CHANGE; n++) {
        sprintf(testId, "SIDNOX_%03d", n);
        assertEquals(testId, noiseValue, computeSignal(&signalGenerator));
    }
}

void sid_can_compute_noise_signal_using_registers() {
    sidInitialize();
    set_FREQ_LO(FREQUENCY_FOR_TEST & 0xFF);
    set_FREQ_HI(FREQUENCY_FOR_TEST >> 8);
    set_CONTROL_REG(NOISE);

    for(int n = 1; n < NOISE_CHANGE; n++) {
        sprintf(testId, "SIRNOI_%03d", n);
        assertEquals(testId, 0, computeSid());
    }
    int noiseValue = computeSid();
    assertNotZero("SIRNOI_XXX", noiseValue);

    for(int n = 2; n < NOISE_CHANGE; n++) {
        sprintf(testId, "SIRNOX_%03d", n);
        assertEquals(testId, noiseValue, computeSid());
    }
}

void testSid() {
    sid_can_compute_sawtooth_signal();
    sid_can_compute_triangular_signal();
    sid_can_compute_pulse_signal();
    sid_can_compute_test_signal();
    sid_can_compute_noise_signal();

    sid_can_set_the_frequency_using_registers();
    sid_can_set_the_pulse_width_using_registers();
            
    sid_can_compute_sawtooth_signal_using_registers();
    sid_can_compute_triangular_signal_using_registers();
    sid_can_compute_pulse_signal_using_registers();
    sid_can_compute_test_signal_using_registers();
    sid_can_compute_noise_signal_using_registers();
}

#endif