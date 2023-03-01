#include "queue.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    queue_t queue;
    queue_attr_t attrs;

    int x = 5;

    int rc = queue_attr_init(&attrs);
    if (rc != QueueSuccess) {
        printf("attr init failed\n");
        return -1;
    }

    rc = queue_create(&queue, &attrs);
    if (rc != QueueSuccess) {
        printf("queue create failed\n");
        return -1;
    }

    rc = queue_enqueue(&queue, &x);
    if (rc != QueueSuccess) {
        printf("queue enqueue (5) failed\n");
        return -1;
    }

    x = 7;
    rc = queue_enqueue(&queue, &x);
    if (rc != QueueSuccess) {
        printf("queue enqueue (7) failed\n");
        return -1;
    }

    int* size;
    rc = queue_size(&queue, &size);
    if (rc != QueueSuccess) {
        printf("queue size failed\n");
        return -1;
    }

    int* item;
    while (size > 0) {
        rc = queue_dequeue(&queue, &item);
        if (rc != QueueSuccess) {
            printf("queue dequeue failed\n");
            return -1;
        }
        printf("item is %d\n", *item);

        rc = queue_size(&queue, &size);
        if (rc != QueueSuccess) {
            printf("queue size failed\n");
            return -1;
        }
    }
}