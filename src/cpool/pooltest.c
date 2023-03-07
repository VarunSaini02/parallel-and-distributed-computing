#include "pool.h"
#include "rc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

rc_t monte_carlo_pi_calculation(void* arg, void** result) {
    int* num_in_quarter_circle = malloc(sizeof(int));
    int total = *(int*)arg;
    *num_in_quarter_circle = 0;

    // the following line ensures a unique seed for each call of this function
    long seed = (long)num_in_quarter_circle;
    srandom(seed);
    for (int i = 0; i < total; i++) {
        double x = (double) random() / RAND_MAX;
        double y = (double) random() / RAND_MAX;
        if (x * x + y * y <= 1) {
            (*num_in_quarter_circle)++;
        }
    }

    *result = num_in_quarter_circle;
    return Success;
}

int main(int argc, char* argv[]) {
    rc_t rc;
    thread_pool_t pool;

    int pool_size = 25;
    int num_tasks = 2500;
    int num_per_task = 1000;

    rc = pool_create(&pool, pool_size);
    if (rc != Success) {
        fprintf(stderr, "main: pool_create was not successful\n");
        return rc;
    }
    printf("\nCreated fixed thread pool with %d threads\n", pool_size);

    void* args[num_tasks];
    int totals[num_tasks];
    for (int i = 0; i < num_tasks; i++) {
        totals[i] = num_per_task;
        args[i] = &totals[i];
    }
    void* results[num_tasks];

    printf("Mapping Monte Carlo Pi Calculation (%d tasks, %d coordinates per task)\n", num_tasks, num_per_task);
    printf("This might take a few seconds... ");
    fflush(stdout);
    rc = pool_map(&pool, monte_carlo_pi_calculation, num_tasks, args, results);
    if (rc != Success) {
        fprintf(stderr, "main: pool_map was not successful\n");
        return rc;
    }
    printf("done\n");
    fflush(stdout);

    int num_in_quarter_circle = 0;
    int total = 0;
    for (int i = 0; i < num_tasks; i++) {
        total += *(int*)args[i];
        num_in_quarter_circle += *(int*)results[i];
        free(results[i]);
    }
    printf("\nEstimate of Pi: %f\n\n", (double) num_in_quarter_circle * 4 / (total));

    rc = pool_destroy(&pool);
    if (rc != Success) {
        fprintf(stderr, "The pool was not destroyed successfully.\n");
        return rc;
    }

    fflush(stdout);
}