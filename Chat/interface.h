#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_CLIENTS_PER_ROOM 30
#define SERVER_PORT 3010

// Possible server responses
#define SERV_REPLY_OK 1
#define SERV_REPLY_FAIL 0

// List of all possible "tags"
#define SERVER_REPLY 'R'
#define LIST_ROOMS 1
#define JOIN_ROOM 'J'
#define MESSAGE_TO_ROOM 3
#define LEAVE_ROOM 'L'
#define SET_NICK 'N'

typedef struct chatRoom {
    unsigned int    roomId;
    char*           roomName;
    CLIENT         connectedClients[MAX_CLIENTS_PER_ROOM];
} CHAT_ROOM;

typedef struct message {
    unsigned int    size;
    char*           messageText;
} MESSAGE;

typedef struct nickMessage {
    char            tag;
    unsigned int    size;
    char            nick[32];
} NICK_MESSAGE;

typedef struct joinMessage {
    char            tag;
    unsigned int    size;
    int             room;

} JOIN_MESSAGE;

typedef struct leaveMessage {
    char            tag;
    unsigned int    size;

} LEAVE_MESSAGE;

typedef struct requestRoomMessage {
    char            tag;
    unsigned int    size;

} REQUEST_ROOM_MESSAGE;

typedef struct serverResponse {
    char            tag;
    int             response;
    char*           message;        // Em caso de erro, dá pra imprimir essa mensagem pro cliente...
} SERVER_RESPONSE;
#endif
