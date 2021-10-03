/*
 * To compile -> gcc -g -Wall -fopenmp -o 6.3 6.3.c
 * TO run     -> ./6.3
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include "omp.h"

#define DEBUG 0
#define THRESHOLD (5e-3)
#define innerMost /*
 * 1. outterMost -> a
 * 2. innerMost -> b
 *
 */
int thread_count;

void init_phi(double *phi, int n);

void compute_outter(double *cur, double *next, int n);

void compute_inner(double *cur, double *next, int n);

int converged(double *cur, double *next, int n);

int compute_conv(double *cur, double *next, int n);

int compute_single_region(double *cur, double *next, int n);

int main(int argc, char *argv[]) {
    int n, niters, conv, n_array[3] = {100, 500, 1000};
    double *phi_cur, *phi_next, *tmp;
    double start, end, elapsed;
//    if (argc != 2)
//    {
//        fprintf( stderr, "Usage: %s <n>\n", argv[0] );
//        exit( -1 );
//    }


//    n = atoi(argv[1]);

//--------------------------------compute_outter------------------------------------------------------------------------
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j < 16; j <<= 1) {

            n = n_array[i];
            phi_cur = (double *) malloc(n * n * sizeof(double));
            phi_next = (double *) malloc(n * n * sizeof(double));
            thread_count = j;
            init_phi(phi_cur, n);
            init_phi(phi_next, n);
            niters = 0;
            start = omp_get_wtime();
            while (1) {
                niters++;
#if DEBUG
                printf("Iteration %d\n", niters );
                int i,j;
                for ( i = 0; i < n; i++ )
                {
                    printf("[" );
                    for ( j = 0; j < n; j++ )
                        printf(" %10.6f ", phi_cur[j*n + i] );
                    printf("]\n" );
                }
                sleep(1);
#endif
                // Compute next (new) phi from current (old) phi
                compute_outter(phi_cur, phi_next, n);
                // If converged, we are done
                conv = converged(phi_cur, phi_next, n);
                if (conv)
                    break;
                // Otherwise, swap pointers and continue
                tmp = phi_cur;
                phi_cur = phi_next;
                phi_next = tmp;
            }
            end = omp_get_wtime();
            free(phi_cur);
            free(phi_next);
            elapsed = end - start;
            printf("[compute_outter] Converged after %d iterations, elapsed time -> %lf, thread_number -> %d n-> %d \n",
                   niters, elapsed,
                   thread_count, n);
        }
    }

//--------------------------------compute_inner--------------------------------------------------------------------------------
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j < 16; j <<= 1) {

            n = n_array[i];
            phi_cur = (double *) malloc(n * n * sizeof(double));
            phi_next = (double *) malloc(n * n * sizeof(double));
            thread_count = j;
            init_phi(phi_cur, n);
            init_phi(phi_next, n);
            niters = 0;
            start = omp_get_wtime();
            while (1) {
                niters++;
#if DEBUG
                printf("Iteration %d\n", niters );
                int i,j;
                for ( i = 0; i < n; i++ )
                {
                    printf("[" );
                    for ( j = 0; j < n; j++ )
                        printf(" %10.6f ", phi_cur[j*n + i] );
                    printf("]\n" );
                }
                sleep(1);
#endif
                // Compute next (new) phi from current (old) phi
                compute_inner(phi_cur, phi_next, n);
                // If converged, we are done
                conv = converged(phi_cur, phi_next, n);
                if (conv)
                    break;
                // Otherwise, swap pointers and continue
                tmp = phi_cur;
                phi_cur = phi_next;
                phi_next = tmp;
            }
            end = omp_get_wtime();
            free(phi_cur);
            free(phi_next);
            elapsed = end - start;
            printf("[compute_inner] Converged after %d iterations, elapsed time -> %lf, thread_number -> %d n-> %d \n",
                   niters, elapsed,
                   thread_count, n);
        }
    }

//--------------------------------compute_outter & compute_conv---------------------------------------------------------
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j < 16; j <<= 1) {

            n = n_array[i];
            phi_cur = (double *) malloc(n * n * sizeof(double));
            phi_next = (double *) malloc(n * n * sizeof(double));
            thread_count = j;
            init_phi(phi_cur, n);
            init_phi(phi_next, n);
            niters = 0;
            start = omp_get_wtime();
            while (1) {
                niters++;
#if DEBUG
                printf("Iteration %d\n", niters );
                int i,j;
                for ( i = 0; i < n; i++ )
                {
                    printf("[" );
                    for ( j = 0; j < n; j++ )
                        printf(" %10.6f ", phi_cur[j*n + i] );
                    printf("]\n" );
                }
                sleep(1);
#endif
                // Compute next (new) phi from current (old) phi
                compute_outter(phi_cur, phi_next, n);
                // If converged, we are done
                conv = compute_conv(phi_cur, phi_next, n);
                if (conv)
                    break;
                // Otherwise, swap pointers and continue
                tmp = phi_cur;
                phi_cur = phi_next;
                phi_next = tmp;
            }
            end = omp_get_wtime();
            free(phi_cur);
            free(phi_next);
            elapsed = end - start;
            printf("[compute_conv] Converged after %d iterations, elapsed time -> %lf, thread_number -> %d n-> %d \n",
                   niters, elapsed,
                   thread_count, n);
        }
    }
//--------------------------------compute_outter & compute_single_region--------------------------------------------------------------------------
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j < 16; j <<= 1) {
            n = n_array[i];
            phi_cur = (double *) malloc(n * n * sizeof(double));
            phi_next = (double *) malloc(n * n * sizeof(double));
            thread_count = j;
            init_phi(phi_cur, n);
            init_phi(phi_next, n);
            niters = 0;
            start = omp_get_wtime();
            while (1) {
                niters++;
#if DEBUG
                printf("Iteration %d\n", niters );
                int i,j;
                for ( i = 0; i < n; i++ )
                {
                    printf("[" );
                    for ( j = 0; j < n; j++ )
                        printf(" %10.6f ", phi_cur[j*n + i] );
                    printf("]\n" );
                }
                sleep(1);
#endif
                // Compute next (new) phi from current (old) phi
                compute_outter(phi_cur, phi_next, n);
                // If converged, we are done
                conv = compute_single_region(phi_cur, phi_next, n);
                if (conv)
                    break;
                // Otherwise, swap pointers and continue
                tmp = phi_cur;
                phi_cur = phi_next;
                phi_next = tmp;
            }
            end = omp_get_wtime();
            free(phi_cur);
            free(phi_next);
            elapsed = end - start;
            printf("[compute_single_region] Converged after %d iterations, elapsed time -> %lf, thread_number -> %d n-> %d \n",
                   niters, elapsed,
                   thread_count, n);


        }
    }
    return 0;
}


void init_phi(double *phi, int n) {
    int i, j;

    // Interior points initialized to 50 degrees
    for (i = 1; i < n - 1; i++)
        for (j = 1; j < n - 1; j++)
            phi[j * n + i] = 50.0;

    // Top, left, and right boundaries fixed at 100 degrees
    for (i = 0; i < n; i++) {
        phi[0 * n + i] = 100.0;
        phi[(n - 1) * n + i] = 100.0;
        phi[i * n + 0] = 100.0;
    }
    // Bottom boundary fixed at 0 degrees
    for (i = 0; i < n; i++)
        phi[i * n + (n - 1)] = 0.0;
}

void compute_outter(double *cur, double *next, int n) {
    int i, j;
    double temp;

#pragma omp parallel for default(none) num_threads(thread_count) shared(n, cur, next) private(j, i, temp)
    for (j = 1; j < n - 1; j++) {
        for (i = 1; i < n - 1; i++) {
            next[j * n + i] =
                    (cur[(j - 1) * n + i] + cur[j * n + (i - 1)] + cur[j * n + (i + 1)] + cur[(j + 1) * n + i]) / 4;
//#pragma omp critical
//            next[j * n + i] = temp;
        }
    }
}

void compute_inner(double *cur, double *next, int n) {
    int i, j;
    double temp;

    for (j = 1; j < n - 1; j++) {
#pragma omp parallel for  default(none) num_threads(thread_count) shared(n, cur, next, j) private(i, temp)
        for (i = 1; i < n - 1; i++) {
            next[j * n + i] =
                    (cur[(j - 1) * n + i] + cur[j * n + (i - 1)] + cur[j * n + (i + 1)] + cur[(j + 1) * n + i]) / 4;
//#pragma omp critical
//            next[j * n + i] = temp;
        }
    }


}

int converged(double *cur, double *next, int n) {
    int i, j;

    for (j = 1; j < n - 1; j++)
        for (i = 1; i < n - 1; i++)
            if (fabs(next[j * n + i] - cur[j * n + i]) > THRESHOLD)
                return 0;
    return 1;
}

int compute_conv(double *cur, double *next, int n) {
    int i, j;
    bool flag = true;
#pragma omp parallel for default(none) num_threads(thread_count) shared(n, cur, next, flag) private(i, j)
    for (j = 1; j < n - 1; j++) {
#pragma omp parallel for
        for (i = 1; i < n - 1; i++)
            if ((fabs(next[j * n + i] - cur[j * n + i]) > THRESHOLD) && flag == true) {
#pragma omp critical
                flag = false;
            }

    }

    if (flag == false) {
        return 0;
    } else {
        return 1;
    }

}

int compute_single_region(double *cur, double *next, int n) {
    int i, j;
    bool flag = true;
#pragma omp parallel for default(none) num_threads(thread_count) shared(n, cur, flag, next) private(i, j)
    for (j = 1; j < n - 1; j++) {
        for (i = 1; i < n - 1; i++)
            if (fabs(next[j * n + i] - cur[j * n + i]) > THRESHOLD && flag == true)
                flag = false;
    }

    if (flag == false) {
        return 0;
    } else {
        return 1;
    }

}