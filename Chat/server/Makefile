CC=gcc
CFLAGS=-Wall

INCLUDES=
OBJECTS=main.o
BINARY=server

all: ${OBJECTS} ${INCLUDES}
	${CC} ${CFLAGS} -o ${BINARY} ${OBJECTS}

clean:
	rm -f ${BINARY}
	rm -f *~
	rm -f ${OBJECTS}
