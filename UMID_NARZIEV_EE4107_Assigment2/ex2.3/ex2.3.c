#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi/mpi.h"

void myMPI_Reduce(const void *sendbuf, void *recvbuf, int count, int root, MPI_Comm comm);
double pseudorand(double max);

int main() {

    int my_rank, comm_sz;
    double rec = 0;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    srand(my_rank+ time(0));
    double a = pseudorand(15.8);
    printf("The process %d generated %f\n", my_rank, a);
    myMPI_Reduce(&a, &rec, 1, 0, MPI_COMM_WORLD);
    if(my_rank == 0){
        printf("Min number is %f\n", rec);
    }


    MPI_Finalize();
    return 0;
}

void myMPI_Reduce(const void *sendbuf, void *recvbuf, int count, int root, MPI_Comm comm){

    int rank, comm_sz;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_sz);


    if(rank != 0){
        MPI_Send(sendbuf, count, MPI_DOUBLE, 0, 0, comm);
    }else{
        double numbers[comm_sz];
        numbers[0] = *(double *)sendbuf;
        for (int i = 1; i < comm_sz; ++i) {
            MPI_Recv(&numbers[i], count, MPI_DOUBLE, i, 0, comm, MPI_STATUSES_IGNORE);

        }
        double min = numbers[1];
        for (int i = 0; i < comm_sz; ++i) {
            if(min >= numbers[i]){
                min = numbers[i];
            }
        }
        *(double *) recvbuf = min;
    }

}

double pseudorand(double max){
    return (max / RAND_MAX) * rand();
}
