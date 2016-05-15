#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "room.h"
#include "../interface.h"

unsigned int registeredRoomsCount;
unsigned int roomsArraySize;
CHAT_ROOM *roomsArray;

pthread_mutex_t roomsMutex;

void initializeRoomsManager() {
    registeredRoomsCount = 0;
    roomsArray = malloc(sizeof(CHAT_ROOM) * ROOMS_ARRAY_STEP_SIZE);
    roomsArraySize = ROOMS_ARRAY_STEP_SIZE;

    pthread_mutex_init(&roomsMutex, NULL);
}

// Returns the ID of the new room
int createChatRoom (char *name) {
    pthread_mutex_lock(&roomsMutex);

    if (registeredRoomsCount >= (roomsArraySize-1)) {
        CHAT_ROOM *tmpArray = realloc(roomsArray, roomsArraySize+ROOMS_ARRAY_STEP_SIZE);
        free(roomsArray);
        roomsArray = tmpArray;
    }
    CHAT_ROOM *newRoom = malloc(sizeof(CHAT_ROOM));
    newRoom->roomId = registeredRoomsCount;
    strcpy(newRoom->roomName, name);

    roomsArray[registeredRoomsCount] = *newRoom;
    free(newRoom);
    int returnValue = registeredRoomsCount;
    registeredRoomsCount++;

    pthread_mutex_unlock(&roomsMutex);
    return returnValue;
}

int existsRoomWithId(int roomId) {
    pthread_mutex_lock(&roomsMutex);

    int result = (roomId < registeredRoomsCount);

    pthread_mutex_unlock(&roomsMutex);
    return result;
}
