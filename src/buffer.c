#include <string.h>
#include <stdlib.h>

#include "./buffer.h"

/* Set high default buffer capacity to reduce
   malloc calls.  */
#define DEFAULT_CAPACITY 64;

Buffer *init_buffer() {
    Buffer *buffer = (Buffer*)malloc(sizeof(Buffer));
    buffer->capacity = DEFAULT_CAPACITY;
    buffer->mem = (char*)malloc(buffer->capacity * sizeof(char));
    buffer->length = 0;
    return buffer;
}

void destroy_buffer(Buffer *buffer) {
    free(buffer->mem);
    free(buffer);
}

static void expand_buffer(Buffer *buffer) {
    /* Increase the buffer capacity by multiplication of 2 each time it is expaned.  */
    int newCapacity = buffer->capacity << 1;
    char* result = (char*)realloc(buffer->mem , newCapacity * sizeof(char));
    /* If C is unable to allocate anymore memory space,
       then free the buffer memory and exit the program.  */
    if (result == NULL) {
        free(buffer->mem);
        free(buffer);
        exit(1);
    }

    buffer->mem = result;
    buffer->capacity = newCapacity;
}

void write_to_buffer(Buffer *buffer, const char* mem, int length) {
    /* If the buffer is already at max capacity,
       then expand the buffer memory space.  */
    if (buffer->length + length > buffer->capacity) {
        expand_buffer(buffer);
    }

    for (int i = 0; i < length; i++) {
        buffer->mem[buffer->length] = mem[i];
        buffer->length++;
    }
}
