#include "cqueue.h"
#include <pthread.h>

typedef rc_t pool_fun_t(void* arg, void** result);

typedef struct pool_work_st {
    int id;
    void* arg;
    pool_fun_t* fun;
} pool_work_t;

typedef struct pool_result_st {
    int id;
    rc_t rc;
    void* result;
} pool_result_t;

typedef struct thread_pool_st {
    cqueue_t work_queue;
    cqueue_t results_queue;
    int size;
    pthread_t* threads;
} thread_pool_t;

typedef struct enq_map_arg_st {
    thread_pool_t* pool;
    pool_fun_t* fun;
    int arg_count;
    void** args;
} enq_map_arg_t;

typedef struct deq_map_arg_st {
    thread_pool_t* pool;
    int arg_count;
    void** results;
} deq_map_arg_t;

rc_t pool_create(thread_pool_t* pool, int pool_size);
rc_t pool_destroy(thread_pool_t* pool);
rc_t pool_map(thread_pool_t* pool, pool_fun_t fun, int arg_count, void* args[], void* results[]);
