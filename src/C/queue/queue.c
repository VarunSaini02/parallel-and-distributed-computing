#include "queue.h"
#include <stdlib.h>

int queue_attr_init(queue_attr_t* attrs) {
    attrs->placeholder = 0;
    return 0;
}

int queue_create(queue_t* queue, queue_attr_t* attrs) {
    // do something with attrs
    queue->head = malloc(sizeof(queue_node_t));

    if (queue->head == NULL) {
        return QueueOutOfMemory;
    }

    queue->tail = queue->head;
    queue->size = 0;

    return QueueSuccess;
}

int queue_destroy(queue_t* queue) {
    queue_node_t* cursor = queue->head;
    while (cursor != NULL) {
        queue_node_t* next = cursor->next;
        free(cursor);
        cursor = next;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    return QueueSuccess;
}

int queue_enqueue(queue_t* queue, void* item) {
    queue_node_t* new_node = malloc(sizeof(queue_node_t));

    if (new_node == NULL) {
        return QueueOutOfMemory;
    }

    new_node->data = item;
    
    queue->tail->next = new_node;
    queue->tail = new_node;
    queue->size++;
    
    return QueueSuccess;
}

int queue_dequeue(queue_t* queue, int** item) {
    if (queue == NULL || item == NULL) {
        return QueueError;
    }

    if (queue->size <= 0) {
        return QueueError;
    }
    
    queue_node_t* tmp = queue->head->next;
    *item = tmp->data;
    queue->head->next = tmp->next;
    free(tmp);

    return QueueSuccess;
}

int queue_size(queue_t* queue, int** size) {
    if (queue == NULL || size == NULL) {
        return QueueError;
    }

    *size = &(queue->size);

    return QueueSuccess;
}