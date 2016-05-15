CC=gcc
CFLAGS=-Wall

INCLUDES=client.c messagehandler.c room.c
OBJECTS=main.o client.o messagehandler.o room.o
BINARY=server

all: ${OBJECTS} ${INCLUDES}
	${CC} ${CFLAGS} -o ${BINARY} ${OBJECTS} -lpthread

clean:
	rm -f ${BINARY}
	rm -f *~
	rm -f ${OBJECTS}
