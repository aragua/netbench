
#include "timestamp.h"

timestamp_t timestamp()
{
    struct timeval ts;
    timestamp_t ret;
    gettimeofday( &ts, NULL );
    ret.sec = (u_int32_t)ts.tv_sec;
    ret.usec = (u_int32_t)ts.tv_usec;
    return ret;
}


int32_t ts_diff( timestamp_t src, timestamp_t dst )
{
    return (int32_t)((dst.sec - src.sec) * 1000000) + (dst.usec - src.usec);
}
