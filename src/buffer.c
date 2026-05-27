#include <string.h>
#include <stdlib.h>

#include "./buffer.h"

/* Set high default buffer capacity to reduce
   malloc calls.  */
#define DEFAULT_CAPACITY 64;

buf_s *init_buffer() {
    buf_s *buffer = (buf_s *)malloc(sizeof(buf_s));
    buffer->capacity = DEFAULT_CAPACITY;
    buffer->mem = (char*)malloc(buffer->capacity * sizeof(char));
    buffer->length = 0;
    return buffer;
}

void destroy_buffer(buf_s *buffer) {
    free(buffer->mem);
    free(buffer);
}

static void expand_buffer(buf_s *buffer) {
    /* Increase the buffer capacity by multiplication of 2 each time it is expaned.  */
    int new_capacity = buffer->capacity << 1;
    char* result = (char*)realloc(buffer->mem , new_capacity * sizeof(char));
    /* If C is unable to allocate anymore memory space,
       then free the buffer memory and exit the program.  */
    if (result == NULL) {
        free(buffer->mem);
        free(buffer);
        exit(1);
    }

    buffer->mem = result;
    buffer->capacity = new_capacity;
}

void write_to_buffer(buf_s *buffer, const char* mem, int length) {
    /* If the buffer is already at max capacity,
       then expand the buffer memory space.  */
    while (buffer->length + length > buffer->capacity) {
        expand_buffer(buffer);
    }

    for (int i = 0; i < length; i++) {
        buffer->mem[buffer->length] = mem[i];
        buffer->length++;
    }
}
