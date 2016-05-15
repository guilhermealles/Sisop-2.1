#ifndef INTERFACE_H
#define INTERFACE_H

// Some constants
#define SERVER_PORT 3010
#define MAX_CLIENTS_PER_ROOM 30
#define MAX_NICK_LENGTH 31
#define MAX_ROOM_NAME_LENGTH 51
#define MAX_MESSAGE_LENGTH 1000

// Possible server responses
#define SERV_REPLY_OK 1
#define SERV_REPLY_FAIL 0

// List of all possible "tags"
#define SERVER_REPLY 'R' //ok
#define CLIENT_REGISTER 'C' // ok
#define LIST_ROOMS 'L'
#define JOIN_ROOM 'J' // ok
#define MESSAGE_TO_ROOM 'M'
#define LEAVE_ROOM 'Q' //ok
#define SET_NICK 'N' // ok
#define CREATE_ROOM 'T'

typedef struct client {
    unsigned int    clientId;
    char            nick[MAX_NICK_LENGTH];
    unsigned int    chatRoom;
} CLIENT;

typedef struct chatRoom {
    unsigned int    roomId;
    char            roomName[MAX_ROOM_NAME_LENGTH];

} CHAT_ROOM;

typedef struct message {
    char            tag;
    unsigned int    size;
    unsigned int    clientId;
    unsigned int    roomId;
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

typedef struct requestRegister{
    char            tag;
    unsigned int    size;
    char            nick[MAX_NICK_LENGTH];
}REQUEST_REGISTER;

typedef struct createRoomMessage{
    char            tag;
    unsigned int    size;
	unsigned int    clientId;
    char            roomName[MAX_ROOM_NAME_LENGTH];
}CREATE_ROOM_MESSAGE;
#endif
