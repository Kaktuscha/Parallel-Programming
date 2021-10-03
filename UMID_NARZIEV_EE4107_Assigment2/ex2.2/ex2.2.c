#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi/mpi.h"

void myMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
int generateRandom(int upper, int lower);

int main(int argc, char** argv) {
    int my_rank, comm_sz;
    int numbers[3];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if(my_rank == 0) {
        printf("The generated numbers are: ");
        for (int i = 0; i < 3; ++i) {
            srand(rand() + time(0)); // :-)))
            numbers[i] = generateRandom(100, -100);
            printf("%d\t", numbers[i]);
        }
        printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD) ;// used to give clarity at the output as processes were fighting for i/o buffer
    printf("Rank -> %d: Before Bcast, arrays is %d, %d, %d \n", my_rank, numbers[0], numbers[1], numbers[2]);
    MPI_Barrier(MPI_COMM_WORLD);
    myMPI_Bcast(&numbers, 3, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Rank -> %d, After Bcast, arrays is %d, %d, %d \n", my_rank, numbers[0], numbers[1], numbers[2]);

    MPI_Finalize();
    return 0;
}

void myMPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm){
    int my_rank, comm_sz;
    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &comm_sz);

    if(my_rank == 0) {
        for (int i = 1; i < comm_sz; ++i) {
            MPI_Send(buffer, count, datatype, i, 0, comm);
        }
    }else{
        MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUSES_IGNORE);
    }

}

int generateRandom(int upper, int lower){

    return (rand() % (upper - lower + 1)) + lower;
}
