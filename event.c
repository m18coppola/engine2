#include <stdlib.h>
#include "event.h"
#include "logger.h"

static int head;
static int tail;
static int capacity;
static struct Event *queue;

int
init_event_system(int event_buffer_length)
{
    queue = malloc(sizeof(struct Event) * event_buffer_length);
    head = -1;
    tail = -1;
    capacity = event_buffer_length;
    return 0;
}

void
close_event_system(void)
{
    free(queue);
    queue = NULL;
}

int
eventqueue_is_empty(void)
{
    return head == -1;
}

int
eventqueue_is_full(void)
{
    return (head == tail + 1) || (head == 0 && tail == capacity - 1);
}

int
eventqueue_post(int time, enum EventType type, int val1, int val2,
                int data_size, char *data)
{
    if (eventqueue_is_full()) {
        log_append(LOG_ERR, "event queue is full!");
        return -1;
    }
    if (head == -1) head = 0;
    tail = (tail + 1) % capacity;
    queue[tail].time = time;
    queue[tail].type = type;
    queue[tail].val1 = val1;
    queue[tail].val2 = val2;
    queue[tail].data_size = data_size;;
    queue[tail].data = data;
    return 0;
}

struct Event *
eventqueue_poll(void)
{
    struct Event *ev;

    if (eventqueue_is_empty()) {
        return NULL;
    }
    ev = queue+head;
    if (head == tail) {
        head = -1;
        tail = -1;
    } else {
        head = (head + 1) % capacity;
    }
    return ev;
}

void
buffer_events(void)
{
    ;
}
