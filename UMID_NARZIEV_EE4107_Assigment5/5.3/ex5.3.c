/*
 * Compile:
 *    gcc -g -Wall -o 5.3 5.3.c -lpthread
 * Usage:
 *     pth_mat_vect no argument is required!!!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define A_part
/*
 *          define A_part for 5.3.A
 *          define B_part for 5.3.B
 *
 */

/* Global variables */
int thread_count;
int m, n;
double *A;
double *x;
double *y;

/* Serial functions */
void Usage(char *prog_name);

void Gen_matrix(double A[], int m, int n);


void Gen_vector(double x[], int n);


void Print_matrix(char *title, double A[], int m, int n);

void Print_vector(char *title, double y[], double m);

/* Parallel function */
void *Pth_mat_vect(void *rank);

/*------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    long thread;
    pthread_t *thread_handles;

    if (argc != 1) Usage(argv[0]);
    thread_count = 4;
    m = 8;
    n = 8000000;

#  ifdef DEBUG
    printf("thread_count =  %d, m = %d, n = %d\n", thread_count, m, n);
#  endif

    thread_handles = malloc(thread_count * sizeof(pthread_t));
    A = malloc(m * n * sizeof(double));
    x = malloc(n * sizeof(double));
#ifdef A_part
    y = malloc((m + 8 * thread_count) * sizeof(double));
#endif
#ifdef B_part
    y = malloc((m + 8 * thread_count) * sizeof(double));
#endif
    Gen_matrix(A, m, n);
#  ifdef DEBUG
    Print_matrix("We generated", A, m, n);
#  endif

    Gen_vector(x, n);
#  ifdef DEBUG
    Print_vector("We generated", x, n);
#  endif
    double start, finish;
    GET_TIME(start);


    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL,
                       Pth_mat_vect, (void *) thread);

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    GET_TIME(finish);
    printf("Elapsed time = %e seconds\n", finish - start);
#  ifdef DEBUG
    Print_vector("The product is", y, m);
#  endif

    free(A);
    free(x);
    free(y);

    return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage(char *prog_name) {
    fprintf(stderr, "usage: %s no argument is required\n", prog_name);
    exit(0);
}  /* Usage */



/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  m, n
 * Out arg:  A
 */
void Gen_matrix(double A[], int m, int n) {
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            A[i * n + j] = random() / ((double) RAND_MAX);
}  /* Gen_matrix */

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Use the random number generator random to generate
 *    the entries in x
 * In arg:   n
 * Out arg:  A
 */
void Gen_vector(double x[], int n) {
    int i;
    for (i = 0; i < n; i++)
        x[i] = random() / ((double) RAND_MAX);
}  /* Gen_vector */




/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiply an mxn matrix by an nx1 column vector
 * In arg:         rank
 * Global in vars: A, x, m, n, thread_count
 * Global out var: y
 */
void *Pth_mat_vect(void *rank) {
    long my_rank = (long) rank;
    int i;
    int j;
    int local_m = m / thread_count;
    int my_first_row = my_rank * local_m;
    int my_last_row = (my_rank + 1) * local_m - 1;
    double temp;


#  ifdef DEBUG
    printf("Thread %ld > local_m = %d, sub = %d\n",
         my_rank, local_m, sub);
#  endif


    for (i = my_first_row; i < my_last_row; i++) {

#ifdef A_part
        y[i + (my_rank * 8)] = 0.0;
#endif
#ifdef B_part
        y[i] = 0.0;
        temp = 0.0;
#endif
        for (j = 0; j < n; j++) {
#ifdef A_part
            y[i + (my_rank * 8)] += A[i * n + j] * x[j];
#endif
#ifdef B_part
            temp += A[i * n + j] * x[j];
#endif
        }
#ifdef B_part
        y[i] = temp;
#endif
    }


    return NULL;
}  /* Pth_mat_vect */


/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void Print_matrix(char *title, double A[], int m, int n) {
    int i, j;

    printf("%s\n", title);
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++)
            printf("%6.3f ", A[i * n + j]);
        printf("\n");
    }
}  /* Print_matrix */


/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(char *title, double y[], double m) {
    int i;

    printf("%s\n", title);
    for (i = 0; i < m; i++)
        printf("%6.3f ", y[i]);
    printf("\n");
}  /* Print_vector */
