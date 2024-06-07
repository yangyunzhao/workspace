#pragma once
#include <time.h>

inline double gettime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000 * 1000 + now.tv_nsec * 1.0e-3;
}