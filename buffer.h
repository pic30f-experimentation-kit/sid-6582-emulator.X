#ifndef BUFFER_H
#define	BUFFER_H

#include <stdbool.h>

#define BUFFER_SIZE 32
#define VALUE_WHEN_EMPTY 0

typedef struct {
    int *in;
    int *out;
    int *limit;
    int buffer[BUFFER_SIZE];
} Buffer;

/**
 * Initializes the buffer.
 * Call this method before using the buffer.
 * @param buffer Pointer to the buffer.
 */
void bufferInitialize(Buffer *buffer);

/**
 * Writes an integer to the buffer.
 * @param buffer The buffer.
 * @param data The integer.
 */
void bufferWrite(Buffer *buffer, int data);

/**
 * Reads one integer from the buffer.
 * @param buffer The buffer.
 * @return The integer.
 */
int bufferRead(Buffer *buffer);

/**
 * Verifies if the buffer contains data.
 * @param buffer The buffer.
 * @return true if the buffer is empty.
 */
bool bufferIsEmpty(Buffer *buffer);

#ifdef TEST
void testBuffer();
#endif

#endif	/* BUFFER_H */

