#ifndef cqueue_h
#define cqueue_h

#include "rc.h"
#include <stdint.h>
#include <stddef.h>
#include "spinlock.h"

typedef struct cqueue_attr_st {
   int block_size;
   int num_blocks;
} cqueue_attr_t;

typedef struct cqueue_obj_st {
   int head;
   int tail;
   int count;
   int num_blocks;
   int block_size;
   spinlock_obj_t lock_obj;
   uint64_t data[];
} cqueue_obj_t;

typedef struct cqueue_st {
   cqueue_obj_t* obj;
   spinlock_t lock;
} cqueue_t;

rc_t cqueue_attr_init(cqueue_attr_t* attrs);
rc_t cqueue_alloc_size(cqueue_attr_t* attrs, int* required_bytes);
rc_t cqueue_init(cqueue_t* queue, cqueue_obj_t* obj, cqueue_attr_t* attrs);
rc_t cqueue_create(cqueue_t* queue, cqueue_attr_t* attrs);
rc_t cqueue_destroy(cqueue_t* queue);
rc_t cqueue_enqueue(cqueue_t* queue, void* item, size_t size);
rc_t cqueue_dequeue(cqueue_t* handle, size_t max_bytes, void** item, size_t* size);
rc_t cqueue_size(cqueue_t* queue, size_t* size);

#endif
