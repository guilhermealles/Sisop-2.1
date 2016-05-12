#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include "../interface.h"

#define MAX_SIMULT_CONN 20

int main (int argc, char **argv) {
	int connectionSocket, messageSocket;
	struct sockaddr_in servAddr;

	// Try to open connection socket
	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectionSocket == -1) {
		fprintf(stderr, "Error creating connection socket.\n");
		exit(EXIT_FAILURE);
	}

	// Fill the servAddr variable
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(servAddr.sin_zero), 8);

	// Bind the connection socket
	if (bind(connectionSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		fprintf(stderr, "Error when binding connection socket.\n");
		exit(EXIT_FAILURE);
	}

	listen(connectionSocket, MAX_SIMULT_CONN);

	// Accept a connection
}
