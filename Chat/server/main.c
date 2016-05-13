#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include <pthread.h>
#include "../interface.h"

#define MAX_SIMULT_CONN 20
#define READ_BUFFER_SIZE 1024

void* connection_thread(void *args);

int main (int argc, char **argv) {
	int connectionSocket;
	struct sockaddr_in servAddr;

	printf("Initializing server...\n");
	printf("Creating connection socket...\n");
	// Try to open connection socket
	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectionSocket == -1) {
		fprintf(stderr, "Error creating connection socket.\n");
		exit(EXIT_FAILURE);
	}
	printf("Connection socket created.\n");

	// Fill the servAddr variable
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	memset(&(servAddr.sin_zero), 0, 8);

	printf("Binding connection socket...\n");
	// Bind the connection socket
	if (bind(connectionSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		fprintf(stderr, "Error when binding connection socket.\n");
		exit(EXIT_FAILURE);
	}
	printf("Connection socket bound.\n");

	listen(connectionSocket, MAX_SIMULT_CONN);
	printf("Connection socket listening...\n");

	while (1) {
		// Accept a connection
		socklen_t clientLength = sizeof(struct sockaddr_in);
		struct sockaddr_in clientAddr;
		int messageSocket;
		if ((messageSocket = accept(connectionSocket, (struct sockaddr *) &clientAddr, &clientLength)) == -1) {
			fprintf(stderr, "Error when accepting message.\n");
			exit(EXIT_FAILURE);
		}
		printf("Connection accepted. Creating thread to handle message on socket %d...\n", messageSocket);

		// Malloc a new int,
		// Save connectionSocket to this variable,
		// Start the thread passing messageSocket as a parameter */
		int *thread_args = (int*)malloc(sizeof(int));
		*thread_args = messageSocket;
		// Memory leaking?
		pthread_t thread;
		if (pthread_create(&thread, NULL, (void *)connection_thread, (void *)thread_args) != 0) {
			fprintf(stderr, "Error when creating a connection thread.\n");
			exit(EXIT_FAILURE);
		}
		printf("Thread created. Waiting for next connection...\n");
	}

	return 0;
}

void* connection_thread(void* args) {
	int *messageSocket_p = (int*) args;
	int messageSocket = *messageSocket_p;
	char buffer[READ_BUFFER_SIZE];

	// Fill buffer with zeros
	memset(buffer, 0, sizeof(char)*READ_BUFFER_SIZE);

	// Iterative reading until packet is completely read
	int bytesRead=0, bytesToRead=4;
	while((bytesRead < bytesToRead) && (bytesRead < READ_BUFFER_SIZE)) {
		printf("[THREAD] Will try to read message from socket %d.\n", messageSocket);
		bytesRead += read(messageSocket, &buffer[bytesRead], bytesToRead);
		printf("Read successful. Bytes read so far: %d, bytes to read: %d.\n", bytesRead, bytesToRead);

		if (bytesRead == 4) {
			unsigned int *packetSizePtr = (unsigned int*) buffer;
			bytesToRead = *packetSizePtr;
			printf("[THREAD] Buffer bytes:\n");
			int i;
			for(i=0; i<4; i++) {
				printf("[%d]:\t\t%d\n", i, (unsigned int)buffer[i]);
			}
			printf("\nPacket size: %d.\n", bytesToRead);
		}
	}

	// At this point the buffer should contain a full messageSocket

	printf("Successfuly read %d bytes from socket with ID #%d. Message is:\n", bytesRead, messageSocket);
	/* TMP */
	int i;
	for (i=0; i<bytesRead; i++) {
		printf("%c", buffer[i]);
	}
	printf("\n");

	// Close the socket and free allocated memory
	close(messageSocket);
	free(messageSocket_p);

	return 0;
}
