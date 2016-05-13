#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_CLIENTS_PER_ROOM 30
#define SERVER_PORT 3010

// List of all possible "tags"
#define LIST_ROOMS 1
#define JOIN_ROOM 2
#define MESSAGE_TO_ROOM 3
#define LEAVE_ROOM 4
#define CHANGE_NICK 5

typedef struct client {
    unsigned int    clientId;
    char*           name;
    unsigned int    chatRoom;
    unsigned int    port;
    char*           ip;     /* TODO Representamos IP como uma string? */
} CLIENT;

typedef struct chatRoom {
    unsigned int    roomId;
    char*           roomName;
    CLIENT         connectedClients[MAX_CLIENTS_PER_ROOM];
} CHAT_ROOM;

typedef struct message {
    unsigned int    size;
    char*           messageText;
} MESSAGE;

#endif
