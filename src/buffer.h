#ifndef buffer_h
#define buffer_h

typedef struct {
    char* mem;          /* Buffer memory address.  */
    int capacity;       /* Capacity of the buffer.
                           Use for determining when to expand
                           the buffer.  */
    int length;         /* Occupied space in the buffer.  */
} Buffer;

/* Initialise the buffer.  */
Buffer *init_buffer();
/* Free the buffer memory and set capacity and length to 0.  */
void destroy_buffer(Buffer *buffer);
/* Write a string to buffer.  */
void write_to_buffer(Buffer *buffer, const char* mem, int length);

#endif
