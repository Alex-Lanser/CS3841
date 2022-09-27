#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define NS_PER_SEC 1000000000

//Get time in nanoseconds
static inline uint64_t gettime_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * NS_PER_SEC + ts.tv_nsec;
}

int main(int argc, char* argv[]){
    return 0;
}