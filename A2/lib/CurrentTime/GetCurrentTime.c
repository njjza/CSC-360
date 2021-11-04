#include "GetCurrentTime.h"

double GetCurrentTime()
{
    struct timeval time;
    
    gettimeofday(&time, (void *) 0);

    return time.tv_sec + (double) time.tv_usec / 1000000;
}