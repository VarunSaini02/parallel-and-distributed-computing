#include "pool.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_QUEUE_NUM_BLOCKS 16

void* pool_thread_routine(void* arg) {
    rc_t* rc = malloc(sizeof(rc_t));
    if (rc == NULL) {
        fprintf(stderr, "pool_thread_routine: error allocating memory for rc\n");
        return NULL;
    }
    *rc = Success;
    if (arg == NULL) {
        fprintf(stderr, "pool_thread_routine: arg cannot be NULL\n");
        *rc = InvalidArgument;
        return rc;
    }
    
    thread_pool_t* pool = arg;
    pool_work_t* item;
    uint32_t size;
    
    *rc = cqueue_dequeue(&pool->work_queue, sizeof(pool_work_t), (void**)&item, &size, NULL);
    if (*rc != Success) {
        fprintf(stderr, "pool_thread_routine: error dequeuing from work_queue\n");
        return rc;
    }
    while (item->fun != NULL) {
        pool_result_t pool_result;
        pool_result.id = item->id;
        pool_result.rc = item->fun(item->arg, &pool_result.result);

        *rc = cqueue_enqueue(&pool->results_queue, &pool_result, sizeof(pool_result_t), NULL);
        if (*rc != Success) {
            fprintf(stderr, "pool_thread_routine: error enqueuing pool_result_t object\n");
            return rc;
        }
        free(item);

        *rc = cqueue_dequeue(&pool->work_queue, sizeof(pool_work_t), (void**)&item, &size, NULL);
        if (*rc != Success) {
            fprintf(stderr, "pool_thread_routine: error dequeuing from work_queue\n");
            return rc;
        }
    } 
    free(item);

    return rc;
}

rc_t pool_create(thread_pool_t* pool, int pool_size) {
    if (pool == NULL) {
        fprintf(stderr, "pool_create: pool cannot be NULL\n");
        return InvalidArgument;
    }

    rc_t rc = Success;
    cqueue_attr_t work_attrs;
    rc = cqueue_attr_init(&work_attrs);
    if (rc != Success) {
        fprintf(stderr, "pool_create: error calling cqueue_attr_init for work_attrs\n");
        return -1;
    }
    cqueue_attr_t results_attrs;
    rc = cqueue_attr_init(&results_attrs);
    if (rc != Success) {
        fprintf(stderr, "pool_create: error calling cqueue_attr_init for results_attrs\n");
        return -1;
    }

    work_attrs.block_size = sizeof(pool_work_t);
    work_attrs.num_blocks = DEFAULT_QUEUE_NUM_BLOCKS;
    results_attrs.block_size = sizeof(pool_result_t);
    results_attrs.num_blocks = DEFAULT_QUEUE_NUM_BLOCKS;

    rc = cqueue_create(&(pool->work_queue), &work_attrs);
    if (rc != Success) {
        fprintf(stderr, "pool_create: error creating pool->work_queue\n");
        return rc;
    }
    rc = cqueue_create(&(pool->results_queue), &results_attrs);
    if (rc != Success) {
        fprintf(stderr, "pool_create: error creating pool->results_queue\n");
        return rc;
    }

    pool->size = pool_size;
    pool->threads = calloc(pool->size, sizeof(pthread_t));
    if (pool->threads == NULL) {
        fprintf(stderr, "pool_create: error allocating memory for pool->threads\n");
        return OutOfMemory;
    }

    for (int i = 0; i < pool->size; i++) {
        int prc = pthread_create(&pool->threads[i], NULL, pool_thread_routine, pool);
        if (prc != 0) {
            fprintf(stderr, "pool_create: error creating threads\n");
            return Error;
        }
    }

    return rc;
}

rc_t pool_destroy(thread_pool_t* pool) {
    if (pool == NULL) {
        fprintf(stderr, "pool_create: pool cannot be NULL\n");
        return InvalidArgument;
    }

    rc_t rc = Success;
    for (int i = 0; i < pool->size; i++) {
        pool_work_t item = (pool_work_t){.id = -1, .arg = NULL, .fun = NULL};
        rc = cqueue_enqueue(&pool->work_queue, &item, sizeof(pool_work_t), NULL);
        if (rc != Success) {
            fprintf(stderr, "pool_destroy: error enqueuing terminating pool_work_t object\n");
            return rc;
        }
    }

    for (int i = 0; i < pool->size; i++) {
        rc_t* trc;
        int prc = pthread_join(pool->threads[i], (void**)&trc);
        
        if (prc != 0) {
            fprintf(stderr, "pool_destroy: error during pthread_join with code %d\n", prc);
            return Error;  
        }
        if (trc == NULL) {
            fprintf(stderr, "pool_destroy: thread return code was NULL\n");
            return Error;
        }
        if (*trc != Success) {
            fprintf(stderr, "pool_destroy: error in thread detected while joining\n");
            rc = *trc;
            free(trc);
            return rc;
        }

        free(trc);
    }

    free(pool->threads);

    rc = cqueue_destroy(&pool->work_queue);
    if (rc != Success) {
        fprintf(stderr, "pool_destroy: error destroying pool->work_queue\n");
    }
    rc = cqueue_destroy(&pool->results_queue);
    if (rc != Success) {
        fprintf(stderr, "pool_destroy: error destroying pool->results_queue\n");
    }

    return rc;
}

void* enqueue_work_routine(void* arg) {
    rc_t* rc = malloc(sizeof(rc_t));
    *rc = Success;
    if (rc == NULL) {
        fprintf(stderr, "enqueue_work_routine: error allocating memory for rc\n");
        return NULL;
    }
    if (arg == NULL) {
        fprintf(stderr, "enqueue_work_routine: arg cannot be NULL\n");
        *rc = InvalidArgument;
        return rc;
    }
    
    enq_map_arg_t* enq_arg = arg;
    thread_pool_t* pool = enq_arg->pool;
    pool_fun_t* fun = enq_arg->fun;
    int arg_count = enq_arg->arg_count;
    void** args = enq_arg->args;

    for (int i = 0; i < arg_count; i++) {

        pool_work_t item = (pool_work_t){.id = i, .arg = args[i], .fun = fun};
        *rc = cqueue_enqueue(&pool->work_queue, &item, sizeof(pool_work_t), NULL);
        if (*rc != Success) {
            fprintf(stderr, "enqueue_work_routine: error enqueuing pool_work_t object\n");
            return rc;
        }
    }

    return rc;
}

void* dequeue_results_routine(void* arg) {
    rc_t* rc = malloc(sizeof(rc_t));
    *rc = Success;
    if (rc == NULL) {
        fprintf(stderr, "dequeue_results_routine: error allocating memory for rc\n");
        return NULL;
    }
    if (arg == NULL) {
        fprintf(stderr, "dequeue_results_routine: arg cannot be NULL\n");
        *rc = InvalidArgument;
        return rc;
    }
    
    deq_map_arg_t* deq_arg = arg;
    thread_pool_t* pool = deq_arg->pool;
    int arg_count = deq_arg->arg_count;
    void** results = deq_arg->results;

    for (int i = 0; i < arg_count; i++) {
        pool_result_t* result_obj;
        uint32_t size;
        *rc = cqueue_dequeue(&pool->results_queue, sizeof(pool_result_t), (void**)&result_obj, &size, NULL);
        if (*rc != Success) {
            fprintf(stderr, "dequeue_results_routine: error dequeuing from results_queue\n");
            return rc;
        }
        if (result_obj->rc != Success) {
            fprintf(stderr, "dequeue_results_routine: error in calling given function\n");
            free(result_obj);
            *rc = result_obj->rc;
            return rc;
        }
        results[result_obj->id] = result_obj->result;
        free(result_obj);
    }

    return rc;
}

rc_t pool_map(thread_pool_t* pool, pool_fun_t fun, int arg_count, void* args[], void* results[]) {
    if (pool == NULL) {
        fprintf(stderr, "pool_create: pool cannot be NULL\n");
        return InvalidArgument;
    }
    if (args == NULL) {
        fprintf(stderr, "pool_create: args cannot be NULL\n");
        return InvalidArgument;
    }
    if (results == NULL) {
        fprintf(stderr, "pool_create: results cannot be NULL\n");
        return InvalidArgument;
    }

    rc_t rc = Success;
    rc_t* trc;
    int prc;

    pthread_t enq_thread;
    pthread_t deq_thread;
    enq_map_arg_t enq_arg = {.pool = pool, .fun = fun, .arg_count = arg_count, .args = args};
    deq_map_arg_t deq_arg = {.pool = pool, .arg_count = arg_count, .results = results};
    pthread_create(&enq_thread, NULL, enqueue_work_routine, &enq_arg);
    pthread_create(&deq_thread, NULL, dequeue_results_routine, &deq_arg);

    prc = pthread_join(enq_thread, (void**)&trc);
    if (prc != 0) {
        fprintf(stderr, "pool_map: error during pthread_join with code %d\n", prc);
        return Error;  
    }
    if (trc == NULL) {
        fprintf(stderr, "pool_map: thread return code was NULL\n");
        return Error;
    }
    if (*trc != Success) {
        fprintf(stderr, "pool_map: error in thread detected while joining\n");
        rc = *trc;
        free(trc);
        return rc;
    }

    free(trc);

    prc = pthread_join(deq_thread, (void**)&trc);
    if (prc != 0) {
        fprintf(stderr, "pool_map: error during pthread_join with code %d\n", prc);
        return Error;  
    }
    if (trc == NULL) {
        fprintf(stderr, "pool_map: thread return code was NULL\n");
        return Error;
    }
    if (*trc != Success) {
        fprintf(stderr, "pool_map: error in thread detected while joining\n");
        rc = *trc;
        free(trc);
        return rc;
    }

    free(trc);

    return rc;
}
