#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"

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

// Returns the ID of the new client
int registerNewClient(char *nick) {
    pthread_mutex_lock(&clientsMutex);

    if (registeredClientsCount >= (clientsArraySize-1)) {
        CLIENT *tmpArray = realloc(clientsArray, clientsArraySize+CLIENTS_ARRAY_STEP_SIZE);
        free(clientsArray);
        clientsArray = tmpArray;
    }
    CLIENT *new_client = malloc(sizeof(CLIENT));
    new_client->id = registeredClientsCount;
    new_client->nick = malloc(sizeof(char) + (strlen(nick)+1));
    strcpy(new_client->nick, nick);

    clientsArray[registeredClientsCount] = *new_client;
    free(new_client);
    int returnValue = registeredClientsCount;
    registeredClientsCount++;

    pthread_mutex_unlock(&clientsMutex);
    return returnValue;
}

// Returns the client Id, -1 if any errors occur
int changeClientRoom(int clientId, unsigned int newRom) {
    pthread_mutex_lock(&clientsMutex);

    if (clientId >= registeredClientsCount) {
        fprintf("[THREAD] Error: tried to change the room of client ID %d. registeredClientsCount = %d.\n", clientId, registeredClientsCount);
        return -1;
    }
    // TODO Add consistency check for room ID
    clientsArray[clientId].chatRoom = newRoom;

    pthread_mutex_unlock(&clientsMutex);
    return clientId;
}

// Returns the client Id, -1 if any errors occur
int leaveRoom(int clientId) {
    pthread_mutex_lock(&clientsMutex);

    if (clientId >= registeredClientsCount) {
        fprintf(stderr, "[THREAD] Error: tried to remove client %d from a room. registeredClientsCount = %d.\n", clientId, registeredClientsCount);
        return -1;
    }
    clientsArray[clientId].chatRoom = -1;

    pthread_mutex_unlock(&clientsMutex);
    return clientId;
}

// TODO Add functions to remove a client from the list, destroy clients, etc
