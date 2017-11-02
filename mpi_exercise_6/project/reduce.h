/*
* Mitch Stark, Fall 2017, Calvin College
* Purpose of this reduce function is to take in an array
* that is accessed by all threads to reduce the local values
* of each thread into a single value (in this case a sum).
* A pthreadBarrier() call is made before calling this to ensure
* that all values have entered the array before summing
*/
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
