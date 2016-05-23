all: client.c
	gcc -o client client.c `pkg-config --libs --cflags gtk+-2.0` -lpthread

clean:
	rm -rf *~
	rm -rf client
