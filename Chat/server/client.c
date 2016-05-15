#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
int registerNewClient(char *nick, struct sockaddr clientAddress) {
    pthread_mutex_lock(&clientsMutex);

    if (registeredClientsCount >= (clientsArraySize-1)) {
        CLIENT *tmpArray = realloc(clientsArray, clientsArraySize+CLIENTS_ARRAY_STEP_SIZE);
        free(clientsArray);
        clientsArray = tmpArray;
    }

    // Check if nickname not used
    int i=0;
    for(i=0; i<registeredClientsCount; i++) {
        if (strcmp(clientsArray[i].nick, nick) == 0) {
            fprintf(stderr, "[THREAD] Error: nick %s already in use!\n", nick);
            pthread_mutex_unlock(&clientsMutex);
            return -1;
        }
    }
    CLIENT *newClient = malloc(sizeof(CLIENT));
    newClient->clientId = registeredClientsCount;
    strcpy(newClient->nick, nick);
    struct sockaddr_in *casted = (struct sockaddr_in*) &clientAddress;
    casted->sin_port = htons(PORTA_CLI);
    newClient->clientAddress = clientAddress;

    clientsArray[registeredClientsCount] = *newClient;
    free(newClient);
    int returnValue = registeredClientsCount;
    registeredClientsCount++;

    pthread_mutex_unlock(&clientsMutex);
    return returnValue;
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

    pthread_mutex_unlock(&clientsMutex);
    return clientId;
}

// TODO Add functions to remove a client from the list, destroy clients, etc
