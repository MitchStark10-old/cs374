/* integral.c defines integrateTrap(), a function to approximate
*  definite integrals using the trapezoidal rule. 
*
* Joel Adams, Fall 2013.
*/

#include "integral.h"
#include <mpi.h>
#include <stdio.h>

/* declaration of the function to be integrated,
* which must be defined in the caller of integrate()
* or a linking error will occur
*/
extern double f(double x); 

/* function to approximate an integral:
* parameters: xLo, the left endpoint
*             xHi, the right endpoint
*             chunkSize, the number of trapezoid to use
*             startPoint, the starting point for the loop: (numProcesses / (id + 1)) - 1
* return: the approximate integral of f x) from x1 to x2.
*/
long double integrateTrap(double xLo, double xHi, unsigned long long numTrapezoids,
					unsigned long long chunkSize, unsigned long long startPoint, int id) {
	long double delta = (xHi - xLo) / numTrapezoids; //problem here, use numProcesses instead
	long double result = 0, global_result = 0;
	unsigned long long i = 0;
	
	for (i = startPoint; i < startPoint + chunkSize; i++) {
		result += f(xLo + i*delta);
	}
	//synchronize result here
	MPI_Reduce(&result, &global_result, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	global_result += (f(xLo) + f(xHi)) * 0.5;
	global_result *= delta;
	
	return global_result;
} 

/* Note: The integral is approximately
*   delta * (f(xLo) + f(x1)) / 2
* + delta * (f(x1) + f(x2)) / 2
* + delta * (f(x2) + f(x3)) / 2
* ...
* + delta * (f(x[n-2] + f(x[n-1]) / 2
* + delta * (f(x[n-1]) + f(x[n])) / 2
*/

