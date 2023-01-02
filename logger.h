enum LogType {
    LOG_EVENT = 0,
    LOG_MESG,
    LOG_ERR,
    LOG
};

int init_logger(void);
void log_append(enum LogType type, char *mesg);
