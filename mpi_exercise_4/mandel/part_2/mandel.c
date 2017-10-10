/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
#include <mpe.h>
#include "display.h"


/* compute the Mandelbrot-set function for a given
 *  point in the complex plane.
 *
 * Receive: doubles x and y,
 *          complex c.
 * Modify: doubles ans_x and ans_y.
 * POST: ans_x and ans_y contain the results of the mandelbrot-set
 *        function for x, y, and c.
 */
void compute(double x, double y, double c_real, double c_imag,
              double *ans_x, double *ans_y)
{
        *ans_x = x*x - y*y + c_real;
        *ans_y = 2*x*y + c_imag;
}

/* compute the 'distance' between x and y.
 *
 * Receive: doubles x and y.
 * Return: x^2 + y^2.
 */
double distance(double x, double y)
{
        return x*x + y*y;
}


int main(int argc, char* argv[])
{
    const int  WINDOW_SIZE = 1024;

    int        n        = 0,
               ix       = 0,
               iy       = 0,
               button   = 0,
               id       = 0,
               numProcesses,
               next_row_to_compute,
               row_to_compute;
    double     spacing  = 0.005,
               x        = 0.0,
               y        = 0.0,
               c_real   = 0.0,
               c_imag   = 0.0,
               x_center = 1.0,
               y_center = 0.0;
    MPI_Status status;
    bool **graph_data = malloc(WINDOW_SIZE * sizeof(bool *));
    bool *row_data = malloc(WINDOW_SIZE * sizeof(bool));

    for(int i = 0; i < WINDOW_SIZE; i++) {
        graph_data[i] = malloc(WINDOW_SIZE * sizeof(bool));
    }

    MPE_XGraph graph;

    MPI_Init(&argc,&argv);
/*
    // Uncomment this block for interactive use
    printf("\nEnter spacing (.005): "); fflush(stdout);
    scanf("%lf",&spacing);
    printf("\nEnter coordinates of center point (0,0): "); fflush(stdout);
    scanf("%lf %lf", &x_center, &y_center);
    printf("\nSpacing=%lf, center=(%lf,%lf)\n",
            spacing, x_center, y_center);
*/
    MPE_Open_graphics( &graph, MPI_COMM_WORLD, 
                         getDisplay(),
                         -1, -1,
                         WINDOW_SIZE, WINDOW_SIZE, 0 );

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    //Remove for-loop dealing with x coordinates, this will be handled for master


    if (numProcesses == 1) {
        for (ix = 0; ix < WINDOW_SIZE; ix++)
        {
            for (iy = 0; iy < WINDOW_SIZE; iy++)
            {
                c_real = (ix - 400) * spacing - x_center;
                c_imag = (iy - 400) * spacing - y_center;
                x = y = 0.0;
                n = 0;

                while (n < 50 && distance(x, y) < 4.0)
                {
                    compute(x, y, c_real, c_imag, &x, &y);
                    n++;
                }

                if (n < 50) {
                    MPE_Draw_point(graph, ix, iy, MPE_RED);
                } else {
                    MPE_Draw_point(graph, ix, iy, MPE_BLACK);
                }
            }
        }
    } else {
        /*
        Process 0 -
            Wait to receive a row
            After receiving, collect data into single uniform array
            Send worker new row to compute
            Continue until array has been filled
       */
        if (id == 0) {
            for(int i = 0; i < WINDOW_SIZE; i++) {
                MPI_Recv(row_data, WINDOW_SIZE, MPI_C_BOOL, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);
                graph_data[i] = row_data;
                for (int itest = 0; itest < WINDOW_SIZE; itest++) {
                   //this works, but the below print statement does not...
                   if (graph_data[i][itest]) {
                        MPE_Draw_point(graph, i, itest, MPE_RED);
                   } else {
                        MPE_Draw_point(graph, i, itest, MPE_BLACK);
                   }
                }
                next_row_to_compute = i + (numProcesses - 1);
                MPI_Send(&next_row_to_compute, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            }
/*
NOTE: This print statement for whatever reason did not work, the graph_data was simply all "1"
            for(ix = 0; ix < WINDOW_SIZE; ix++) {
                for(iy = 0; iy < WINDOW_SIZE; iy++) {
                    if (graph_data[ix][iy]) {
                        MPE_Draw_point(graph, ix, iy, MPE_RED);
                    } else {
                        MPE_Draw_point(graph, ix, iy, MPE_BLACK);
                    }
                }
            }
 */
        } else {
       /*
        All other Processes -
            Computer row ID - 1
            Send row to Master
            Recieve new row to compute
            if tag != 0 -> exit
        */
            row_to_compute = id - 1;
            while(true) {
                for(iy = 0; iy < WINDOW_SIZE; iy++) {
                    c_real = (row_to_compute - 400) * spacing - x_center;
                    c_imag = (iy - 400) * spacing - y_center;
                    x = y = 0.0;
                    n = 0;

                    while (n < 50 && distance(x, y) < 4.0) {
                        compute(x, y, c_real, c_imag, &x, &y);
                        n++;
                    }
                    row_data[iy] = (n < 50);
                }

                //send row
                MPI_Send(row_data, WINDOW_SIZE, MPI_C_BOOL, 0, row_to_compute, MPI_COMM_WORLD);

                //recieve new row
                MPI_Recv(&row_to_compute, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

                if (row_to_compute > (WINDOW_SIZE - 1)) break;
            }
        }

    }

    // pause until mouse-click so the program doesn't terminate
    if (id == 0) {
        printf("\nClick in the window to continue...\n");
        MPE_Get_mouse_press( graph, &ix, &iy, &button );
    }

    MPE_Close_graphics( &graph );
    MPI_Finalize();
    return 0;
}

