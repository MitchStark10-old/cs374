#include <stdio.h>                 // printf(), fprintf(), etc.
#include <stdlib.h>                // strtoul(), exit(), ...
#include <pthread.h>               // pthreads
#include "pthreadBarrier.h"

extern volatile long double * localSums;

/*
Takes array of local sums to sum into a value to be added to the pi variable
*/
long double reduce(long double numThreads) {
    long double sum = 0;
    for(unsigned long i = 0; i < numThreads; i++) {
        sum += localSums[i];
    }
    return sum;
}
