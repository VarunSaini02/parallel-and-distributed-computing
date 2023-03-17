#ifndef barrier_h
#define barrier_h

#include "cqueue.h"
#include <pthread.h>

typedef rc_t barrier_action_fun_t(void* arg, void** result);

typedef struct barrier_st {
    uint32_t threads_limit;
    cqueue_t threads_at_barrier_queue;
    cqueue_t thread_blocking_queue;
    barrier_action_fun_t* action_fun;
    void* action_arg;
    void** action_result;
} barrier_t;

rc_t barrier_create(barrier_t* barrier, uint32_t threads_limit, barrier_action_fun_t action_fun, void* action_arg, void** action_result);
rc_t barrier_wait(barrier_t* barrier);
rc_t barrier_destroy(barrier_t* barrier);
rc_t barrier_reset(barrier_t* barrier);

#endif