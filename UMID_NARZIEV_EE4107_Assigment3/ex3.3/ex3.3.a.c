/*
 * 3.3.a
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "mpi/mpi.h"

#define DEBUG 0
#define THRESHOLD (5e-3)

void init_phi(double *phi, int n, int end, int my_rank, int comm_sz);

void update(double *cur, double *next, int n, int end, int my_rank, int comm_sz);

int converged(double *cur, double *next, int n, int colEnd);

//#define PRINT // -> unncomment it to print the result

int main(int argc, char *argv[]) {
    int my_rank, comm_sz;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int n, niters, conv;
    double *phi_cur, *phi_next, *tmp;
    if (my_rank == 0) {
        printf("\nEnter the n:");
        scanf("%d", &n);
    }
    MPI_Bcast(&n, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int count = n / comm_sz;
    int remainder = n % comm_sz;
    int start, stop;
    if (my_rank < remainder) {
// The first 'remainder' ranks get 'count + 1' tasks each
        start = my_rank * (count + 1);
        stop = start + count;
    } else {
// The remaining 'size - remainder' ranks get 'count' task each
        start = my_rank * count + remainder;
        stop = start + (count - 1);
    }

    int endCol = stop - start + 1;
    phi_cur = (double *) malloc(endCol * n * sizeof(double));
    phi_next = (double *) malloc(endCol * n * sizeof(double));

    init_phi(phi_cur, n, endCol, my_rank, comm_sz);
    init_phi(phi_next, n, endCol, my_rank, comm_sz);

    niters = 0;
    while (1) {
        niters++;
#if DEBUG
        printf("Iteration %d\n", niters );
        int i,j;
        for ( i = 0; i < endCol; i++ )
        {
            printf("[" );
            for ( j = 0; j < n; j++ )
                printf(" %10.6f ", phi_cur[j*endCol + i] );
            printf("]\n" );
        }
        sleep(1);
#endif
// Compute next (new) phi from current (old) phi
        update(phi_cur, phi_next, n, endCol, my_rank, comm_sz);
// If converged, we are done
        conv = converged(phi_cur, phi_next, n, endCol);

        int *rec = NULL;
        if (my_rank == 0) {
            rec = malloc(comm_sz * sizeof(int));
        }
        MPI_Gather(&conv, 1, MPI_INT, rec, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (my_rank == 0) {
            for (int j = 0; j < comm_sz; ++j) {
                conv = rec[j];
            }
        }
        MPI_Bcast(&conv, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        if (conv)
            break;
        // Otherwise, swap pointers and continue
        tmp = phi_cur;
        phi_cur = phi_next;
        phi_next = tmp;
    }


    MPI_Barrier(MPI_COMM_WORLD);
#ifdef PRINT
    for (int i = 0; i < endCol; i++) {

        for (int j = 0; j < n; j++) {
            printf(" %10.3f ", phi_next[j * endCol + i]);
        }
        printf("\n");
    }
#endif

    free(phi_cur);
    free(phi_next);

    printf("Converged after %d iterations\n", niters);

    MPI_Finalize();
    return 0;
}


void init_phi(double *phi, int n, int end, int my_rank, int comm_sz) {

    if (my_rank == 0) {
        for (int col = 1; col < end; col++)
            for (int row = 1; row < n - 1; row++)
                phi[row * end + col] = 50.0;

        for (int col = 0; col < end; col++) {
            for (int row = 0; row < n - 1; row++) {
                phi[0 * end + col] = 100.0;
                phi[(n - 1) * end + col] = 100.0;
                phi[row * end + 0] = 100.0;
            }
        }

    } else if (my_rank == comm_sz - 1) {
        for (int col = 0; col < end - 1; col++)
            for (int row = 1; row < n - 1; row++)
                phi[row * end + col] = 50.0;
        for (int col = 0; col < end - 1; col++) {
            for (int row = 0; row < n - 1; row++) {
                phi[0 * end + col] = 100.0;
                phi[(n - 1) * end + col] = 100.0;
            }
        }
        for (int col = 0; col < end; col++) {
            for (int row = 0; row < n; row++) {
                phi[row * end + end - 1] = 0.0;
            }
        }
    } else {
        for (int col = 0; col < end; col++)
            for (int row = 1; row < n - 1; row++)
                phi[row * end + col] = 50.0;

        for (int col = 0; col < end; col++) {
            for (int row = 0; row < n - 1; row++) {
                phi[0 * end + col] = 100.0;
                phi[(n - 1) * end + col] = 100.0;
            }
        }
    }

}

void update(double *cur, double *next, int n, int colEnd, int my_rank, int comm_sz) {

    double tempRow[n];
    double tempRowBelow[n];
    if (my_rank == 0) {
        for (int i = 0; i < n; ++i) {
            MPI_Sendrecv(&cur[i * colEnd + (colEnd - 1)], 1, MPI_DOUBLE, my_rank + 1, 0, &tempRow[i], 1, MPI_DOUBLE,
                         my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
    } else if (my_rank == comm_sz - 1) {
        for (int i = 0; i < n; ++i) {
            MPI_Sendrecv(&cur[i * colEnd + 0], 1, MPI_DOUBLE, my_rank - 1, 0, &tempRow[i], 1, MPI_DOUBLE, my_rank - 1,
                         0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
    } else {
        for (int i = 0; i < n; ++i) {
            MPI_Sendrecv(&cur[i * colEnd + 0], 1, MPI_DOUBLE, my_rank - 1, 0, &tempRow[i], 1, MPI_DOUBLE, my_rank - 1,
                         0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Sendrecv(&cur[i * colEnd + (colEnd - 1)], 1, MPI_DOUBLE, my_rank + 1, 0, &tempRowBelow[i], 1,
                         MPI_DOUBLE, my_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
    }

    if (my_rank == 0) {
        for (int col = 1; col < colEnd; col++) {
            for (int row = 1; row < n - 1; row++) {
                if (col == colEnd - 1) {
                    next[row * colEnd + col] =
                            (cur[(row - 1) * colEnd + col] + cur[row * colEnd + (col - 1)] + tempRow[row] +
                             cur[(row + 1) * colEnd + col]) / 4;
                } else {
                    next[row * colEnd + col] = (cur[(row - 1) * colEnd + col] + cur[row * colEnd + (col - 1)] +
                                                cur[row * colEnd + (col + 1)] + cur[(row + 1) * colEnd + col]) / 4;
                }
            }
        }
    } else if (my_rank == comm_sz - 1) {
        for (int col = 0; col < colEnd - 1; col++) {
            for (int row = 1; row < n - 1; row++) {
                if (col == 0) {
                    next[row * colEnd + col] =
                            (cur[(row - 1) * colEnd + col] + tempRow[row] + cur[row * colEnd + (col + 1)] +
                             cur[(row + 1) * colEnd + col]) / 4;
                } else {
                    next[row * colEnd + col] = (cur[(row - 1) * colEnd + col] + cur[row * colEnd + (col - 1)] +
                                                cur[row * colEnd + (col + 1)] + cur[(row + 1) * colEnd + col]) / 4;
                }
            }
        }
    } else {
        for (int col = 0; col < colEnd; col++) {
            for (int row = 1; row < n - 1; row++) {
                if (col == 0) {
                    next[row * colEnd + col] =
                            (cur[(row - 1) * colEnd + col] + cur[row * colEnd + (col + 1)] + tempRow[row] +
                             cur[(row + 1) * colEnd + col]) / 4;
                } else if (col == colEnd - 1) {
                    next[row * colEnd + col] =
                            (cur[(row - 1) * colEnd + col] + tempRow[row] + cur[row * colEnd + (col - 1)] +
                             cur[(row + 1) * colEnd + col]) / 4;
                } else {
                    next[row * colEnd + col] = (cur[(row - 1) * colEnd + col] + cur[row * colEnd + (col - 1)] +
                                                cur[row * colEnd + (col + 1)] + cur[(row + 1) * colEnd + col]) / 4;
                }
            }
        }
    }
}

int converged(double *cur, double *next, int n, int colEnd) {
    int i, j;

    for (j = 1; j < colEnd - 1; j++)
        for (i = 1; i < n - 1; i++)
            if (fabs(next[j * n + i] - cur[j * n + i]) > THRESHOLD)
                return 0;
    return 1;
}
