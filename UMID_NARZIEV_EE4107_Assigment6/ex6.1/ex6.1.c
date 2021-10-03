/*
 * To compile ->  gcc -g -Wall -fopenmp -o 6.1 6.1.c
 * To run     -> ./6.1
 */

#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include "timer.h"

long long int trap(int thread_count, long long int global_number_of_tosses);

int main(int argc, char *argv[]) {

    long long int global_number_in_circle;
    float pi_estimate;
    double start, finish;
    for (int ii = 1; ii <= 16; ii <<= 1) {
        for (int jj = 1000000; jj <= 100000000; jj *= 10) {
            int thread_count = ii;
            long long int number_of_tosses = jj;
            global_number_in_circle = 0;
            GET_TIME(start);
#pragma omp parallel  num_threads(thread_count) reduction(+: global_number_in_circle)
            {
                global_number_in_circle += trap(thread_count, number_of_tosses);

            }
            GET_TIME(finish);
            pi_estimate = (4.0 * (double) global_number_in_circle) / ((double) number_of_tosses);
            printf("Number of threads -> %d, Result -> %f, Elapsed time -> %lf seconds, number of tosses -> %lld\n",
                   ii, pi_estimate, finish - start, number_of_tosses);
        }
    }


    return 0;
}

long long int trap(int thread_count, long long int global_number_of_tosses) {

    double x, y, distance_squared;
    long long int number_in_circle = 0;
    long long int number_of_tosses = global_number_of_tosses / thread_count;
    for (long long int toss = 0;
         toss < number_of_tosses; toss++) { // number of tosses should be equally divided among processor
        x = rand() / (RAND_MAX + 1.0) * (1 - (-1)) + (-1);
        y = rand() / (RAND_MAX + 1.0) * (1 - (-1)) + (-1);
        distance_squared = x * x + y * y;
        if (distance_squared <= 1) {
            number_in_circle++;
        }
    }
    return number_in_circle;
}



//#pragma clang diagnostic pop