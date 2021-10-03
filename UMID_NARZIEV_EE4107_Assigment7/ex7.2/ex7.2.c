/*
 * To compile ->  gcc -g -Wall -fopenmp -o 7.2 7.2.c
 * To run     -> ./7.2 thread_number number_of_element
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "omp.h"

//#define PRINT // To see the elements

void count_Sort_Serial(int a[], int n);
void count_Sort_Parallel(int a[], int n);
void print(int a[], int n);
void usage(char* prog_name);
void array_Generating(int a[], int n);
int comparator(const void * p1, const void * p2);
int thread_count;

int main(int argc, char* argv[]) {


    int n; /// number of elements
    int *a, *b, *c; /// pointer to the array
    double start, end, temp_Parallel, temp_Serial, temp_qSort;  /// timing variables

    if (argc != 3) usage(argv[0]);
    thread_count = strtol(argv[1], NULL, 10);
    n = strtol(argv[2], NULL, 10);
    a = malloc(n * sizeof(int));
    b = malloc(n * sizeof(int));
    c = malloc(n * sizeof(int));

    array_Generating(a, n);
    memcpy(b, a, n * sizeof(int));// Duplicating the values
    memcpy(c, a, n * sizeof(int));// Duplicating the values
#ifdef PRINT
    print(a, n);
#endif
    //-------------------------------Parallel Count Sort------------------------------------------------------------
    start = omp_get_wtime();
    count_Sort_Parallel(a,n);
    end = omp_get_wtime();
#ifdef PRINT
    printf("After Parallel Count Sort function: \n");
    print(a, n);
#endif
    temp_Parallel = end - start;
    //-------------------------------Serial Count Sort------------------------------------------------------------
    start = omp_get_wtime();
    count_Sort_Serial(b,n);
    end = omp_get_wtime();
#ifdef PRINT
    printf("After Serial Count Sort function: \n");
    print(b, n);
#endif
    temp_Serial = end - start;
    //-------------------------------qSort Count Sort------------------------------------------------------------
    start = omp_get_wtime();
    qsort(c, n, sizeof(int), comparator);
    end = omp_get_wtime();
#ifdef PRINT
    printf("After qSort Count Sort function: \n");
    print(b, n);
#endif
    temp_qSort = end - start;
    //-------------------------------The result printing ---------------------------------------------------------
    printf("The elapsed time is %e for Parallel count sort function with %d elements.\n", temp_Parallel, n);
    printf("The elapsed time is %e for Serial count sort function with %d elements.\n", temp_Serial, n);
    printf("The elapsed time is %e for Qsort count sort function with %d elements.\n", temp_qSort, n);
    return 0;
}

void usage(char* prog_name) {

    fprintf(stderr, "usage: %s <number of threads>, <number of elements>\n", prog_name);
    exit(0);
}
void array_Generating(int a[], int n){

    for (int i = 0; i < n; ++i) {
        srand(i);
        a[i] = rand() % n;
    }
}
void print(int a[], int n){
    for (int i = 0; i < n; i++) {
        printf("%d\t", a[i]);
    }
    printf("\n");
}
void count_Sort_Serial(int a[], int n){
    int count;
    int *temp = malloc(n*sizeof(int));

    for (int i = 0; i < n; ++i) {
        count = 0;
        for (int j = 0; j < n; ++j) {
            if(a[j]<a[i]){
                count++;
            } else if((a[j] == a[i]) && (j < i)){
                count++;
            }
        }
        temp[count] = a[i];
    }

    memcpy(a, temp, n * sizeof(int));
    free(temp);
}
void count_Sort_Parallel(int a[], int n){
    int count, i, j;
    int *temp = malloc(n*sizeof(int));
#pragma omp parallel for num_threads(thread_count) default(none) shared(a, n,  temp) private(j, i, count)
    for (i = 0; i < n; ++i) {
        count = 0;
        for (j = 0; j < n; ++j) {
            if(a[j] < a[i]){
                count++;
            } else if((a[j] == a[i]) && (j < i)){
                count++;
            }
        }
#pragma omp critical
        {
            temp[count] = a[i];
        };

    }

    memcpy(a, temp, n * sizeof(int));

    free(temp);
}
int comparator (const void * p1, const void * p2){
    return (*(int*)p1 - *(int*)p2);
}
