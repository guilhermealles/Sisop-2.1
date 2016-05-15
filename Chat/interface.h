#ifndef INTERFACE_H
#define INTERFACE_H

// Some constants
#define SERVER_PORT 3010
#define MAX_CLIENTS_PER_ROOM 30
#define MAX_NICK_LENGTH 31
#define MAX_ROOM_NAME_LENGTH 51

// Possible server responses
#define SERV_REPLY_OK 1
#define SERV_REPLY_FAIL 0

// List of all possible "tags"
#define SERVER_REPLY 'R'
#define CLIENT_REGISTER 'C' // ok
#define LIST_ROOMS 'L'
#define JOIN_ROOM 'J' // ok
#define MESSAGE_TO_ROOM 'M'
#define LEAVE_ROOM 'Q' //ok
#define SET_NICK 'N' // ok

typedef struct client {
    unsigned int    clientId;
    char            nick[MAX_NICK_LENGTH];
    unsigned int    chatRoom;
} CLIENT;

typedef struct chatRoom {
    unsigned int    roomId;
    char            roomName[MAX_ROOM_NAME_LENGTH];
    unsigned int    connectedClientIds[MAX_CLIENTS_PER_ROOM];
} CHAT_ROOM;

typedef struct message {
    char            tag;
    unsigned int    size;
    unsigned int    clientId;
    char*           messageText;

} MESSAGE;

typedef struct nickMessage {
    char            tag;
    unsigned int    size;
    unsigned int    clientId;
    char            nick[MAX_NICK_LENGTH];

} NICK_MESSAGE;

typedef struct joinMessage {
    char            tag;
    unsigned int    size;
    unsigned int    clientId;
    unsigned int    room;

} JOIN_MESSAGE;

typedef struct leaveMessage {
    char            tag;
    unsigned int    size;
    unsigned int    clientId;

} LEAVE_MESSAGE;

typedef struct requestRoomMessage {
    char            tag;
    unsigned int    size;
    unsigned int    clientId;

} REQUEST_ROOM_MESSAGE;

typedef struct serverResponse {
    char            tag;
    int             response;
    char*           message;        // Em caso de erro, dá pra imprimir essa mensagem pro cliente...
} SERVER_RESPONSE;
#endif
