#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include <pthread.h>
#include "../interface.h"
#include "client.h"
#include "room.h"
#include "messagehandler.h"

#define MAX_SIMULT_CONN 20
#define READ_BUFFER_SIZE 1024

void* connection_thread(void *args);

int main (int argc, char **argv) {
	int connectionSocket;
	struct sockaddr_in servAddr;

	printf("Initializing server...\n");
	initializeClientsManager();
	initializeRoomsManager();
	initializeMessageHandler();
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
		struct sockaddr clientAddr;
		int messageSocket;
		if ((messageSocket = accept(connectionSocket, (struct sockaddr *) &clientAddr, &clientLength)) == -1) {
			fprintf(stderr, "Error when accepting message.\n");
			exit(EXIT_FAILURE);
		}
		printf("Connection accepted. Creating thread to handle message on socket %d...\n", messageSocket);

		// Malloc a void* variable,
		// First 4 bytes contains an int with the messageSocket descriptor
		// Remaining bytes are the cliAddr structure
		void *thread_args = (void*) malloc(sizeof(int) + sizeof(struct sockaddr));
		int *firstArg = &thread_args[0];
		struct sockaddr *secondArg = &thread_args[sizeof(int)];
		*firstArg = messageSocket;
		memcpy(secondArg, (void*) &clientAddr, clientLength);

		pthread_t thread;
		if (pthread_create(&thread, NULL, (void *)connection_thread, thread_args) != 0) {
			fprintf(stderr, "Error when creating a connection thread.\n");
			exit(EXIT_FAILURE);
		}
		printf("Thread created. Waiting for next connection...\n");
	}

	return 0;
}

void* connection_thread(void* args) {
	// Unwrap arguments
	int *messageSocket_p = (int*) &args[0];
	int messageSocket = *messageSocket_p;
	struct sockaddr *clientAddr_p = (struct sockaddr *) &args[sizeof(int)];
	struct sockaddr clientAddr = *clientAddr_p;
	printf("[THREAD] Will try to read message from socket %d.\n", messageSocket);

	char buffer[READ_BUFFER_SIZE];

	// Fill buffer with zeros
	memset(buffer, 0, sizeof(char)*READ_BUFFER_SIZE);
	int bytesRead = 0;
	while (bytesRead < 1) {
		// Try to read at least first byte (containing the tag)
		int currentBytesRead = read(messageSocket, &buffer[bytesRead], READ_BUFFER_SIZE);
		bytesRead += currentBytesRead;
	}
	int bytesToRead=0;
	switch(buffer[0]) {
		case CLIENT_REGISTER:
			bytesToRead = sizeof(REQUEST_REGISTER) - bytesRead;
			printf("[THREAD] Recognized tag %c, size of struct = %lu, bytes read = %d\n", buffer[0], sizeof(REQUEST_REGISTER), bytesRead);
			break;
		case SET_NICK:
			bytesToRead = sizeof(NICK_MESSAGE) - bytesRead;
			printf("[THREAD] Recognized tag %c, size of struct = %lu, bytes read = %d\n", buffer[0], sizeof(NICK_MESSAGE), bytesRead);
			break;
		case JOIN_ROOM:
			bytesToRead = sizeof(JOIN_MESSAGE) - bytesRead;
			printf("[THREAD] Recognized tag %c, size of struct = %lu, bytes read = %d\n", buffer[0], sizeof(JOIN_MESSAGE), bytesRead);
			break;
		case LEAVE_ROOM:
			bytesToRead = sizeof(LEAVE_MESSAGE) - bytesRead;
			printf("[THREAD] Recognized tag %c, size of struct = %lu, bytes read = %d\n", buffer[0], sizeof(LEAVE_MESSAGE), bytesRead);
			break;
		case LIST_ROOMS:
			bytesToRead = sizeof(REQUEST_ROOM_MESSAGE) - bytesRead;
			printf("[THREAD] Recognized tag %c, size of struct = %lu, bytes read = %d\n", buffer[0], sizeof(REQUEST_ROOM_MESSAGE), bytesRead);
			break;
		case MESSAGE_TO_ROOM:
			bytesToRead = sizeof(MESSAGE) - bytesRead;
			printf("[THREAD] Recognized tag %c, size of struct = %lu, bytes read = %d\n", buffer[0], sizeof(MESSAGE), bytesRead);
			break;
		 default:
		 	fprintf(stderr, "[THREAD] Error: unrecognized tag \"%c\".\n", buffer[0]);
			close(messageSocket);
			exit(EXIT_FAILURE);
			break;
	}
	if (bytesToRead < 0) {
		fprintf(stderr, "[THREAD] Warning: bytesToRead is less than 0 (value %d)!\n", bytesToRead);
		//close(messageSocket);
		//exit(EXIT_FAILURE);
	}
	// Read the remaining part of the packet (if any)
	while (bytesToRead != 0) {
		int currentBytesRead = read(messageSocket, &buffer[bytesRead], bytesToRead);
		bytesRead += currentBytesRead;
		bytesToRead -= currentBytesRead;
	}

	int serverResponse = SERV_REPLY_FAIL;
	// Switch the packet to the correct message, and treat accordingly
	switch(buffer[0]) {
		case CLIENT_REGISTER:
			serverResponse = handleRegisterClient(buffer);
			break;
		case SET_NICK:
			serverResponse = handleChangeNick(buffer);
			break;
		case JOIN_ROOM:
			serverResponse = handleChangeRoom(buffer);
			break;
		case LEAVE_ROOM:
			serverResponse = handleLeaveRoom(buffer);
			break;
		case LIST_ROOMS:
			//handle
			break;
		case MESSAGE_TO_ROOM:
			//handle
			break;
		 default:
		 	fprintf(stderr, "[THREAD] Error: unrecognized tag \"%c\".\n", buffer[0]);
			exit(EXIT_FAILURE);
			break;
	}

	// Send response to client
	SERVER_RESPONSE *response = malloc(sizeof(SERVER_RESPONSE));
	response->tag = SERVER_REPLY;
	response->response = serverResponse;
	int confirm = write(messageSocket, response, sizeof(SERVER_RESPONSE));
	if (confirm < 0) {
		fprintf(stderr, "[THREAD] Error sending reply to client");
		exit(EXIT_FAILURE);
	}

	// Close the socket and free allocated memory
	close(messageSocket);
	//free(messageSocket_p);
	//free(clientAddr_p);
	//free(response);

	return 0;
}
