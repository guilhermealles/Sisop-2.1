#include <stdio.h>
#include <pthread.h>
#include "../interface.h"
#include "client.h"
#include "room.h"

pthread_mutex_t handlerMutex;

void initializeMessageHandler() {
    pthread_mutex_init(&handlerMutex, NULL);
}

int handleRegisterClient(char *buffer) {
    REQUEST_REGISTER *message = (REQUEST_REGISTER*) buffer;
    int clientId;
    if ((clientId = registerNewClient(message->nick)) == -1) {
        fprintf(stderr, "[THREAD] Error when registering user with nick %s. Probably nick already in use.\n", message->nick);
        return SERV_REPLY_FAIL;
    }

    printf("[THREAD] Registered new user\n\t\tNick: %s\n\t\tClientId: %d\n", message->nick, clientId);
    return SERV_REPLY_OK;
}

int handleChangeNick(char *buffer) {
    NICK_MESSAGE *message = (NICK_MESSAGE*) buffer;
    if (changeClientNick(message->clientId, message->nick) != message->clientId) {
        fprintf(stderr, "[THREAD] Error when changing the nick of cliend %d.\n", message->clientId);
        return SERV_REPLY_FAIL;
    }
    return SERV_REPLY_OK;
}

int handleChangeRoom(char *buffer) {
    JOIN_MESSAGE *message = (JOIN_MESSAGE*) buffer;
    if (changeClientRoom(message->clientId, message->room) != message->clientId) {
        fprintf(stderr, "[THREAD] Error when moving client %d to room %d!.\n", message->clientId, message->room);
        return SERV_REPLY_FAIL;
    }
    return SERV_REPLY_OK;
}

int handleLeaveRoom(char *buffer) {
    LEAVE_MESSAGE *message = (LEAVE_MESSAGE*) buffer;
    if (leaveRoom(message->clientId) != message->clientId) {
        fprintf(stderr, "[THREAD] Error when removing client %d from room.\n", message->clientId);
        return SERV_REPLY_FAIL;
    }
    return SERV_REPLY_OK;
}

int handleCreateRoom(char *buffer) {
    CREATE_ROOM_MESSAGE *message = (CREATE_ROOM_MESSAGE*) buffer;
    int newRoom = createChatRoom(message->roomName);
    extern unsigned int registeredClientsCount;
    if (((int)message->clientId >= 0) && (message->clientId < registeredClientsCount)) {
        if (changeClientRoom(message->clientId, newRoom) != message->clientId) {
            fprintf(stderr, "[THREAD] Error when moving client %d to the just created room %d.\n", message->clientId, newRoom);
        }
    }

    printf("[THREAD] Created new room:\n\t\tRoom ID: %d\n\t\tRoom Name: %s\n\t\tCreator ID: %d.\n", newRoom, message->roomName, message->clientId);
    return SERV_REPLY_OK;
}
