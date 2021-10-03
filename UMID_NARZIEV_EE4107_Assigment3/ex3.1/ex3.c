#include <stdio.h>
#include <mpi/mpi.h>

#define A  //Use followings to change the mode
/*           1. A -> task A
 *           2. B -> task B
 *           3. C -> task C
 */

void printMatrix(int matrix[], int size, int rank, char title[]);
void matrixGenerate(int matrix[], int my_rank, int size);
void matrixCommunication_A(int matrixOriginal[], int matrixGenerating[], int size, int my_rank);
void matrixCommunication_B(int matrixOriginal[], int matrixGenerating[], int size, int my_rank, MPI_Request *requests);
void matrixCommunication_C(int matrixOriginal[], int matrixGenerating[], int size, int my_rank, MPI_Request *requests);

int main(){

    int my_rank, comm_sz, n, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Request requests[comm_sz];

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
    int B_local[size*size];
    int C_local[size*size];

    matrixGenerate(A_local, my_rank, size);
    printMatrix(A_local, size, my_rank, "A matrix -> ");
#ifdef A
    matrixCommunication_A(A_local, B_local, size, my_rank); // Generating matrix B
#endif

#ifdef B
    matrixCommunication_B(A_local, B_local, size, my_rank, &requests[my_rank]); // Generating matrix B
#endif

#ifdef C
    matrixCommunication_C(A_local, B_local, size, my_rank, &requests[my_rank]); // Generating matrix B
#endif
///----------------------------------------------------------------------------------------------
    printMatrix(B_local, size, my_rank, "B matrix -> ");
///----------------------------------------------------------------------------------------------
#ifdef A
    matrixCommunication_A(B_local, C_local, size, my_rank); // Generating matrix C
#endif
#ifdef B
    matrixCommunication_B(B_local, C_local, size, my_rank, &requests[my_rank]); // Generating matrix C
#endif
#ifdef C
    matrixCommunication_B(B_local, C_local, size, my_rank, &requests[my_rank]); // Generating matrix C
#endif

    printMatrix(C_local, size, my_rank, "C matrix -> ");

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

void matrixCommunication_A(int matrixOriginal[], int matrixGenerating[], int size, int my_rank){
    if(my_rank % 2 != 0) { //odd
        for (int i = 0; i < size*size; ++i) {
            MPI_Send(&matrixOriginal[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
        }
        //   printf("Send -> my_rank -> %d, my_rank - 1 -> %d\n", my_rank, my_rank - 1);
    }else{
        for (int i = 0; i < size*size; ++i) {
            MPI_Recv(&matrixGenerating[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            matrixGenerating[i] += matrixOriginal[i];
        }
        //  printf("Recv -> my_rank -> %d, my_rank + 1 -> %d\n", my_rank, my_rank + 1);
    }

    if(my_rank % 2 == 0) { //even
        for (int i = 0; i < size*size; ++i) {
            MPI_Send(&matrixOriginal[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
        }
        //  printf("Send -> my_rank -> %d, my_rank + 1 -> %d\n", my_rank, my_rank + 1);
    }else{
        for (int i = 0; i < size*size; ++i) {
            MPI_Recv(&matrixGenerating[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            matrixGenerating[i] -= matrixOriginal[i];
        }
        //  printf("Recv -> my_rank -> %d, my_rank - 1 -> %d\n", my_rank, my_rank - 1);
    }
}

void matrixCommunication_B(int matrixOriginal[], int matrixGenerating[], int size, int my_rank, MPI_Request *requests){
    if(my_rank % 2 != 0) { //odd

        for (int i = 0; i < size*size; ++i) {

            MPI_Isend(&matrixOriginal[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, requests);
        }
        //   printf("Send -> my_rank -> %d, my_rank - 1 -> %d\n", my_rank, my_rank - 1);
    }else{
        for (int i = 0; i < size*size; ++i) {
            MPI_Recv(&matrixGenerating[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            matrixGenerating[i] += matrixOriginal[i];
        }
        //  printf("Recv -> my_rank -> %d, my_rank + 1 -> %d\n", my_rank, my_rank + 1);
    }

    if(my_rank % 2 == 0) { //even

        for (int i = 0; i < size*size; ++i) {

            MPI_Isend(&matrixOriginal[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD,  requests);
        }
        //  printf("Send -> my_rank -> %d, my_rank + 1 -> %d\n", my_rank, my_rank + 1);
    }else{
        for (int i = 0; i < size*size; ++i) {
            MPI_Recv(&matrixGenerating[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            matrixGenerating[i] -= matrixOriginal[i];
        }
        //  printf("Recv -> my_rank -> %d, my_rank - 1 -> %d\n", my_rank, my_rank - 1);
    }
}

void matrixCommunication_C(int matrixOriginal[], int matrixGenerating[], int size, int my_rank, MPI_Request *requests){
    if(my_rank % 2 != 0) { //odd
        for (int i = 0; i < size*size; ++i) {
           // MPI_Barrier(MPI_COMM_WORLD); -> not used
            MPI_Send(&matrixOriginal[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
        }
        //   printf("Send -> my_rank -> %d, my_rank - 1 -> %d\n", my_rank, my_rank - 1);
    }else{
        for (int i = 0; i < size*size; ++i) {
          //  MPI_Barrier(MPI_COMM_WORLD);
            MPI_Irecv(&matrixGenerating[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, requests);
            MPI_Barrier(MPI_COMM_WORLD);
            matrixGenerating[i] += matrixOriginal[i];
        }
        //  printf("Recv -> my_rank -> %d, my_rank + 1 -> %d\n", my_rank, my_rank + 1);
    }

    if(my_rank % 2 == 0) { //even
        for (int i = 0; i < size*size; ++i) {
            //MPI_Barrier(MPI_COMM_WORLD); -> not used
            MPI_Send(&matrixOriginal[i], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
        }
        //  printf("Send -> my_rank -> %d, my_rank + 1 -> %d\n", my_rank, my_rank + 1);
    }else{
        for (int i = 0; i < size*size; ++i) {
          //  MPI_Barrier(MPI_COMM_WORLD);
            MPI_Irecv(&matrixGenerating[i], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, requests);
            MPI_Barrier(MPI_COMM_WORLD);
            matrixGenerating[i] -= matrixOriginal[i];
        }
        //  printf("Recv -> my_rank -> %d, my_rank - 1 -> %d\n", my_rank, my_rank - 1);
    }
}

