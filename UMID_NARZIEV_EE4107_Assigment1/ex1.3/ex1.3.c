#include <stdio.h>
#include <mpi/mpi.h>
#include <time.h>

#define MAX_CYCLE 16386


int main() {

    int my_rank, comm_sz;
    int AA[MAX_CYCLE];
    double start, elapsed, elapsed_t;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    if(comm_sz != 2){
        if (my_rank == 0){
            printf("Sorry only 2 process are needed to run this program\n");
        }
        MPI_Finalize();
        return -1;
    }
    for (int i = 0; i < MAX_CYCLE; ++i) {
        AA[i] = 10;
    }



    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank == 0) {
        start = clock() / (double) CLOCKS_PER_SEC;
        for (int i = 0; i < MAX_CYCLE; ++i) {
            MPI_Send(&AA[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&AA[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }

        elapsed_t = clock() / (double) CLOCKS_PER_SEC - start;
    } else if (my_rank == 1) {
        for (int i = 0; i < MAX_CYCLE; ++i) {
            MPI_Recv(&AA[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Send(&AA[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank == 0) {
        start = MPI_Wtime();
        for (int i = 0; i < MAX_CYCLE; ++i) {
            MPI_Send(&AA[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&AA[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
        elapsed = MPI_Wtime() - start;
    } else if (my_rank == 1) {
        for (int i = 0; i < MAX_CYCLE; ++i) {
            MPI_Recv(&AA[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Send(&AA[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        }
    }

    if (my_rank == 0) {
        printf("Clock function elapsed time -> %e\n", elapsed_t/(2*MAX_CYCLE));
        printf("MPI_Wtime with Elapsed time -> %e\n", elapsed/(2*MAX_CYCLE));
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (my_rank == 0) {
        start = MPI_Wtime();
        for (int i = 0; i < MAX_CYCLE; ++i) {
            MPI_Send(&AA[i], 0, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&AA[i], 0, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }
        elapsed = MPI_Wtime() - start;
    } else if (my_rank == 1) {
        for (int i = 0; i < MAX_CYCLE; ++i) {
            MPI_Recv(&AA[i], 0, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            MPI_Send(&AA[i], 0, MPI_INT, 0, 0, MPI_COMM_WORLD);

        }
    }
    if (my_rank == 0) {
        printf("With count -> 0, MPI_Wtime -> %e\n", elapsed/(2*MAX_CYCLE));
    }

    MPI_Finalize();
    return 0;
}