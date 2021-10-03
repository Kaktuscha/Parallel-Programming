#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include <semaphore.h>
#include "timer.h"

/*
 * compiling -> gcc -g -Wall -o 5.1 5.1.c -pthread
 * No argument is required
 */
long long int a, b, n;
int thread_count;
double global_result_busy_waiting = 0;
double global_result_mutex = 0;
double global_result_sempahore = 0;
int flag;
pthread_mutex_t mut;
sem_t semaphores;

void *trap_busy_waiting(void *rank);

void *trap_mutex(void *rank);

void *trap_semaphore(void *rank);

int main(int argc, char *argv[]) {

    double start, end;
    pthread_t *thread_handles;
    for (int thread_number = 1; thread_number < 128; thread_number <<= 1) {
        thread_count = thread_number;
        a = 0;
        b = 10000;
        n = 1000000000;
        thread_handles = malloc(thread_count * sizeof(pthread_t));
        pthread_mutex_init(&mut, NULL);
        sem_init(&semaphores, 1, 1);
        global_result_busy_waiting = 0;
        global_result_mutex = 0;
        global_result_sempahore = 0;
        //--------------------------------------------Busy-Waiting---------------------------------------------
        GET_TIME(start);
        for (long thread = 0; thread < thread_count; thread++) {
            pthread_create(&thread_handles[thread], NULL, trap_busy_waiting, (void *) thread);
        }

        for (long thread = 0; thread < thread_count; thread++) {
            pthread_join(thread_handles[thread], NULL);
        }
        GET_TIME(end);
        printf("The elapsed time -> %e, estimated value is %lf from busy waiting function, thread_count -> %d\n", end - start,
               global_result_busy_waiting, thread_count);

        //--------------------------------------------Mutex---------------------------------------------
        GET_TIME(start);
        for (long thread = 0; thread < thread_count; thread++) {
            pthread_create(&thread_handles[thread], NULL, trap_mutex, (void *) thread);
        }

        for (long thread = 0; thread < thread_count; thread++) {
            pthread_join(thread_handles[thread], NULL);
        }
        GET_TIME(end);
        printf("The elapsed time -> %e, estimated value is %lf from mutex function, thread_count -> %d\n", end - start,
               global_result_mutex, thread_count);
        //----------------------------------Semaphore-------------------------------------------------------
        GET_TIME(start);
        for (long thread = 0; thread < thread_count; thread++) {
            pthread_create(&thread_handles[thread], NULL, trap_semaphore, (void *) thread);
        }

        for (long thread = 0; thread < thread_count; thread++) {
            pthread_join(thread_handles[thread], NULL);
        }
        GET_TIME(end);
        printf("The elapsed time -> %e, estimated value is %lf from semaphore function, thread_count -> %d\n", end - start,
               global_result_sempahore, thread_count);
        printf("\n");
    }
    free(thread_handles);
    pthread_mutex_destroy(&mut);
    sem_destroy(&semaphores);

    return 0;
}


void *trap_busy_waiting(void *rank) {
    double my_rank = (long) rank;

    double h = (double) (b - a) / n;
    double local_n = (double) n / thread_count;
    double local_a = a + my_rank * local_n * h;
    double local_b = local_a + local_n * h;

    double estimated = local_a * local_a + local_b * local_b;
    double x;
    for (int i = 1; i < local_n - 1; ++i) {
        x = local_a + i * h;
        estimated += (x * x);
    }
    estimated = estimated * h;

    while (flag != my_rank);
    global_result_busy_waiting += estimated;
    flag = (flag + 1) % thread_count;


    return NULL;
}


void *trap_mutex(void *rank) {
    double my_rank = (long) rank;

    double h = (double) (b - a) / n;
    double local_n = (double) n / thread_count;
    double local_a = a + my_rank * local_n * h;
    double local_b = local_a + local_n * h;

    double estimated = local_a * local_a + local_b * local_b;
    double x;
    for (int i = 1; i < local_n - 1; ++i) {
        x = local_a + i * h;
        estimated += (x * x);
    }
    estimated = estimated * h;

    pthread_mutex_lock(&mut);
    global_result_mutex += estimated;
    pthread_mutex_unlock(&mut);

    return NULL;
}


void *trap_semaphore(void *rank) {
    double my_rank = (long) rank;

    double h = (double) (b - a) / n;
    double local_n = (double) n / thread_count;
    double local_a = a + my_rank * local_n * h;
    double local_b = local_a + local_n * h;

    double estimated = local_a * local_a + local_b * local_b;
    double x;
    for (int i = 1; i < local_n - 1; ++i) {
        x = local_a + i * h;
        estimated += (x * x);
    }
    estimated = estimated * h;

    sem_post(&semaphores);
    global_result_sempahore += estimated;
    sem_wait(&semaphores);

    return NULL;
}
