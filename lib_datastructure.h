#ifndef LIB_DATASTRUCTURE_H
#define LIB_DATASTRUCTURE_H

#include <unistd.h>

struct lib_cqueue {
    unsigned int *array;
    int head;
    int tail;
    int size;
    int capacity;
    size_t element_size;
};

struct lib_cqueue *lib_cqueue_init(int capacity, size_t element_size);
void lib_cqueue_free(struct lib_cqueue **cq_p);
int lib_cqueue_is_empty(struct lib_cqueue *queue);
int lib_cqueue_is_full(struct lib_cqueue *queue);
void *lib_cqueue_enqueue(struct lib_cqueue *queue);
void *lib_cqueue_peek_head(struct lib_cqueue *queue);
void *lib_cqueue_peek_tail(struct lib_cqueue *queue);
void *lib_cqueue_dequeue(struct lib_cqueue *queue);

#endif /* LIB_DATASTRUCTURE_H */
