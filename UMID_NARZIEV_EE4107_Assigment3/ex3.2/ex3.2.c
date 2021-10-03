#include <stdio.h>
#include <mpi/mpi.h>


void printMatrix(int matrix[], int size, int rank, char title[]);
void matrixGenerate(int matrix[], int my_rank, int size);
void matrixCommunication(int matrixOriginal[], int vector[], int size, int my_rank);
void computation(int matrixOriginal[], const int vector[], int size, int my_rank);

int main(){

    int my_rank, comm_sz, n, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    if(my_rank == 0){
        if(comm_sz != 4){
            printf("4 processes is need for communication, sorry");
            MPI_Finalize();
            return -1;

        }
        printf("Please eneter the size of the matrix(n): ");
        scanf("%d", &n);
        if(n%2 != 0){
            printf("Sorry eneter multiple of two");
            MPI_Finalize();
            return -1;

        }
            size = n/2;
    }
    MPI_Bcast(&size,1, MPI_INT, 0, MPI_COMM_WORLD);
    int A_local[size*size];
    int vector[size];

    matrixGenerate(A_local, my_rank, size);
    printMatrix(A_local, size, my_rank, "A matrix -> ");

    matrixCommunication(A_local, vector, size, my_rank); // Generating matrix B
    computation(A_local, vector, size, my_rank);
    printMatrix(A_local, size, my_rank, "B matrix -> ");

    matrixCommunication(A_local, vector, size, my_rank); // Generating matrix C
    computation(A_local, vector, size, my_rank);
    printMatrix(A_local, size, my_rank, "C matrix -> ");



    MPI_Finalize();
    return 1;
}

void matrixGenerate(int matrix[], int my_rank, int size){
    for (int i = 0; i < size*size; ++i) {
        matrix[i] = my_rank;
    }
}

void printMatrix(int matrix[], int size, int rank, char title[]){
    MPI_Barrier(MPI_COMM_WORLD);
    printf("my_rank -> %d, %s ", rank, title);
    for (int i = 0; i < size*size; ++i) {
        printf("%d\t", matrix[i]);
    }
    printf("\n");
}

void matrixCommunication(int matrixOriginal[], int vector[], int size, int my_rank){
    if(my_rank % 2 != 0) { //odd
        for (int i = 0; i < size*size; ++i) {
            MPI_Send(&matrixOriginal[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
        }

    }else{
        for (int i = 0; i < size*size; ++i) {
            MPI_Recv(&vector[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

    }

    if(my_rank % 2 == 0) { //even
        for (int i = 0; i < size*size; ++i) {
            MPI_Send(&matrixOriginal[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
        }

    }else{
        for (int i = 0; i < size*size; ++i) {
            MPI_Recv(&vector[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

    }

}
void computation(int matrixOriginal[], const int vector[], int size, int my_rank){
    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank % 2 != 0) { //odd
        for (int i = 0; i < size*size; ++i) {
            matrixOriginal[i] = vector[i] - matrixOriginal[i];
        }

    }else   if(my_rank % 2 == 0) { //even
        for (int i = 0; i < size * size; ++i) {
            matrixOriginal[i] += vector[i];
        }
    }
}
