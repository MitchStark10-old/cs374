/* Compute/draw mandelbrot set using MPI/MPE commands
 *
 * Written Winter, 1998, W. David Laverell.
 *
 * Refactored Winter 2002, Joel Adams. 
 * Parallel additions made by:
 * Mitch Stark, Fall 2017
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
               numProcesses = -1,
               chunkSize = -1;
    double     spacing  = 0.005,
               x        = 0.0,
               y        = 0.0,
               c_real   = 0.0,
               c_imag   = 0.0,
               x_center = 1.0,
               y_center = 0.0,
               start_time = -1,
               end_time = -1;

   bool **graph_data = malloc(WINDOW_SIZE * sizeof(bool *));
   bool **global_graph_data = malloc(WINDOW_SIZE * sizeof(bool *));

   for (int i = 0; i < WINDOW_SIZE; i++) {
        graph_data[i] = malloc(WINDOW_SIZE * sizeof(bool));
        global_graph_data[i] = malloc(WINDOW_SIZE * sizeof(bool));
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
    //start time
    start_time = MPI_Wtime();

    chunkSize = WINDOW_SIZE / numProcesses;

    if (id == numProcesses - 1) {
        chunkSize += WINDOW_SIZE % numProcesses;
    }

    for (ix = (WINDOW_SIZE / numProcesses) * id; ix < ((WINDOW_SIZE / numProcesses) * id) + chunkSize; ix++)
 //   for(ix = 0; ix < (WINDOW_SIZE / (id + 1)); ix++)
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
          graph_data[ix][iy] = (n < 50);

          

 /*         if (n < 50) {
             MPE_Draw_point(graph, ix, iy, MPE_RED);
          } else {
             MPE_Draw_point(graph, ix, iy, MPE_BLACK);
          }*/
       }
    }
    
    for(int i = 0; i < WINDOW_SIZE; i++) {
        //reduce all local arrays
        MPI_Reduce(graph_data[i], global_graph_data[i], WINDOW_SIZE, MPI_C_BOOL, MPI_LOR, 0, MPI_COMM_WORLD);
    }
    
    if (id == 0) {
        for(ix = 0; ix < WINDOW_SIZE; ix++) {
            for (iy = 0; iy < WINDOW_SIZE; iy++) {
                if (global_graph_data[ix][iy]) {
                    MPE_Draw_point(graph, ix, iy, MPE_RED);
                } else {
                    MPE_Draw_point(graph, ix, iy, MPE_BLACK);
                }
            }
        }
        //end time
        end_time = MPI_Wtime();
        printf("Time: %f seconds\n", end_time - start_time);
    }

    // pause until mouse-click so the program doesn't terminate
    if (id == 0) {
        printf("\nClick in the window to continue...\n");
        MPE_Get_mouse_press( graph, &ix, &iy, &button );
    }

    //need to free the graph_data array

    MPE_Close_graphics( &graph );
    MPI_Finalize();
    return 0;
}

