#include "cqueue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_BLOCK_SIZE 128
#define DEFAULT_NUM_BLOCKS 32

typedef struct cqueue_item {
   size_t size;
   char data[];
} cqueue_item_t;

rc_t cqueue_attr_init(cqueue_attr_t* attrs) {
   if (attrs == NULL) {
      fprintf(stderr, "On cqueue_attr_init the attrs cannot be NULL\n");
      return InvalidArgument;
   }

   attrs->block_size = DEFAULT_BLOCK_SIZE;
   attrs->num_blocks = DEFAULT_NUM_BLOCKS;
   return Success;
}

rc_t cqueue_alloc_size(cqueue_attr_t* attrs, int* required_bytes) {

   if (attrs == NULL) {
      fprintf(stderr, "On cqueue_alloc_size the attrs cannot be NULL\n");
      return InvalidArgument;
   }

   if (required_bytes == NULL) {
      fprintf(stderr, "On cqueue_alloc_size the required_bytes pointer cannot be NULL\n");
      return InvalidArgument;
   }

   int sz = 0;
   sz += sizeof(cqueue_obj_t);
   sz += (attrs->block_size + sizeof(cqueue_item_t)) * attrs->num_blocks;

   *required_bytes = sz;

   return Success;
}

rc_t cqueue_init(cqueue_t* handle, cqueue_obj_t* obj, cqueue_attr_t* attrs) {
   cqueue_attr_t default_attrs;
   rc_t rc;

   if (handle == NULL) {
      fprintf(stderr, "The queue pointer cannot be NULL.\n");
      return InvalidArgument;
   }

   if (obj == NULL) {
      fprintf(stderr, "The obj pointer cannot be NULL.\n");
      return InvalidArgument;
   }

   if (attrs == NULL) {
      attrs = &default_attrs;
      rc = cqueue_attr_init(attrs);
      if (rc != Success) {
            fprintf(stderr, "Could not init default attrs\n");
            return rc;
      }
   }

   handle->obj = obj;
   obj->block_size = attrs->block_size;
   obj->num_blocks = attrs->num_blocks;
   obj->head = 0;
   obj->tail = 0;
   obj->count = 0;

   rc = spinlock_init(&(handle->lock), &(obj->lock_obj), NULL);
   if (rc != Success) {
      fprintf(stderr, "Could not initialize spinlock\n");
      return rc;
   }

   return Success;
}

rc_t cqueue_create(cqueue_t* handle, cqueue_attr_t* attrs) {
   cqueue_attr_t default_attrs;
   rc_t rc;

   if (handle == NULL) {
      fprintf(stderr, "The handle cannot be NULL.\n");
      return InvalidArgument;
   }

   if (attrs == NULL) {
      attrs = &default_attrs;
      rc = cqueue_attr_init(attrs);
      if (rc != Success) {
            fprintf(stderr, "Could not init default attrs\n");
            return rc;
      }
   }

   int sz;
   rc = cqueue_alloc_size(attrs, &sz);
   if (rc != Success) {
      fprintf(stderr, "Could not get the cqueue allocation size\n");
      return rc;
   }

   void* obj = malloc(sz);

   rc = cqueue_init(handle, obj, attrs);
   if (rc != Success) {
      fprintf(stderr, "Could not init the cqueue in create.\n");
      return rc;
   }

   return Success;
}

rc_t cqueue_destroy(cqueue_t* handle) {
   if (handle == NULL) {
      fprintf(stderr, "On destroy the handle cannot be NULL\n");
      return InvalidArgument;
   }

   if (handle->obj == NULL) {
      fprintf(stderr, "The handle was empty. Could not destroy it.\n");
      return InvalidArgument;
   }

   free(handle->obj);

   return Success;
}

rc_t cqueue_enqueue(cqueue_t* handle, void* item, size_t size) {
   rc_t rc;
   if (handle == NULL) {
      fprintf(stderr, "The handle cannot be NULL\n");
      return InvalidArgument;
   }

   if (item == NULL) {
      fprintf(stderr, "The item cannot be NULL\n");
      return InvalidArgument;
   }

   if (size == 0) {
      fprintf(stderr, "The byte_size cannot be zero\n");
      return InvalidArgument;
   }

   if (size > handle->obj->block_size) {
      fprintf(stderr, "The item cannot fit in the queue\n");
      return InvalidArgument;
   }

   rc = spinlock_acquire(&handle->lock);
   if (rc != Success) {
      fprintf(stderr, "Error in spinlock acquire\n");
      return rc;
   }
   if (handle->obj->count == handle->obj->num_blocks) {
      rc = spinlock_release(&handle->lock);
      if (rc != Success) {
         fprintf(stderr, "Error in spinlock release\n");
         return rc;
      }
      return QueueFull;
   }

   void* dest = &handle->obj->data;
   dest += handle->obj->head * (handle->obj->block_size + sizeof(cqueue_item_t));

   cqueue_item_t* item_ptr = dest;
   item_ptr->size = size;
   memcpy(&item_ptr->data, item, size);
   handle->obj->head = (handle->obj->head + 1) % handle->obj->num_blocks;
   handle->obj->count += 1;

   rc = spinlock_release(&handle->lock);
   if (rc != Success) {
      fprintf(stderr, "Error in spinlock release\n");
      return rc;
   }

   return Success;
}

rc_t cqueue_dequeue(cqueue_t* handle, size_t max_size, void** item, size_t* size) {
   rc_t rc;
   if (handle == NULL) {
      fprintf(stderr, "The handle cannot be NULL\n");
      return InvalidArgument;
   }

   if (item == NULL) {
      fprintf(stderr, "The item cannot be NULL\n");
      return InvalidArgument;
   }

   if (max_size == 0) {
      fprintf(stderr, "The max_bytes cannot be zero\n");
      return InvalidArgument;
   }

   if (size == NULL) {
      fprintf(stderr, "The item size cannot be NULL\n");
      return InvalidArgument;
   }

   rc = spinlock_acquire(&handle->lock);
   if (rc != Success) {
      fprintf(stderr, "Error in spinlock acquire\n");
      return rc;
   }
   if (handle->obj->count == 0) {
      rc = spinlock_release(&handle->lock);
      if (rc != Success) {
         fprintf(stderr, "Error in spinlock release\n");
         return rc;
      }
      return QueueEmpty;
   }

   void* src = &handle->obj->data;
   src += handle->obj->tail * (handle->obj->block_size + sizeof(cqueue_item_t));

   cqueue_item_t* item_ptr = src;
   if (item_ptr->size > max_size) {
      fprintf(stderr, "The item size cannot be NULL\n");
      rc = spinlock_release(&handle->lock);
      if (rc != Success) {
         fprintf(stderr, "Error in spinlock release\n");
         return rc;
      }
      return InvalidArgument;
   }

   void* data = malloc(item_ptr->size);
   if (data == NULL) {
      fprintf(stderr, "Out of Memory.\n");
      rc = spinlock_release(&handle->lock);
      if (rc != Success) {
         fprintf(stderr, "Error in spinlock release\n");
         return rc;
      }
      return OutOfMemory;
   }

   *size = item_ptr->size;
   memcpy(data, &item_ptr->data, *size);
   handle->obj->tail = (handle->obj->tail + 1) % handle->obj->num_blocks;
   handle->obj->count -= 1;
   *item = data;

   rc = spinlock_release(&handle->lock);
   if (rc != Success) {
      fprintf(stderr, "Error in spinlock release\n");
      return rc;
   }

   return Success;
}

rc_t cqueue_size(cqueue_t* handle, size_t* size) {
   if (handle == NULL) {
      fprintf(stderr, "The handle cannot be NULL\n");
      return InvalidArgument;
   }

   if (size == NULL) {
      fprintf(stderr, "The size cannot be NULL\n");
      return InvalidArgument;
   }

   *size = handle->obj->count;

   return Success;
}
