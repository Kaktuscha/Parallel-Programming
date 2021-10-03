/*
 * Butterfly structure communication
 */
#include <stdio.h>
#include <mpi/mpi.h>
#include <stdlib.h>


/* Butterfly communication */
int main() {

    int my_rank, comm_sz;
    MPI_Init(NULL, NULL); // Look at this func parameters
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int group = 2, step = 1, sum = 0;
    int *a = NULL;
    int dest_var = 0;
    if(comm_sz%2 != 0){
        printf("Sorry I did not implement this program for any number of processes\n");
        MPI_Finalize();
        return 1;
    }
    if(my_rank == 0){
        a = malloc(comm_sz * sizeof (int));
        printf("Please enter n = %d of numbers: \n", comm_sz);
        for (int i = 0; i < comm_sz; ++i) {
            scanf("%d", &a[i]);
        }
        MPI_Scatter(a, 1, MPI_INT, &sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
        free(a);
    }else{
        MPI_Scatter(a, 1, MPI_INT, &sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    printf("Before my_rank -> %d, value -> %d\n", my_rank, sum);
    while (group <= comm_sz){
        if(my_rank % group >= step){
            MPI_Sendrecv(&sum, 1, MPI_INT, my_rank - step, 0, &dest_var, 1, MPI_INT, my_rank - step, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            sum += dest_var;
        }else{
            MPI_Sendrecv(&sum, 1, MPI_INT, my_rank + step, 0, &dest_var, 1, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            sum += dest_var;
        }

        group <<= 1;
        step <<= 1;
   }


    printf("After my_rank -> %d, value -> %d\n", my_rank, sum);
    MPI_Finalize();
    return 0;
}
