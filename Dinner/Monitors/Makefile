CC=gcc
CFLAGS=-Wall

INCLUDES=monitor.h philosopher.h
OBJECTS=main.o monitor.o philosopher.o
BINARY=philosophers

all: ${OBJECTS} ${INCLUDES}
	${CC} -o ${BINARY} ${OBJECTS}
	rm -f ${OBJECTS}

clean:
	rm -f ${BINARY}
	rm -f *~
	rm -f ${OBJECTS}
