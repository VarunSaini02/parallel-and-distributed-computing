#include "barrier.h"
#include <stdio.h>
#include <stdlib.h>

rc_t barrier_create(barrier_t* barrier, uint32_t threads_limit, barrier_action_fun_t action_fun, void* action_arg, void** action_result) {
    if (barrier == NULL) {
        fprintf(stderr, "barrier_create: barrier cannot be NULL");
        return InvalidArgument;
    }

    rc_t rc;
    barrier->threads_limit = threads_limit;

    cqueue_attr_t thread_queues_attrs;
    rc = cqueue_attr_init(&thread_queues_attrs);
    if (rc != Success) {
        fprintf(stderr, "barrier_create: error calling cqueue_attr_init for thread_queues_attrs\n");
        return rc;
    }

    thread_queues_attrs.block_size = sizeof(int);
    thread_queues_attrs.num_blocks = barrier->threads_limit;

    rc = cqueue_create(&barrier->threads_at_barrier_queue, &thread_queues_attrs);
    if (rc != Success) {
        fprintf(stderr, "barrier_create: error creating barrier->threads_at_barrier_queue\n");
        return rc;
    }

    rc = cqueue_create(&barrier->thread_blocking_queue, &thread_queues_attrs);
    if (rc != Success) {
        fprintf(stderr, "barrier_create: error creating barrier->threads_at_barrier_queue\n");
        return rc;
    }

    for (int i = 1; i <= barrier->threads_limit; i++) {
        rc = cqueue_enqueue(&barrier->threads_at_barrier_queue, &i, sizeof(int), NULL);
        if (rc != Success) {
            fprintf(stderr, "barrier_wait: error calling cqueue_enqueue");
            return rc;
        }
    }

    barrier->action_fun = action_fun;
    barrier->action_arg = action_arg;
    barrier->action_result = action_result;

    return Success;
}

rc_t barrier_wait(barrier_t* barrier) {
    if (barrier == NULL) {
        fprintf(stderr, "barrier_wait: barrier cannot be NULL");
        return InvalidArgument;
    }

    rc_t rc;
    uint32_t size;
    rc = cqueue_size(&barrier->threads_at_barrier_queue, &size);
    if (rc != Success) {
        fprintf(stderr, "barrier_wait: error calling cqueue_size");
        return rc;
    }
    if (size == 0) { // Barrier already broken, just return
        return Success;
    }

    // Get how many threads have encountered the barrier so far
    int* threads_at_barrier;
    rc = cqueue_dequeue(&barrier->threads_at_barrier_queue, sizeof(int), (void**)&threads_at_barrier, &size, NULL);
    if (rc != Success) {
        fprintf(stderr, "barrier_wait: error calling cqueue_dequeue");
        return rc;
    }

    // If threads_at_barrier < barrier->threads_limit, then we wait for more threads to arrive
    // Else (if threads_at_barrier == barrier->threads_limit), we can wake all waiting threads
    if (*threads_at_barrier < barrier->threads_limit) {
        // Make the current thread wait by attempting to dequeue an empty queue
        int* stop_waiting_status;
        rc = cqueue_dequeue(&barrier->thread_blocking_queue, sizeof(int), (void**)&stop_waiting_status, &size, NULL);
        if (rc != Success) {
            fprintf(stderr, "barrier_wait: error calling cqueue_dequeue");
            return rc;
        }
        free(stop_waiting_status);
    } else {
        // Call the barrier's action function
        rc = barrier->action_fun(barrier->action_arg, barrier->action_result);
        if (rc != Success) {
            fprintf(stderr, "barrier_wait: error calling barrier->action_fun");
            return rc;
        }

        // Wake all waiting threads by enqueueing one item per waiting thread
        // All waiting threads trying to dequeue from thread_blocking_queue will be able to proceed
        for (int i = 1; i < barrier->threads_limit; i++) {
            int stop_waiting_status = 0;
            rc = cqueue_enqueue(&barrier->thread_blocking_queue, &stop_waiting_status, sizeof(int), NULL);
            if (rc != Success) {
                fprintf(stderr, "barrier_wait: error calling cqueue_enqueue");
                return rc;
            }
        }
    }

    free(threads_at_barrier);
    return Success;
}

rc_t barrier_destroy(barrier_t* barrier) {
    if (barrier == NULL) {
        fprintf(stderr, "barrier_destroy: barrier cannot be NULL");
        return InvalidArgument;
    }

    rc_t rc;
    rc = cqueue_destroy(&barrier->threads_at_barrier_queue);
    if (rc != Success) {
        fprintf(stderr, "barrier_destroy: error calling cqueue_destroy");
        return rc;
    }
    rc = cqueue_destroy(&barrier->thread_blocking_queue);
    if (rc != Success) {
        fprintf(stderr, "barrier_destroy: error calling cqueue_destroy");
        return rc;
    }

    return Success;
}