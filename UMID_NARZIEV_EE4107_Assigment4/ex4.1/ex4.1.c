#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

long thread_count;
long long int number_of_tosses;
long long int number_in_circle;
pthread_mutex_t mutex;

void *estimate_PI(void *rank);

void Usage(char *prog_name);

int main(int argc, char *argv[]) {

    long thread;                /* Use long in case of a 64-bit system*/
    pthread_t *thread_handles;
    double start, finish, elapsed;
    if (argc != 1) Usage(argv[0]);
    pthread_mutex_init(&mutex, NULL);
    for (int thread_number = 1; thread_number < 16; thread_number <<= 1) {
        for (long long int number_of_toss = 100000; number_of_toss < 100000000; number_of_toss *= 10) {
            thread_count = thread_number;
            number_of_tosses = number_of_toss;
            number_in_circle = 0;
            thread_handles = (pthread_t *) malloc(thread_count * sizeof(pthread_t));
            GET_TIME(start);
            for (thread = 0; thread < thread_count; thread++)
                pthread_create(thread_handles + thread, NULL, (void *) estimate_PI,
                               (void *) thread);        /* or &thread_handles[thread]*/

            for (thread = 0; thread < thread_count; thread++)
                pthread_join(*(thread_handles + thread), NULL);                    /* or thread_handles[thread]  */


            double pi_estimate = 4 * ((double) number_in_circle / (double) (number_of_tosses));
            GET_TIME(finish);
            elapsed = finish - start;

            printf("pi_estimate -> %f, elapsed_time -> %0.5f, thread_count -> %ld, number_of_tosses -> %lld\n",
                   pi_estimate, elapsed, thread_count, number_of_tosses);
        }
    }
    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    return 0;

}


void *estimate_PI(void *rank) {

    long my_rank = (long) rank;
    long long int local_number_tosses = number_of_tosses / thread_count;
    double local_x = 0;
    double local_y = 0;
    double distance_squared = 0;
    long long int local_number_in_circle = 0;
    srand(my_rank);
    for (long long int i = 0; i < local_number_tosses; ++i) {
        local_x = rand() / (RAND_MAX + 1.0);
        local_y = rand() / (RAND_MAX + 1.0);

        distance_squared = local_x * local_x + local_y * local_y;
        if (distance_squared <= 1) {
            local_number_in_circle++;
        }
    }
    pthread_mutex_lock(&mutex);
    number_in_circle += local_number_in_circle;
    pthread_mutex_unlock(&mutex);
    return NULL;

}

void Usage(char *prog_name) {
    fprintf(stderr, "usage: %s No argument required to run this program\n", prog_name);
    exit(0);
}