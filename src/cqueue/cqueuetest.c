#include <stdio.h>
#include <stdlib.h>
#include "cqueue.h"
#include <pthread.h>
#include <unistd.h>

#define QUEUE_CONFLICT_WAIT_USECS 10
#define NUM_THREADS 8
#define NUM_ITEMS_PER_THREAD 65536
#define BLOCK_SIZE 8
#define NUM_BLOCKS 16

typedef struct thread_arg_st {
   pthread_t thread_id;
   cqueue_t* queue;
   rc_t result;
} thread_arg_t;

static rc_t check_thread_routine_arg(void* arg) {
   if (arg == NULL) {
      fprintf(stderr, "Thread arg cannot be null\n");
      return Error;
   }

   thread_arg_t* thread_arg = arg;
   if (thread_arg->queue == NULL) {
      fprintf(stderr, "Thread arg's queue parameter cannot be null\n");
      return Error;
   }

   return Success;
}

static void* enqueue_thread_routine(void* arg) {
   if (check_thread_routine_arg(arg) != Success) {
      return NULL;
   }
   thread_arg_t* thread_arg = arg;

   for (int k = 0; k < NUM_ITEMS_PER_THREAD; k++) {
      rc_t rc = cqueue_enqueue(thread_arg->queue, &k, sizeof(int));
      while (rc == QueueFull) {
         usleep(QUEUE_CONFLICT_WAIT_USECS);
         rc = cqueue_enqueue(thread_arg->queue, &k, sizeof(int));
      } 
      if (rc != Success) {
         fprintf(stderr, "Error calling enqueue\n");
         return NULL;
      }
   }

   thread_arg->result = Success;
   return NULL;
}

static void* dequeue_thread_routine(void* arg) {
   if (check_thread_routine_arg(arg) != Success) {
      return NULL;
   }

   thread_arg_t* thread_arg = arg;
   int* val;
   size_t size;
   
   for (int k = 0; k < NUM_ITEMS_PER_THREAD; k++) {
      rc_t rc = cqueue_dequeue(thread_arg->queue, sizeof(int), (void**)&val, &size);
      while (rc == QueueEmpty) {
         usleep(QUEUE_CONFLICT_WAIT_USECS);
         rc = cqueue_dequeue(thread_arg->queue, sizeof(int), (void**)&val, &size);
      }
      if (rc != Success) {
         fprintf(stderr, "Error calling dequeue\n");
         return NULL;
      }
      free(val);
   }

   thread_arg->result = Success;
   return NULL;
}

int main(int argc, char* argv[]) {
   rc_t rc;
   cqueue_t queue;
   cqueue_attr_t attrs;
   size_t size;

   rc = cqueue_attr_init(&attrs);
   if (rc != Success) {
      fprintf(stderr, "Error calling attr init.\n");
      return rc;
   }

   attrs.block_size = BLOCK_SIZE;
   attrs.num_blocks = NUM_BLOCKS;

   rc = cqueue_create(&queue, &attrs);
   if (rc != Success) {
      fprintf(stderr, "Error calling cqueue create.\n");
      return rc;
   }

   // Create and start threads (both enqueue and dequeue threads)
   thread_arg_t enqueue_thread_args[NUM_THREADS];
   thread_arg_t dequeue_thread_args[NUM_THREADS];
   for (int i = 0; i < NUM_THREADS; i++) {
      enqueue_thread_args[i] = (thread_arg_t){.queue = &queue, .result = Error};
      dequeue_thread_args[i] = (thread_arg_t){.queue = &queue, .result = Error};
      pthread_create(&(enqueue_thread_args[i].thread_id), NULL, enqueue_thread_routine, &enqueue_thread_args[i]);
      pthread_create(&(dequeue_thread_args[i].thread_id), NULL, dequeue_thread_routine, &dequeue_thread_args[i]);
   }
   // Join all threads (both enqueue and dequeue threads)
   for (int i = 0; i < NUM_THREADS; i++) {
      pthread_join(enqueue_thread_args[i].thread_id, NULL);
      rc = enqueue_thread_args[i].result;
      if (rc != Success) {
         fprintf(stderr, "FAIL (thread %d / %d): exiting program with return code %d\n", i+1, NUM_THREADS, rc);
         return rc;
      }
      pthread_join(dequeue_thread_args[i].thread_id, NULL);
      rc = dequeue_thread_args[i].result;
      if (rc != Success) {
         fprintf(stderr, "FAIL (thread %d / %d): exiting program with return code %d\n", i+1, NUM_THREADS, rc);
         return rc;
      }
   }

   rc = cqueue_size(&queue, &size);
   if (rc != Success) {
      fprintf(stderr, "Error calling cqueue size.\n");
      return rc;
   }

   if (size != 0) {
      fprintf(stderr, "Error on getting size. Expected 0 but got %lu.\n", size);
      return Error;
   }

   int* val;
   rc = cqueue_dequeue(&queue, sizeof(int), (void**)&val, &size);
   if (rc != QueueEmpty) {
      fprintf(stderr, "Error calling dequeue on empty queue.\n");
      return Error;
   }

   rc = cqueue_destroy(&queue);
   if (rc != Success) {
      fprintf(stderr, "Error calling cqueue destroy.\n");
      return rc;
   }

   printf("Tests Completed Successfully\n");
   return Success;
}
