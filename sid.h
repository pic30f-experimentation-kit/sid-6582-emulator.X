#ifndef SID__H
#include <stdint.h>

/**
 * Enumerates the available register to control one SID channel.
 */
typedef enum {
    /**
     * FREQ_LO and FREQ_HI form a 16-bit number 
     * which linearly controls the frequency of Oscillator.
     */
    FREQ_LO = 0,
    /**
     * FREQ_LO and FREQ_HI form a 16-bit number 
     * which linearly controls the frequency of Oscillator.
     */
    FREQ_HI = 1,
    /**
     * PW_LO and PW_HI form a 12-bit number (bits 4-7 of PW HI are not used) 
     * which linearly controls the Pulse Width (duty cycle) of the Pulse waveform.
     */
    PW_LO = 2,
            
    /**
     * PW_LO and PW_HI form a 12-bit number (bits 4-7 of PW HI are not used) 
     * which linearly controls the Pulse Width (duty cycle) of the Pulse waveform.
     */
    PW_HI = 3,
            
    /**
     * This register contains eight control bits 
     * which select various options on channel.
     * <dl>
     *  <dt>GATE (Bit 0)</dt>
     *  <dd>The GATE bit controls the Envelope Generator. 
     *  When this bit is set to a one, the Envelope Generator is Gated (triggered) 
     *  and the ATTACK/ DECAY/SUSTAIN cycle is initiated. 
     *  When the bit is reset to a zero, the RELEASE cycle begins. </dd>
     *  <dt>SYNC (Bit 1)</dt>
     *  <dd>When set to one, synchronizes the fundamental frequency of the channel
     *  with the fundamental frequency of channel 3, producing "Hard Sync" effects.</dd>
     *  <dt>RING MOD (Bit 2)</dt>
     *  <dd>When set to a one, performs a ring modulation with channels 1 and 3, 2 and 1, 3 and 2.</dd>
     *  <dt>TRI (Bit 4)</dt>
     *  <dd>When set to one, selects the triangle waveform.</dd>
     *  <dt>SAW (Bit 5)</dt>
     *  <dd>When set to one, selects the Sawtooth waveform.</dd>
     *  <dt>PULSE (Bit 6)</dt>
     *  <dd>When set to one, selects the Pulse waveform.</dd>
     *  <dt>NOISE (Bit 7)</dt>
     *  <dd>When set to one, selects the Noise output waveform.</dd>
     * </dl>
     */
    CONTROL_REG = 4,
    /**
     * Bits 4—7 of this register (ATKO— ATK3) select 1 of 16 ATTACK rates for the Envelope Generator. 
     * Bits 0-3 (DCY0-DCY3) select 1 of 16 DECAY rates for the Envelope Generator.
     */
    ATACK_DECAY = 5,
    /**
     * Bits 4—7 of this register (STNO— STN3) select 1 of 16 SUSTAIN levels for the Envelope Generator.
     * Bits 0-3 (RLS0-RLS3) select 1 of 16 RELEASE rates for the Envelope Generator. 
     */
    SUSTAIN_RELEASE = 6
} SidRegister;

typedef enum {
    TRIANGULAR = 0b10000000,
    SAWTOOTH   = 0b01000000,
    PULSE      = 0b00100000,
    NOISE      = 0b00010000,
    XTEST      = 0b00001000
} FunctionType;

typedef struct {
    FunctionType functionType;
    uint32_t frecuency;
    uint32_t pulseWidth;
} SignalGeneratorProperties;

typedef struct {
    uint32_t counter;
    uint32_t noiseBitValue;
    uint32_t noiseValue;
} SignalGeneratorStatus;

typedef struct {
    SignalGeneratorProperties properties;
    SignalGeneratorStatus status;
} SignalGenerator;

typedef enum {
    OFF,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE
} EnvelopePhase;

typedef struct {
    uint32_t a;
    uint32_t d;
    uint32_t s;
    uint32_t r;
} EnvelopeGeneratorProperties;

typedef struct {
    EnvelopePhase phase;
    uint32_t counter;
} EnvelopeGeneratorStatus;

typedef struct {
    EnvelopeGeneratorProperties properties;
    EnvelopeGeneratorStatus status;
} EnvelopeGenerator;

typedef struct {
    uint32_t enabled;
} SidChannelConfiguration;

typedef struct {
    SidChannelConfiguration configuration;
    SignalGenerator signalGenerator;
    EnvelopeGenerator envelopeGenerator;
} SidChannel;

typedef struct {
    SidChannel channelA;
    SidChannel channelB;
    SidChannel channelC;
} Sid;

uint8_t computeSid();

void resetSidStatus();

void set_FREQ_LO(uint8_t value);
void set_FREQ_HI(uint8_t value);
void set_PW_LO(uint8_t value);
void set_PW_HI(uint8_t value);
void set_CONTROL_REG(uint8_t value);

#ifdef TEST
void testSid();
#endif

#endif