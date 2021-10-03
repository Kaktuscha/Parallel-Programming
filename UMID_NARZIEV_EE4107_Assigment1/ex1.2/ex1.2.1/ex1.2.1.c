/*
 * Tree-Structured 2^n case
 * To Do tasks:
 * 1 -> Check 2^n processes
 *
 * */
#include "mpi/mpi.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int count = 0;
    int my_rank, comm_sz;
    int sum = 0;
    int grouping = 2;
//    int n = 8; // Number of processes
    int *a = NULL;
    int dest_var = 0;
    int step = 1;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if(comm_sz%2 != 0){
        printf("Sorry I did not implement this program for any number of processes\n");
        MPI_Finalize();
        return -1;
    }
    if(my_rank == 0){
        a = malloc(comm_sz * sizeof (int)); // n -> comm_sz
        printf("Please enter %d numbers of elements: \n", comm_sz);
        for (int i = 0; i < comm_sz; i++){
            scanf("%d", &a[i]);
        }

        MPI_Scatter(a, 1, MPI_INT, &sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
        free(a);
    } else {
            MPI_Scatter(a, 1, MPI_INT, &sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }



    while(grouping <= comm_sz){
       // MPI_Barrier(MPI_COMM_WORLD);
        if(my_rank % grouping == step){
            count++;
         //   printf("my_rank - step => %d - %d = %d: \n", my_rank, step, my_rank - step);
            MPI_Send(&sum, 1, MPI_INT, my_rank - step, 0, MPI_COMM_WORLD);
        }
        if(my_rank % grouping == 0){
            count++;
            MPI_Recv(&dest_var, 1, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         //   printf("my_rank + step => %d + %d = %d: \n", my_rank, step, my_rank + step);
            sum += dest_var;
        }

        step <<= 1;
        grouping <<= 1;

    }

    printf("my_rank = %d, sum = %d, count = %d\n", my_rank, sum, count);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}

