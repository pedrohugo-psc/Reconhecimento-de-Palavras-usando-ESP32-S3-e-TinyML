#ifndef INMP441_H
#define INMP441_H


typedef struct{
    void* buffer;
    int feed_chunksize;
    int feed_nch;
    int16_t *feed_buffer;
    size_t feed_buffer_offset;
    int feed_buffer_len;
    int buffer_len;
    size_t bytes_read;
} inmp441_datachunk_t;

void inmp441_setup();
void inmp441_enable();
void* inmp441_read_data();
inmp441_datachunk_t *inmp441_get_datachunk();


#endif // INMP441_H