/* File:
 * Compile:
 *    gcc -g -Wall -o 5.2 5.2.c -lpthread
 * Usage:
 *     5.2 <thread_count> <m> <n>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

//#define PRINT1 //------------------> define to see output of one-dim-matrix
//#define PRINT2 //------------------> define to see output of two-dim-matrix
/* Global variables */
#define mm 8192
#define nn 8192
int m = 8192, n = 8192;
int     thread_count;
double* A;
double AA[mm][nn];
double* x;
double* y;
double* yy;
/* Serial functions */
void Usage(char* prog_name);
void Gen_matrix(double A[], int m, int n);
void Gen_vector(double x[], int n);
void Print_matrix_one_dim(char* title, double A[], int m, int n);
void Print_matrix_two_dim(char* title, double A[], int m, int n);
void Print_vector(char* title, double y[], double m);

/* Parallel function */
void *Pth_mat_vect_one_dim(void* rank);
void *Pth_mat_vect_two_dim(void* rank);

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    long       thread;
    pthread_t* thread_handles;

    if (argc != 2) Usage(argv[0]);
    thread_count = strtol(argv[1], NULL, 10);


#  ifdef PRINT
    printf("thread_count =  %d, m = %d, n = %d\n", thread_count, m, n);
#  endif

    thread_handles = malloc(thread_count*sizeof(pthread_t));
    A = malloc(m*n*sizeof(double));
    x = malloc(n*sizeof(double));
    y = malloc(m*sizeof(double));
    yy = malloc(m*sizeof(double));
    Gen_matrix(A, m, n);
#ifdef PRINT1
    Print_matrix_one_dim("Generated one-dim matrix: ", A, m, n);
#endif
    Gen_vector(x, n);

#ifdef PRINT1
    Print_vector("Generated one-dim matrix", x, n);
#endif
    double start, finish;
    GET_TIME(start);
    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL,
                       Pth_mat_vect_one_dim,  (void*) thread);

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    GET_TIME(finish);
#ifdef PRINT1
    Print_vector("The product is", y, m);
#endif
    printf("[One_Dimensional_Matrix] -> Elapsed time is %f\n", finish - start);
//-------------------------------------------------------------------------------------------------------------
    for (int i = 0; i < n; ++i) { // copying matrix to AA
        for (int j = 0; j < m; ++j) {
            AA[i][j] = A[i*m+j];
        }
    }
#ifdef PRINT2
    Print_matrix_two_dim("Generated two-dim matrix: ", A, m, n);
#endif

#ifdef PRINT2
    Print_vector("Generated two-dim matrix", x, n);
#endif

    GET_TIME(start);
    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL,
                       Pth_mat_vect_two_dim, (void*) thread);

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    GET_TIME(finish);
#ifdef PRINT2
    Print_vector("The product is", yy, m);
#endif

    printf("[Two_Dimensional_Matrix] -> Elapsed time is %f\n", finish - start);

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
void Usage (char* prog_name) {
    fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
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
            A[i*n+j] = random()/((double) RAND_MAX);
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
        x[i] = random()/((double) RAND_MAX);
}  /* Gen_vector */



/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiply an mxn matrix by an nx1 column vector
 * In arg:         rank
 * Global in vars: A, x, m, n, thread_count
 * Global out var: y
 */
void *Pth_mat_vect_one_dim(void* rank) {
    long my_rank = (long) rank;
    int i;
    int j;
    int local_m = m/thread_count;
    int my_first_row = my_rank*local_m;
    int my_last_row = my_first_row + local_m;


#  ifdef DEBUG
    printf("Thread %ld > local_m = %d, sub = %d\n",
         my_rank, local_m, sub);
#  endif


    for (i = my_first_row; i < my_last_row; i++) {
        y[i] = 0.0;
        for (j = 0; j < n; j++) {
           y[i] +=A[i*n+j];
        }
    }



    return NULL;
}  /* Pth_mat_vect */

/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiply an mxn matrix by an nx1 column vector
 * In arg:         rank
 * Global in vars: A, x, m, n, thread_count
 * Global out var: y
 */
void *Pth_mat_vect_two_dim(void* rank) {
    long my_rank = (long) rank;
    int i;
    int j;
    int local_m = m/thread_count;
    int my_first_row = my_rank*local_m;
    int my_last_row = my_first_row + local_m;




#  ifdef DEBUG
    printf("Thread %ld > local_m = %d, sub = %d\n",
         my_rank, local_m, sub);
#  endif


    for (i = my_first_row; i < my_last_row; i++) {
        yy[i] = 0.0;
        for (j = 0; j < n; j++) {
           yy[i] += AA[i][j];
        }
    }



    return NULL;
}  /* Pth_mat_vect */



/*------------------------------------------------------------------
 * Function:    Print_matrix
 * Purpose:     Print the matrix
 * In args:     title, A, m, n
 */
void Print_matrix_one_dim( char* title, double A[], int m, int n) {
    int   i, j;

    printf("%s\n", title);
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++)
            printf("%6.3f ", A[i*n + j]);
        printf("\n");
    }
}  /* Print_matrix */

void Print_matrix_two_dim( char* title, double A[], int m, int n) {
    int   i, j;

    printf("%s\n", title);
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++)
            printf("%6.3f ", AA[i][j]);
        printf("\n");
    }
}  /* Print_matrix */

/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, m
 */
void Print_vector(char* title, double y[], double m) {
    int   i;

    printf("%s\n", title);
    for (i = 0; i < m; i++)
        printf("%6.3f ", y[i]);
    printf("\n");
}  /* Print_vector */
