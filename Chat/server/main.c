#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in

int main (int argc, char **argv) {
	int connection_socket, message_socket;
	
	// Try to open connection socket
	connection_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (connection_socket == -1) {
		fpritnf(stderr, "Error creating connection socket.\n");
		exit(EXIT_FAILURE);
	}
	
	// Fill the struct sockaddr_in variable
}
