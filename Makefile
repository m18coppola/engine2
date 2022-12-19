include config.mk

SRC = main.c lib_datastructure.c
OBJ = ${SRC:.c=.o}

all: options main


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
