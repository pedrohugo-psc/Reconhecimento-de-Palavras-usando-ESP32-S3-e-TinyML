#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdio.h>

#define BUFFER_SIZE 16000

typedef struct
{
    int16_t buffer[BUFFER_SIZE];
    int start;
    int end;
    int length;

}ring_buffer_t;

void rb_init(ring_buffer_t *cb);
void rb_insert(ring_buffer_t *cb, int16_t data);
void rb_remove(ring_buffer_t *cb);
int rb_get_size(ring_buffer_t *cb);

#endif // RING_BUFFER_H
