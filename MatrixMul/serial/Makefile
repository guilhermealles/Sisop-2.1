CC=gcc
CFLAGS=-Wall

INCLUDES=matrix.h
OBJECTS=main.o matrix.o

all: ${OBJECTS} ${INCLUDES}
	${CC} -o matrixmul ${OBJECTS}

clean:
	rm -f matrixmul
	rm -f *~
	rm -f ${OBJECTS}
