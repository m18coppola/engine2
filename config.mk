# version
VERSION = 2.0

# paths
PREFIX = /usr/local

# libs
MATHLIBS = -lm
MATHINC = 

SDLLIBS = `sdl2-config --libs`
SDLINC = `sdl2-config --cflags`

EPOXYLIBS = -lepoxy

# includes and libs
LIBS = ${MATHLIBS} ${SDLLIBS} ${EPOXYLIBS}
INCS = ${SDLINC} 

# flags
CFLAGS = -Wall -g  ${INCS}
LDFLAGS = ${LIBS}

# compiler/linker
CC = gcc
