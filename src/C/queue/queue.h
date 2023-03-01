#ifndef queue_h
#define queue_h

enum QueueRC {
    QueueSuccess,
    QueueOutOfMemory,
    QueueError
};

typedef struct queue_attr_st {
    void* placeholder;
} queue_attr_t;

typedef struct queue_node_st {
    void* data;
    struct queue_node_st* next;
} queue_node_t;

typedef struct queue_st {
    queue_node_t* head;
    queue_node_t* tail;
    int size; 
} queue_t;

int queue_attr_init(queue_attr_t* attrs);
int queue_create(queue_t* queue, queue_attr_t* attrs);
int queue_destroy(queue_t* queue);
int queue_enqueue(queue_t* queue, void* item);
int queue_dequeue(queue_t* queue, int** item);
int queue_size(queue_t* queue, int** size);

#endif