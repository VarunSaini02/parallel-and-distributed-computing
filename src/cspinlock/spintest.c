#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "spinlock.h"

#define VERBOSITY 0 // 0 (low), 1 (medium), 2 (high)
#define NUM_THREADS 20
#define ITERATIONS_PER_THREAD 100

static int counter = 0;

/***
 * @brief This function is a helper function for dealing with spinlock function failures.
 *
 * This function is a helper function for dealing with spinlock function failures. It prints
 * out which function caused the failure and returns an Error.
 *
 * @param func The function that caused the failure.
 *
 * @returns Error
 *
 ***/
static int spinlock_function_failure(const char func[]) {
    fprintf(stderr, "FAIL (%s): exiting program with return code %d\n", func, Error);
    return Error;
}

typedef struct thread_arg_st {
    int thread_num;
    int verbosity;
    pthread_t thread_id;
    spinlock_t* handle;
    rc_t result;
} thread_arg_t;

/***
 * @brief This function prints an update about the thread that called it.
 *
 * This function prints an update about the thread that called it, which includes
 * information about the static counter variable, the calling thread's number, and
 * the total number of threads. The exact information that is printed out is dependent
 * on the global variable VERBOSITY.
 *
 * @param i The iteration that the calling thread is currently in.
 * @param thread_arg The calling thread's thread_arg_t struct.
 *
 * @returns void
 *
 ***/
static void print_thread_update(int i, thread_arg_t* thread_arg) {
    if (thread_arg->verbosity == 2) {
        printf("Thread %d/%d:\tincremented counter to %d\n", thread_arg->thread_num, NUM_THREADS, counter);
    } else if (i == ITERATIONS_PER_THREAD - 1 && thread_arg->verbosity == 1) {
        printf("Thread %d/%d:\tfinished all iterations with counter at %d\n", thread_arg->thread_num, NUM_THREADS, counter);
    }
}

/***
 * @brief This routine increments the global counter variable ITERATIONS_PER_THREAD times.
 *
 * This routine increments the global counter variable ITERATIONS_PER_THREAD times. It is
 * designed to be compatible with pthread_create, and it assumes the given argument
 * arg can be casted to a thread_arg_t pointer (thread_arg_t*). This type contains a pointer
 * to a spinlock, which is used to ensure thread safety while incrementing the counter. The 
 * thread_arg_t struct also has a result field, which is used to pass back a return code (rc_t).
 *
 * @param arg The argument that is casted to a thread_arg_t pointer (thread_arg_t*).
 *
 * @returns NULL
 *
 ***/
static void* increment_counter_routine(void* arg) {
    if (arg == NULL) {
        invalid_null_argument_error("increment_counter_routine", "arg");
        return NULL;
    }

    thread_arg_t* thread_arg = arg;
    if (thread_arg->handle == NULL) {
        invalid_null_argument_error("increment_counter_routine", "((thread_arg_t*) arg)->handle");
        return NULL;
    }

    spinlock_t* handle = thread_arg->handle;
    for (int i = 0; i < ITERATIONS_PER_THREAD; i++) {
        rc_t rc;
        rc = spinlock_acquire(handle);
        if (rc != Success) {
            return NULL;
        }
        counter++;
        print_thread_update(i, thread_arg);
        rc = spinlock_release(handle);
        if (rc != Success) {
            return NULL;
        }
    }
    
    thread_arg->result = Success;
    return NULL;
}

/***
 * @brief This function tests the spinlock implementation using multiple threads.
 *
 * This function tests the spinlock implementation defined in "spinlock.h" using multiple threads.
 * First, spinlock_attr_init and spinlock_create are called to initialize the spinlock. Then, 
 * NUM_THREADS threads are created using pthread_create and joined using pthread_join. Each
 * thread runs increment_counter_routine using the same spinlock. Upon completion, the spinlock
 * is destroyed using spinlock_destroy, and the value of the counter is verified to be equal to
 * NUM_THREADS * ITERATIONS_PER_THREAD.
 *
 * @param argc The number of command line arguments provided.
 * @param argv An array containing the given command line arguments.
 *
 * @returns Success on success and Error on failure.
 *
 ***/
int main(int argc, char *argv[]) {
    spinlock_t handle;
    rc_t rc;

    // Create spinlock
    rc = spinlock_create(&handle, NULL);
    if (rc != Success) {
        return spinlock_function_failure("spinlock_create");
    }

    // Create and start the threads, keeping track of each thread's respective thread_args
    thread_arg_t thread_args[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = (thread_arg_t){.thread_num = i + 1, .verbosity = VERBOSITY, .handle = &handle, .result = Error};
        pthread_create(&(thread_args[i].thread_id), NULL, increment_counter_routine, &thread_args[i]);
    }

    // Join each thread and check for a successful return code
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_args[i].thread_id, NULL);
        rc = thread_args[i].result;
        if (rc != Success) {
            fprintf(stderr, "FAIL (thread %d / %d): exiting program with return code %d\n", i+1, NUM_THREADS, rc);
            return rc;
        }
    }

    // Destroy spinlock
    rc = spinlock_destroy(&handle);
    if (rc != Success) {
        return spinlock_function_failure("spinlock_destroy");
    }

    int expected_total = NUM_THREADS * ITERATIONS_PER_THREAD;
    if (counter != expected_total) {
        fprintf(stderr, "FAIL: Counter is equal to %d, not %d.\n", counter, expected_total);
        return Error;
    }
    printf("PASS: Counter is equal to %d (%d threads x %d iterations per thread).\n", expected_total, NUM_THREADS, ITERATIONS_PER_THREAD);

    return Success;
}
