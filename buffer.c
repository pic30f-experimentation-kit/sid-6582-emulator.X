#include "buffer.h"

#include "test.h"

/**
 * 
 * Reads the value stored in the position pointed at by *out pointer in the
 * Buffer structure
 * 
 * @param buffer - pointer to Buffer structure
 * @return integer value 
 */
void bufferInitialize(Buffer *buffer) {
    buffer->in = 0;
    buffer->out = 0;
}

/**
 * 
 * Evaluates the situation in which the next value read has already been read
 * and no new value was written
 * 
 * @param buffer - pointer to Buffer structure
 * @return true if no new value was written to the buffer after last read
 */

bool bufferIsEmpty(Buffer *buffer) {
    if (buffer->in == buffer->out){
        return true;        
    }
    return false;
}

/**
 * 
 * Writes a value in the position pointed at by *in pointer in the
 * Buffer structure
 * 
 * @param buffer - pointer to Buffer structure
 * @param data - integer value to be stored
 */
void bufferWrite(Buffer *buffer, int data) {
    
    //Write value
    buffer->buffer[buffer->in] = data;
    
    //Update control pointers 
    if (buffer->in >= BUFFER_SIZE - 1) {
        buffer->in = 0;
    }
    else {
        buffer->in++; 
    }    
}

/**
 * 
 * Reads the value stored in the position pointed at by *out pointer in the
 * Buffer structure
 * 
 * @param buffer - pointer to Buffer structure
 * @return integer value 
 */
int bufferRead(Buffer *buffer) {

    if (bufferIsEmpty(buffer)) {
        return VALUE_WHEN_EMPTY;
    }

    //Read Value
    int data = buffer->buffer[buffer->out];

    //Update control pointers
    if (buffer->out >= BUFFER_SIZE - 1) {
        buffer->out = 0;
    } else {
        buffer->out++;
    }

    return data;
}

#ifdef TEST

/*
 * Tests basic reading - writing functionalities of Buffer
 */
void test_can_write_and_read() {
    
    //Test values to read and write
    int firstValue  = 10;
    int secondValue = 20;
    
    //Creation of the buffer
    Buffer buffer;
    bufferInitialize(&buffer);
    
    //Write to the buffer
    bufferWrite(&buffer, firstValue);
    bufferWrite(&buffer, secondValue);
    
    assertEquals("BUFF_RE01", firstValue, bufferRead(&buffer));
    assertEquals("BUFF_RE02", secondValue, bufferRead(&buffer));
}

/*
 * Tests that no value will be read if it has already been read and no new value
 * has been added. (Reading faster than writing)
 */
void test_buffer_is_empty() {
    int firstValue = 10;
    
    //Create buffer
    Buffer buffer;
    bufferInitialize (&buffer);
    
    //Write and Read a value
    bufferWrite (&buffer,firstValue);
    bufferRead(&buffer);
    
    //Read again with no new value written after last read
    assertEquals("BUFF_EMPTY", VALUE_WHEN_EMPTY, bufferRead(&buffer));
    
}

/*
 * Tests that the buffer will store values in first position after the last 
 * position has been filled.
 */
void test_buffer_is_circular() {
    
    int value = 0;
    int testValue = 40;
    
    //Create the buffer
    Buffer buffer;
    bufferInitialize (&buffer);
    
    //Populate the buffer up to the last position
    for (int i = 0; i < BUFFER_SIZE; i++){
        bufferWrite (&buffer, value);
        value++;
        
        //Read the Written Value so as to advance "in pointer" 
        bufferRead (&buffer);
    }
    
    //Add one more value (that should be stored in the first position again)
    bufferWrite (&buffer, testValue);
    
    //Check that it has been stored in the first position
    assertEquals("BUFF_CIRC", testValue, buffer.buffer[0]);
}

/*
 Set of tests for the buffer
 */
void testBuffer() {
    test_can_write_and_read();
    test_buffer_is_empty();
    test_buffer_is_circular();
}
#endif
