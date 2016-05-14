#ifndef INTERFACE_H
#define INTERFACE_H

// Some constants
#define SERVER_PORT 3010
#define MAX_CLIENTS_PER_ROOM 30
#define MAX_NICK_LENGTH 31

// Possible server responses
#define SERV_REPLY_OK 1
#define SERV_REPLY_FAIL 0

// List of all possible "tags"
#define SERVER_REPLY 'R'
#define CLIENT_REGISTER 'C'
#define LIST_ROOMS 'L'
#define JOIN_ROOM 'J'
#define MESSAGE_TO_ROOM 'M'
#define LEAVE_ROOM 'Q'
#define SET_NICK 'N'

typedef struct chatRoom {
    unsigned int    roomId;
    char*           roomName;
    CLIENT         connectedClients[MAX_CLIENTS_PER_ROOM];
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
