enum EventType {
    NONE = 0, //timestamp is valid
    KEY, //val1 is keycode, val2 is 1 if down event, 0 if up event
    CHAR, // val1 is a char
    MOUSE, //val1,val2 is x,y respectively
    CONSOLE, //data is *char
    PACKET, //TBD
    DEBUG
};

int eventqueue_is_empty(void);
int eventqueue_is_full(void);
int eventqueue_post(int time, enum EventType type, int val1, int val2, int data_size, char *data);
struct Event *eventqueue_poll(void);
void buffer_events(void);
void process_events(void);
