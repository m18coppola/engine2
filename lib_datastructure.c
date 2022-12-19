#include <stdlib.h>
#include "lib_datastructure.h"

struct lib_cqueue *
lib_cqueue_init(int capacity, size_t element_size)
{
    struct lib_cqueue *new_queue = NULL;
    new_queue = malloc(sizeof(*new_queue));

    new_queue->capacity = capacity;
    new_queue->size = 0;
    new_queue->head = 0;
    new_queue->tail = capacity - 1;
    new_queue->array = malloc(element_size * capacity);
    new_queue->element_size = element_size;

    return new_queue;
}


void
lib_cqueue_free(struct lib_cqueue **cq_p)
{
    struct lib_cqueue *queue = *cq_p;
    free(queue->array);
    free(queue);
    *cq_p = NULL;
}


int
lib_cqueue_is_empty(struct lib_cqueue *queue)
{
    return queue->size == 0;
}


int
lib_cqueue_is_full(struct lib_cqueue *queue)
{
    return queue->size == queue->capacity;
}


void *
lib_cqueue_enqueue(struct lib_cqueue *queue)
{
    if (lib_cqueue_is_full(queue)) {
        return NULL;
    }
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;
    return lib_cqueue_peek_head(queue);
}

void *
lib_cqueue_peek_head(struct lib_cqueue *queue)
{
    if (lib_cqueue_is_empty(queue)) {
        return NULL;
    }
    return queue->array + (queue->element_size * queue->head);
}


void *
lib_cqueue_peek_tail(struct lib_cqueue *queue)
{
    if (lib_cqueue_is_empty(queue)) {
        return NULL;
    }
    return queue->array + (queue->element_size * queue->tail);
}


void *
lib_cqueue_dequeue(struct lib_cqueue *queue)
{
    if (lib_cqueue_is_empty(queue)) {
        return NULL;
    }
    void *addr = queue->array + (queue->element_size * queue->head);
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;
    return addr;
}
