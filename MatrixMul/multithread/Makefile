CC=gcc
CFLAGS=-Wall

INCLUDES=matrix.h
OBJECTS=main.o matrix.o

all: ${OBJECTS} ${INCLUDES}
	${CC} -o matrixmul ${OBJECTS} -lpthread

clean:
	rm -f *~
	rm -f ${OBJECTS}
