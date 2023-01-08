include config.mk

SRC = main.c logger.c event.c window.c cvar.c game.c input.c devcon.c
OBJ = ${SRC:.c=.o}

all: options main

debug: CFLAGS += -DDEBUG_KB -DDEBUG_MOUSE
debug: main

options:
	@echo build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

main: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f main ${OBJ}

run: main
	./main

.PHONY: all options clean run
