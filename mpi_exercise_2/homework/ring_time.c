/* messagePassing.c
 * ... illustrates the use of the MPI_Send() and MPI_Recv() commands...
 * Joel Adams, Calvin College, November 2009.
 */

#include <stdio.h>
#include <mpi.h>
#include <math.h>   // sqrt()
#include <stdlib.h>
#include <string.h>

int odd(int number) { return number % 2; }

int main(int argc, char** argv) {
    int id = -1, numProcesses = -1; 
    double startTime = -1, endTime = -1;
    int sendAddress = -1;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    const int SIZE = numProcesses * 4;
    char * sendString = NULL;
    char * receivedString = NULL;


    if (numProcesses < 2) {
        printf("Please specify 2 or more processes\n");
        MPI_Finalize();
        return 0;
    }
    startTime = MPI_Wtime();

    sendString = (char*) malloc( SIZE );
    receivedString = (char*) malloc( SIZE );

    if (id == 0) {
        //Send a message containg rank to process 1
        sprintf(sendString, "0 ");
        MPI_Send(sendString, strlen(sendString) + 1, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
        //Recieve a message from n-1 process
        MPI_Recv(receivedString, SIZE, MPI_CHAR, numProcesses - 1, 1, MPI_COMM_WORLD, &status);
        //Print the message and time
        endTime = MPI_Wtime();
        printf("%s\n", receivedString);
        printf("Time: %f\n", endTime - startTime);
    } else {
        //Recieve message from rank i - 1
        MPI_Recv(receivedString, SIZE, MPI_CHAR, id - 1, 1, MPI_COMM_WORLD, &status);
        //Create a new message of received message + rank
        sprintf(sendString, "%s%d ", receivedString, id);
        sendAddress = (id + 1) % numProcesses;
        //Send message along to next process
        MPI_Send(sendString, strlen(sendString) + 1, MPI_CHAR, sendAddress, 1, MPI_COMM_WORLD);
    }


    MPI_Finalize();
    return 0;
}

