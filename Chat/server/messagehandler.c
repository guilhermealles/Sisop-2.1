#include "../interface.h"
#include "client.h";

int handleRegisterClient(char *buffer) {

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
        return SERV_REPLY_FAIL
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
