/* firestarter.c 
 * David Joiner
 * Usage: Fire [forestSize(20)] [numTrials(5000)] * [numProbabilities(101)] [showGraph(1)]
 * Usage (with MPI) : mpirun Fire -np 1 -machinefile ../../host_information/hosts *ABOVE_USAGE*
 */
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include "X-graph.h"

#define UNBURNT 0
#define SMOLDERING 1
#define BURNING 2
#define BURNT 3

#define true 1
#define false 0

typedef int boolean;

extern void seed_by_time(int);
extern int ** allocate_forest(int);
extern void initialize_forest(int, int **);
extern double get_percent_burned(int, int **);
extern void delete_forest(int, int **);
extern void light_tree(int, int **,int,int);
extern boolean forest_is_burning(int, int **);
extern void forest_burns(int, int **,double);
extern int burn_until_out(int,int **,double,int,int);
extern void print_forest(int, int **);

int main(int argc, char ** argv) {
    // initial conditions and variable definitions
    int forest_size=20;
    double * prob_spread;
    double prob_min=0.0;
    double prob_max=1.0;
    double prob_step;
    int **forest;
    double * percent_burned;
    double * average_iterations;
    int i_trial;
    int i;
    int n_trials=5000;
    int i_prob;
    int n_probs=101;
    int do_display=1;
    xgraph thegraph;

    //initialize mpi arguments
    int id = -1, numProcesses = -1;

    // check command line arguments
    if (argc > 1) {
        sscanf(argv[1],"%d",&forest_size);
    }
    if (argc > 2) {
        sscanf(argv[2],"%d",&n_trials);
    }
    if (argc > 3) {
        sscanf(argv[3],"%d",&n_probs);
    }
    if (argc > 4) {
        sscanf(argv[4],"%d",&do_display);
    }
    if (do_display!=0) do_display=1;

    // setup problem
    seed_by_time(0);
    forest=allocate_forest(forest_size);
    prob_spread = (double *) malloc (n_probs*sizeof(double));
    percent_burned = (double *) malloc (n_probs*sizeof(double));
    average_iterations = (double *) malloc (n_probs*sizeof(double));

    //initialize the arrays
    for(i=0; i < n_probs; i++) {
        prob_spread[i] = 0;
        percent_burned[i] = 0;
        average_iterations[i] = 0;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    printf("Specified %d processes!", numProcesses);

    // for a number of probabilities, calculate
    // average burn and output
    prob_step = (prob_max-prob_min)/(double)(n_probs-1);
    printf("Probability of fire spreading, Average percent burned\n");
    
    //parallelize the trials, put the trials on the outer loop
    // for (i_prob = 0 ; i_prob < n_probs; i_prob++) {
    //     //for a number of trials, calculate average
    //     //percent burn
    //     prob_spread[i_prob] = prob_min + (double)i_prob * prob_step;
    //     percent_burned[i_prob]=0.0;
    //     average_iterations=0;
    //     for (i_trial=0; i_trial < n_trials; i_trial++) {
    //         //burn until fire is gone
    //         average_iterations += burn_until_out(forest_size,forest,prob_spread[i_prob],
    //             forest_size/2,forest_size/2);
    //         percent_burned[i_prob]+=get_percent_burned(forest_size,forest);
    //     }
    //     average_iterations/=n_trials;
    //     percent_burned[i_prob]/=n_trials;

    //     // print output
    //     printf("Probability = %lf , %% Burned = %lf, Iterations = %d\n",prob_spread[i_prob],
    //         percent_burned[i_prob], average_iterations);
    // }

    for(i_trial=0; i_trial < n_trials; i_trial++) {
        for(i_prob=0; i_prob < n_probs; i_prob++) {
            prob_spread[i_prob] = prob_min + (double)i_prob * prob_step;
            
            average_iterations[i_prob]+=burn_until_out(forest_size,forest,prob_spread[i_prob],
                forest_size/2,forest_size/2);

            percent_burned[i_prob]+=get_percent_burned(forest_size,forest);
        }
    }

    //Ensure that all processes have completed before iterating one last time to print
    MPI_Barrier(MPI_COMM_WORLD);
    for(i=0; i < n_probs; i++) {
        average_iterations[i]/=n_trials;
        percent_burned[i]/=n_trials;

        printf("Probability = %lf , %% Burned = %lf, Iterations = %lf\n",prob_spread[i],
            percent_burned[i], average_iterations[i]);
    }
    MPI_Finalize();

    // plot graph
    if (do_display==1) {
        xgraphSetup(&thegraph,300,300);
        xgraphDraw(&thegraph,n_probs,0,0,1,1,prob_spread,percent_burned);
        pause();
    }

    // clean up
    delete_forest(forest_size,forest);
    free(prob_spread);
    free(percent_burned);
    free(average_iterations);
    return 0;
}

#include <time.h>

void seed_by_time(int offset) {
    time_t the_time;
    time(&the_time);
    srand((int)the_time+offset);
}

int burn_until_out(int forest_size,int ** forest, double prob_spread,
    int start_i, int start_j) {
    int count;

    initialize_forest(forest_size,forest);
    light_tree(forest_size,forest,start_i,start_j);

    // burn until fire is gone
    count = 0;
    while(forest_is_burning(forest_size,forest)) {
        forest_burns(forest_size,forest,prob_spread);
        count++;
    }

    return count;
}

double get_percent_burned(int forest_size,int ** forest) {
    int i,j;
    int total = forest_size*forest_size-1;
    int sum=0;

    // calculate pecrent burned
    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                sum++;
            }
        }
    }

    // return percent burned;
    return ((double)(sum-1)/(double)total);
}


int ** allocate_forest(int forest_size) {
    int i;
    int ** forest;

    forest = (int **) malloc (sizeof(int*)*forest_size);
    for (i=0;i<forest_size;i++) {
        forest[i] = (int *) malloc (sizeof(int)*forest_size);
    }

    return forest;
}

void initialize_forest(int forest_size, int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            forest[i][j]=UNBURNT;
        }
    }
}

void delete_forest(int forest_size, int ** forest) {
    int i;

    for (i=0;i<forest_size;i++) {
        free(forest[i]);
    }
    free(forest);
}

void light_tree(int forest_size, int ** forest, int i, int j) {
    forest[i][j]=SMOLDERING;
}

boolean fire_spreads(double prob_spread) {
    if ((double)rand()/(double)RAND_MAX < prob_spread) 
        return true;
    else
        return false;
}

void forest_burns(int forest_size, int **forest,double prob_spread) {
    int i,j;
    extern boolean fire_spreads(double);

    //burning trees burn down, smoldering trees ignite
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) forest[i][j]=BURNT;
            if (forest[i][j]==SMOLDERING) forest[i][j]=BURNING;
        }
    }

    //unburnt trees catch fire
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) {
                if (i!=0) { // North
                    if (fire_spreads(prob_spread)&&forest[i-1][j]==UNBURNT) {
                        forest[i-1][j]=SMOLDERING;
                    }
                }
                if (i!=forest_size-1) { //South
                    if (fire_spreads(prob_spread)&&forest[i+1][j]==UNBURNT) {
                        forest[i+1][j]=SMOLDERING;
                    }
                }
                if (j!=0) { // West
                    if (fire_spreads(prob_spread)&&forest[i][j-1]==UNBURNT) {
                        forest[i][j-1]=SMOLDERING;
                    }
                }
                if (j!=forest_size-1) { // East
                    if (fire_spreads(prob_spread)&&forest[i][j+1]==UNBURNT) {
                        forest[i][j+1]=SMOLDERING;
                    }
                }
            }
        }
    }
}

boolean forest_is_burning(int forest_size, int ** forest) {
    int i,j;

    for (i=0; i<forest_size; i++) {
        for (j=0; j<forest_size; j++) {
            if (forest[i][j]==SMOLDERING||forest[i][j]==BURNING) {
                return true;
            }
        }
    }
    return false;
}

void print_forest(int forest_size,int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                printf(".");
            } else {
                printf("X");
            }
        }
        printf("\n");
    }
}
