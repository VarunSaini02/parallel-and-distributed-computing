#include "barrier.h"
#include "rc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

static int arrivals = 1; // For easily displaying how many threads have arrived at the barrier
static int departures = 1; // For easily displaying how many threads have passed through the barrier

typedef struct {
    barrier_t* barrier;
    int thread_num;
} thread_arg_t;

void* barrier_thread_routine(void* arg) {
    // Each thread waits a random number of seconds (10 seconds max) before arriving at the barrier
    thread_arg_t* thread_arg = arg;
    barrier_t* barrier = thread_arg->barrier;
    int thread_num = thread_arg->thread_num;

    // This malloc ensures a unique seed for each thread
    void** seed = malloc(sizeof(void*)); 
    srandom((long)seed);

    double sleep_secs = (double) 10 * random() / RAND_MAX;
    sleep(sleep_secs);
    printf("(%d)\tThread %d:\tarrived at the barrier\n", arrivals++, thread_num);
    barrier_wait(barrier);
    printf("(%d)\tThread %d:\tpassed through the barrier\n", departures++, thread_num);
    
    free(seed);
    return NULL;
}

rc_t barrier_broken(void* arg, void** result) {
    printf("\nBarrier was broken!\n");
    printf("All waiting threads will now pass through the barrier.\n");
    printf("Additional threads that arrive at the barrier will now also immediately pass through.\n\n");
    return Success;
}

int main(int argc, char* argv[]) {
    printf("\n");

    rc_t rc;
    barrier_t barrier;
    int barrier_capacity = 15;
    int num_threads_to_test = 30;

    rc = barrier_create(&barrier, barrier_capacity, barrier_broken, NULL, NULL);
    if (rc != Success) {
        fprintf(stderr, "main: barrier_create was not successful\n");
        return rc;
    }

    // The barrier should "break" after barrier_capacity threads arrive
    // and then allow all future threads to simply pass through.
    pthread_t threads[num_threads_to_test];
    thread_arg_t thread_args[num_threads_to_test];
    for (int i = 0; i < num_threads_to_test; i++) {
        thread_args[i] = (thread_arg_t) {.barrier = &barrier, .thread_num = i + 1};
        pthread_create(&threads[i], NULL, barrier_thread_routine, (void*)&thread_args[i]);
    }

    for (int i = 0; i < num_threads_to_test; i++) {
        pthread_join(threads[i], NULL);
    }

    rc = barrier_destroy(&barrier);
    if (rc != Success) {
        fprintf(stderr, "main: barrier_destroy was not successful\n");
        return rc;
    }
    
    printf("\nIf you can't see where the barrier was broken, you may have to scroll up a bit.\n");
    printf("You should be able to see that %d threads arrived (and were stuck waiting) at the barrier before it finally broke.\n\n", barrier_capacity);
    return Success;
}