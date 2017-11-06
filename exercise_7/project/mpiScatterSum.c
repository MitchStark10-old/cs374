/* arraySum.c uses an array to sum the values in an input file,
 *  whose name is specified on the command-line.
 * Joel Adams, Fall 2005
 * Mitch Stark, Fall 2017
 * Updates made to parallelize the array sum with MPI
 * for CS 374 (HPC) at Calvin College.
 */

#include <stdio.h>      /* I/O stuff */
#include <stdlib.h>     /* calloc, etc. */
#include "mpi.h"

void readArray(char * fileName, double ** a, int * n);
double sumArray(double * a, int numValues) ;

int main(int argc, char * argv[])
{
    int  howMany;
    int chunk_size;
    int id;
    int num_processes;
    double * sub_a;
    double partial_sum;
    double total_sum;
    double * a;

    if (argc != 2) {
        fprintf(stderr, "\n*** Usage: arraySum <inputFile>\n\n");
        exit(1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (id == 0) {
        readArray(argv[1], &a, &howMany);
    }
    MPI_Bcast(&howMany, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    chunk_size = howMany / num_processes;
    if (id == (num_processes - 1)) {
        chunk_size += howMany % num_processes;
    }

    //send array to all the processes
    sub_a = malloc(sizeof(double) * chunk_size);
    MPI_Scatter(a, chunk_size, MPI_DOUBLE, sub_a, chunk_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    

    partial_sum = sumArray(sub_a, chunk_size);

    MPI_Reduce(&partial_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    if (id == 0) {
        printf("The sum of the values in the input file '%s' is %g\n",
                argv[1], total_sum);
    }

    free(sub_a);

    return 0;
}

/* readArray fills an array with values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char * fileName, double ** a, int * n) {
    int count, howMany;
    double * tempA;
    FILE * fin;

    fin = fopen(fileName, "r");
    if (fin == NULL) {
        fprintf(stderr, "\n*** Unable to open input file '%s'\n\n",
                fileName);
        exit(1);
    }

    fscanf(fin, "%d", &howMany);
    tempA = calloc(howMany, sizeof(double));
    if (tempA == NULL) {
        fprintf(stderr, "\n*** Unable to allocate %d-length array",
                howMany);
        exit(1);
    }

    for (count = 0; count < howMany; count++)
        fscanf(fin, "%lf", &tempA[count]);

    fclose(fin);

    *n = howMany;
    *a = tempA;
}

/* sumArray sums the values in an array of doubles.
 * Receive: a, a pointer to the head of an array;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

double sumArray(double * a, int numValues) {
    int i;
    double result = 0.0;

    for (i = 0; i < numValues; i++) {
        result += *a;
        a++;
    }

    return result;
}

