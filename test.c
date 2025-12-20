#ifdef TEST

#include "test.h"

#include <stdio.h>

#include <xc.h>

/** Number of passed tests, since initialization. */
int testsPassed;

/** Number of failed tests, since initialization. */
int testsFailed;

/**
 * Sends a char to the UART.
 * This method is called by <code>printf</code> behind the scenes.
 * If a terminal is connected to RX / TX, or simulator is configured to output
 * UART 1, then text is displayed.
 * @param data Ascii code of the char to display.
 */
void putch(char data) {
   while( ! U1STAbits.UTXBF);
   U1TXREG = data; 
}

/**
 * Initializes UART for asynchronous output.
 */
void initializationUART() {
    // Initialize the UxBRG register for the appropriate baud rate.
    U1BRG = 95;

    // Set the number of data bits, number of Stop bits, and parity selection.
    // The most common data format is 8 bits, no parity and one Stop
    // bit (denoted as 8, N, 1), which is the default (POR) setting.
    U1MODEbits.PDSEL = 0;   // 8 Bits, no parity
    U1MODEbits.STSEL = 0;   // 1 Stop bit.

    // Simulator UART is connected to the regular pins
    U1MODEbits.ALTIO = 0;   // Use regular pins - U1TX / U1RX

    // Enable UART1:
    U1MODEbits.UARTEN = 1;  // Enable UART1
    U1STAbits.UTXEN = 1;    // Enable UART1 transmitter (after enabling UART).
}

void testInitialize() {
    initializationUART();
    testsFailed = 0;
    testsPassed = 0;
    printf("Starting tests:\r\n");
}

bool assertEquals(const char *testId, int expected, int actual) {
    if (actual != expected) {
        printf("Test %s: expected is %d, but actual is %d\r\n", 
                testId, expected, actual);
        testsFailed++;
        return false;
    }
    testsPassed++;
    return true;
}

bool assertEqualULongs(const char *testId, unsigned long expected, unsigned long actual) {
    if (actual != expected) {
        printf("Test %s: expected is %lx, but actual is %lx\r\n", 
                testId, expected, actual);
        testsFailed++;
        return false;
    }
    testsPassed++;
    return true;
}

bool assertWithinBounds(const char *testId, int actual, int min, int max) {
    if ( (actual < min) || (actual > max) ) {
        printf("Test %s: actual [%d] outside bounds: [%d] to [%d]\r\n",
                testId, actual, min, max);
        testsFailed ++;
        return false;
    }
    testsPassed++;
    return true;
}

bool assertNotZero(const char *testId, int actual) {
    if (actual == 0) {
        printf("Test %s: actual is zero, but it should not.\r\n", testId);
        testsFailed ++;
        return false;
    }
    testsPassed++;
    return true;
}

bool assertZero(const char *testId, int actual) {
    if (actual != 0) {
        printf("Test %s: actual is %d, but it should be zero.\r\n", testId, actual);
        testsFailed ++;
        return false;
    }
    testsPassed ++;
    return true;
}

void testReport() {
    printf("Finished tests:\r\n");
    printf("- Tests passed: %d\r\n", testsPassed);
    printf("- Tests failed: %d\r\n", testsFailed);
}

#endif
