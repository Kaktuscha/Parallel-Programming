#include <stdio.h>
#include <stdlib.h>
#include "mpi/mpi.h"

#define MR_MULTIPLIER 279470273
#define MR_INCREMENT 0
#define MR_MODULUS 4294967291U
#define MR_DIVISOR ((double) 4294967291U)



double uniform(double a, double b);

//-----------------------------------USE STRUCT-------------------------------------------------------------------------
// pi =  3.14 15 92 65 35 89 79 32 38 46 26 43 3
int main() {

    int my_rank, comm_sz;
    double pi_estimate = 0;
    long long int number_of_tosses = 100000000;
    double local_x = 0, local_y = 0;
    long long int local_number_in_circle = 0;
    double local_distance_squared = 0;
    long long int global_number_in_circle = 0;
    double local_start = 0, local_end = 0, local_elapsed = 0, elapsed;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();
    for (long long int toss = 0; toss < number_of_tosses / comm_sz; toss++) {
        local_x = uniform(-1, 1); // use uniform function
        local_y = uniform(-1, 1);
        local_distance_squared = local_x * local_x + local_y * local_y;
        if (local_distance_squared <= 1) {
            local_number_in_circle++;
        }

    }
    /// printf("local_number_in_circle => %lld\n", local_number_in_circle);

    MPI_Reduce(&my_rank, &global_number_in_circle, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_number_in_circle, &global_number_in_circle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    local_end = MPI_Wtime();
    local_elapsed = local_end - local_start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        pi_estimate = 4 * (double) global_number_in_circle / ((double) number_of_tosses);
        printf("The estimated pi -> %f, elapsed time -> %f, number of processors -> %d, number of tosses -> %lld\n",
               pi_estimate, elapsed, comm_sz, number_of_tosses);
    }

    MPI_Finalize();
    return 0;
}


double uniform(double a, double b) {
    return rand() / (RAND_MAX + 1.0) * (b - a) + a;
}

