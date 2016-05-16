#ifndef INTERFACE_H
#define INTERFACE_H

#include <netinet/in.h>

// Some constants
#define SERVER_PORT 3010
#define DATA_SERVER_PORT 3020
#define PORTA_CLI 2345
#define MAX_CLIENTS_PER_ROOM 30
#define MAX_NICK_LENGTH 31
#define MAX_ROOM_NAME_LENGTH 51
#define MAX_REPLY_MESSAGE_SIZE 101
#define MAX_MESSAGE_LENGTH 512

// Possible server responses
#define SERV_REPLY_OK 1
#define SERV_REPLY_FAIL -1

// List of all possible "tags"
#define SERVER_REPLY 'R' //ok
#define CLIENT_REGISTER 'C' // ok
#define LIST_ROOMS 'L'
#define JOIN_ROOM 'J' // ok
#define MESSAGE_TO_ROOM 'M'
#define LEAVE_ROOM 'Q' //ok
#define SET_NICK 'N' // ok
#define CREATE_ROOM 'T' // ok
#define CONFIRM_CLIENT 'H'

typedef struct client {
    unsigned int    clientId;
    char            nick[MAX_NICK_LENGTH];
    unsigned int    chatRoom;
    struct sockaddr clientAddress;
    int             dataSocket;
} CLIENT;

typedef struct chatRoom {
    unsigned int    roomId;
    char            roomName[MAX_ROOM_NAME_LENGTH];

} CHAT_ROOM;

typedef struct message {
    char            tag;
    unsigned int    size;
    unsigned int    clientId;
    char            senderNick[MAX_NICK_LENGTH];
    unsigned int    roomId;
    char            messageText[MAX_MESSAGE_LENGTH];
	char 			nick[MAX_NICK_LENGTH];

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
    char            message[MAX_REPLY_MESSAGE_SIZE];        // Em caso de erro, dá pra imprimir essa mensagem pro cliente...
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

typedef struct confirmClient{
	char            tag;
    unsigned int    size;
	unsigned int    clientId;

} CONFIRM_CLIENT_MESSAGE;
#endif
