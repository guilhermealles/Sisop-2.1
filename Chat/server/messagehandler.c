#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
 //   extern CLIENT* clientsArray;
    printf("[THREAD] Registered new user\n\t\tNick: %s\n\t\tClientId: %d\n", message->nick, clientId);
    return clientId;
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

int handleMessageToRoom(char *buffer) {
    MESSAGE *message = (MESSAGE*) buffer;
    extern CLIENT *clientsArray;
    extern unsigned int registeredClientsCount;
    if (message->clientId >= registeredClientsCount) {
        fprintf(stderr, "[THREAD] Error when sending message: sender id %d not valid!\n", message->clientId);
        return SERV_REPLY_FAIL;
    }
    strcpy(message->senderNick, clientsArray[message->clientId].nick);

    printf("Client \"%s\" sent message to room %d. Message:\n\"%s\".\n", message->nick, message->roomId, message->messageText);
    int i;
    for(i=0; i<registeredClientsCount; i++) {
     //   if (clientsArray[i].chatRoom == message->roomId) {
            // Send the message to this client.
            if (clientsArray[i].dataSocket == -1) {
                fprintf(stderr, "[THREAD] Error: trying to send message to client %d with no data socket bound.\n", i);
            }
            int confirm = write(clientsArray[i].dataSocket, message, sizeof(MESSAGE));
            if (confirm < 0) {
                fprintf(stderr, "[THREAD] Error when writing message to socket.\n");
                return SERV_REPLY_FAIL;
            }
     //   }
    }
    return SERV_REPLY_OK;
}

int handleDisconnectClient(char *buffer) {
    CLOSE_CHAT_MESSAGE *message = (CLOSE_CHAT_MESSAGE*) buffer;
    if (disconnectClient(message->clientId) == -1) {
        fprintf(stderr, "[THREAD] Error when disconnecting client %d.\n", message->clientId);
        return SERV_REPLY_FAIL;
    }
    return SERV_REPLY_OK;
}
