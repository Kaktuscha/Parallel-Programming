#include <stdio.h>
#include <mpi/mpi.h>
#include <stdlib.h>

#define sizeOfArray 20
struct valueRecord{
    int value;
    int *index;
    int count;
};

void swap(int *xp, int *yp);
void bubbleSort(int arr[], int n);
void printArray(int arr[], int n, int my_rank);
void generateArray(int arr[], int n, int my_rank);
struct valueRecord linearSearch(int *arr, int n);

int main() {

    int my_rank, comm_sz, collection[sizeOfArray];

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    generateArray(collection, sizeOfArray, my_rank);
    bubbleSort(collection, sizeOfArray);

//    printArray(collection, sizeOfArray, my_rank); // ---------> uncomment it to print the result

    MPI_Barrier(MPI_COMM_WORLD);

    int *finalArray = NULL;
    if(my_rank == 0){
        finalArray = malloc(sizeof(int) * comm_sz);
    }
    MPI_Gather(&collection[0], 1, MPI_INT, finalArray, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank == 0){
        printf("The largest values collected from process stored in ");
        printArray(finalArray, comm_sz, my_rank);
        struct valueRecord record;
        record = linearSearch(finalArray, comm_sz);
        printf("The largest value is %d from process(es) ", record.value);
        for (int i = 0; i < record.count; ++i) {
            printf("%d\t", record.index[i]);
        }
        printf("\n");
    }


    MPI_Finalize();
    return 0;
}

void swap(int *xp, int *yp){
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n){
    for (int i = 0; i < n-1; ++i) {
        for (int j = 0; j < n-i-1; ++j) {
            if (arr[j] < arr[j+1])
            swap(&arr[j], &arr[j+1]);
        }
    }
}

void printArray(int arr[], int n, int my_rank){
    printf("my_rank -> %d\n", my_rank);
    for (int i = 0; i < n; ++i) {
        printf("%d\t", arr[i]);
    }
    printf("\n");
}

void generateArray(int arr[], int n, int my_rank){
    for (int i = 0; i < n; ++i) {
        srand(my_rank+i+i*i); //may be use may_rank + timer() to generate more uniqe numbers but I want repeated larges number so I used this combination
        arr[i] = (rand() % (100 - (-100) + 1)) + (-100);
    }
}

struct valueRecord linearSearch(int *arr, int n) {
    int theLargest = arr[0];
    for (int i = 0; i < n-1; ++i) {
        if(theLargest < arr[i+1]){
            theLargest = arr[i+1];
        }
    }
    int count = 0;

    struct valueRecord rec;
    rec.value = theLargest;

    for (int i = 0; i < n; ++i) {
        if(rec.value == arr[i]){
            count++;
        }
    }

    rec.count = count;
    rec.index = malloc(sizeof(int) * count);

    int j = 0;
    for (int i = 0; i < n; ++i) {
        if(theLargest == arr[i]){
            rec.index[j] = i;
            j++;
        }
    }

    return rec;
}


