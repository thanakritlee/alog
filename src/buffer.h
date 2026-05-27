#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    char* mem;          /* Buffer memory address.  */
    int capacity;       /* Capacity of the buffer.
                           Use for determining when to expand
                           the buffer.  */
    int length;         /* Occupied space in the buffer.  */
} buf_s;

/* Initialise the buffer.  */
buf_s *init_buffer();
/* Free the buffer memory and set capacity and length to 0.  */
void destroy_buffer(buf_s *buf);
/* Write a string to buffer.  */
void write_to_buffer(buf_s *buf, const char* mem, int length);

#endif
