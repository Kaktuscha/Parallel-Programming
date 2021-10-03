/*
 * To compile -> gcc -g -Wall -fopenmp -o 7.3 7.3.c
 * To run     -> ./7.3
 * export OMP_SCHEDULE="guided, 500"
 */


#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "omp.h"

void row_Oriented_Inner(double A[], double b[], double x[], int n, int thread_count);
void column_Oriented_Inner(double A[], double b[], double x[], int n, int thread_count);
void generatorMatrixVector(double A[], double b[],   int n);
void matrixVectorPrint(double A[], double b[],   int n);
void printVector(double x[], int n);
int main() {
    int n = 10000;
    double *a = malloc(n * n * sizeof(double ));
    double *b = malloc(n * n * sizeof(double ));
    double *x =  malloc(n * sizeof(double ));
    double *y =  malloc(n * sizeof(double ));
    double start, end;
    generatorMatrixVector(a, b, n);
    for (int thread_count  = 1; thread_count  < 16; thread_count<<=1) {
        start = omp_get_wtime();
        column_Oriented_Inner(a, b, x, n, thread_count);
        end = omp_get_wtime();
        printf("[Column] The elapsed time -> %f, thread_count -> %d, n -> %d, thread_count->%d\n", end - start,
               thread_count, n, thread_count);
    }
    for (int thread_count  = 1; thread_count  < 16; thread_count<<=1) {
        start = omp_get_wtime();
        row_Oriented_Inner(a, b, y, n, thread_count);
        end = omp_get_wtime();
        printf("[Row] The elapsed time -> %f, thread_count -> %d, n -> %d, thread_count->%d\n", end - start,
               thread_count, n, thread_count);
    }
    return 0;
}

void row_Oriented_Inner(double A[], double b[], double x[], int n, int thread_count){
    double temp;
    int row, col;
#pragma omp parallel default(none) num_threads(thread_count) \
    shared(n, b, A, temp, x) private(col, row)
    for ( row = n-1; row >= 0; row--) {
#pragma omp single
        temp = b[row];
#pragma omp for reduction(+:temp) schedule(runtime)
        for ( col = row + 1; col < n; col++) {
            temp += -A[row*n+col] * x[col];
        }
#pragma omp single  // using ciritical gives strange result as it is distributed over process so I used again single
        x[row] = temp /  A[row*n+row] ;
    }
}

void column_Oriented_Inner(double A[], double b[], double x[], int n, int thread_count){
    int col, row;
#pragma omp parallel default(none) num_threads(thread_count) \
    shared(n, b, A, x) private(col, row)
    {
#pragma omp for
        for (int row = 0; row < n; ++row) {
            x[row] = b[row];
        }
        for (col = n - 1; col >= 0 ; col--) {
#pragma omp single
            x[col] /= A[col*n+col];
#pragma omp for schedule(runtime)
            for (row = 0; row < col; row++) {
                x[row] +=- A[row*n+col] * x[col];
            }
        }
    }



}

void generatorMatrixVector(double A[], double b[],   int n){
    srand(1);
    for (int col = 0; col < n; ++col) {
        for (int row = 0; row < n; ++row) {
            A[row*n+col] =  random()/((double) RAND_MAX);
        }
    }
    srand(2);
    for (int i = 0; i < n; ++i) {
        b[i] =  random()/((double) RAND_MAX);
    }
}
void matrixVectorPrint(double A[], double b[],   int n){

    printf("The generated matrix\n");
    for (int col = 0; col < n; ++col) {
        for (int row = 0; row < n; ++row) {
            printf("%f\t", A[row*n+col]);
        }
        printf("\n");
    }
    printf("The generated vector\n");
    for (int i = 0; i < n; ++i) {
        printf("%f\t", b[i]);
    }
    printf("\n");

}
void printVector(double x[], int n){

    printf("The vector is: \n");
    for (int i = 0; i < n; ++i) {
        printf("%10.4f\t", x[i]);
    }
    printf("\n");

}