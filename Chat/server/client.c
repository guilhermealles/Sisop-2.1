#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../interface.h"
#include "client.h"
#include "room.h"

unsigned int registeredClientsCount;
unsigned int clientsArraySize;
CLIENT *clientsArray;

pthread_mutex_t clientsMutex;

void initializeClientsManager() {
    registeredClientsCount = 0;
    clientsArray = malloc(sizeof(CLIENT) * CLIENTS_ARRAY_STEP_SIZE);
    clientsArraySize = CLIENTS_ARRAY_STEP_SIZE;

    pthread_mutex_init(&clientsMutex, NULL);
}

// Returns the ID of the new client, -1 if error
int registerNewClient (char *nick) {

    pthread_mutex_lock(&clientsMutex);
    unsigned int i;
    int id = -1;
    for (i=0; i<registeredClientsCount; i++) {
        if (strcmp(clientsArray[i].nick, nick) == 0) {
            fprintf(stderr, "[THREAD] Error: nick %s already in use!\n", nick);
            return -1;
        }

        // If an id has not yet been found and there is an empty entry in the array
        if ((id == -1) && (clientsArray[i].clientId == -1)) {
            // Assign ID to the new client
            id = i;
        }
    }

    //If it was not possible to find an empty entry in the middle of the array
    if (id == -1) {
        // Realloc array if needed
        if (registeredClientsCount >= (clientsArraySize-1)) {
            CLIENT *tmpArray = realloc(clientsArray, clientsArraySize+CLIENTS_ARRAY_STEP_SIZE);
            free(clientsArray);
            clientsArray = tmpArray;
        }
        id = registeredClientsCount;
        registeredClientsCount++;
    }
    // At this point a valid ID has already been found

    CLIENT *newClient = malloc(sizeof(CLIENT));
    newClient->clientId = id;
    strcpy(newClient->nick, nick);
    newClient->dataSocket = -1;

    clientsArray[newClient->clientId] = *newClient;
    free(newClient);
    pthread_mutex_unlock(&clientsMutex);

    return id;
}


int changeClientNick(int clientId, char *newNick) {
    pthread_mutex_lock(&clientsMutex);

    if (clientId >= registeredClientsCount) {
        fprintf(stderr, "[THREAD] Error: tried to change the nick of client %d. registeredClientsCount = %d.\n", clientId, registeredClientsCount);
        pthread_mutex_unlock(&clientsMutex);
        return -1;
    }
    if (strlen(newNick) >= MAX_NICK_LENGTH) {
        fprintf(stderr, "[THREAD] Error: tried to change the nick of client %d. New nickname is too long.\n", clientId);
        pthread_mutex_unlock(&clientsMutex);
        return -1;
    }
    strcpy(clientsArray[clientId].nick, newNick);

    pthread_mutex_unlock(&clientsMutex);
    return clientId;
}

// Returns the old room id, -1 if any errors occur
int changeClientRoom(int clientId, unsigned int newRoom) {
    pthread_mutex_lock(&clientsMutex);

    if ((clientId >= registeredClientsCount) || (existsRoomWithId(newRoom) == 0)) {
        fprintf(stderr, "[THREAD] Error: tried to change the room of client ID %d. registeredClientsCount = %d.\n", clientId, registeredClientsCount);
        pthread_mutex_unlock(&clientsMutex);
        return -1;
    }

    pthread_mutex_unlock(&clientsMutex);
    return clientId;
}

// Returns the client Id, -1 if any errors occur
int leaveRoom(int clientId) {
    pthread_mutex_lock(&clientsMutex);

    if (clientId >= registeredClientsCount) {
        fprintf(stderr, "[THREAD] Error: tried to remove client %d from a room. registeredClientsCount = %d.\n", clientId, registeredClientsCount);
        pthread_mutex_unlock(&clientsMutex);
        return -1;
    }
    clientsArray[clientId].chatRoom = -1;
	printf("[THREAD] Room of client ID %d - %s changed to %d.\n", clientId, clientsArray[clientId].nick, clientsArray[clientId].chatRoom);

    pthread_mutex_unlock(&clientsMutex);
    return clientId;
}

int bindDataSocket(int clientId, int socket) {
    pthread_mutex_lock(&clientsMutex);

    if (clientId >= registeredClientsCount) {
        fprintf(stderr, "[THREAD] Error: tried to bind socket to non existend client %d. registeredClientsCount = %d.\n", clientId, registeredClientsCount);
        pthread_mutex_unlock(&clientsMutex);
        return -1;
    }
    clientsArray[clientId].dataSocket = socket;

    pthread_mutex_unlock(&clientsMutex);
    return 0;
}

int disconnectClient(int clientId) {
    pthread_mutex_lock(&clientsMutex);

    if (clientId >= registeredClientsCount) {
        fprintf(stderr, "[THREAD] Error: tried to disconnect client %d. registeredClientsCount = %d.\n", clientId, registeredClientsCount);
        pthread_mutex_unlock(&clientsMutex);
        return -1;
    }
    // Close the data socket for the specified client
    close (clientsArray[clientId].dataSocket);
    // Clear client nickname, chat room and client ID
    memset(&(clientsArray[clientId].nick[0]), MAX_NICK_LENGTH, '\0');
    clientsArray[clientId].chatRoom = -1;
    clientsArray[clientId].clientId = -1;

    pthread_mutex_unlock(&clientsMutex);
    return 0;
}
