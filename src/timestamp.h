#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdlib.h>

typedef struct{
    u_int32_t sec;
    u_int32_t usec;
} __attribute__((packed)) timestamp_t;

timestamp_t timestamp();

int32_t ts_diff( timestamp_t src, timestamp_t dst );

#endif // TIMESTAMP_H
