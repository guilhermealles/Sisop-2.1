#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_CLIENTS_PER_ROOM 30
#define SERVER_PORT 3010

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
