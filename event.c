#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "event.h"
#include "logger.h"
#include "cvar.h"
#include "input.h"

#define MAX_EVENTS 1024

struct Event {
    int time;
    enum EventType type;
    int val1, val2;
    int data_size;
    void *data;
};

static int head = -1;
static int tail = -1;
static struct Event queue[MAX_EVENTS];


int
eventqueue_is_empty(void)
{
    return head == -1;
}

int
eventqueue_is_full(void)
{
    return (head == tail + 1) || (head == 0 && tail == MAX_EVENTS - 1);
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
    tail = (tail + 1) % MAX_EVENTS;
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
        head = (head + 1) % MAX_EVENTS;
    }
    return ev;
}



void
process_events(void)
{
    int deffered_exit = 0;
    char msg[512];
    while(!eventqueue_is_empty()) {
        struct Event *ev;
        ev = eventqueue_poll();
        switch (ev->type) {
            case CONSOLE:
                parse_command((char *)ev->data);
                free(ev->data);
                break;
            case KEY:
                handleKey(ev->val1, ev->val2);
                break;
            case MOUSE:
                break;
            default:
                sprintf(msg
                        ,"bad event type: %d"
                        ,ev->type);
                log_append(LOG_ERR, msg);
                break;
        }
    }
    if (deffered_exit) exit(0);
}
