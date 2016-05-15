#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include <netinet/in.h>

void initializeMessageHandler();
int handleRegisterClient(char *buffer, struct sockaddr clientAddress);
int handleChangeNick(char *buffer);
int handleChangeRoom(char *buffer);
int handleLeaveRoom(char *buffer);
int handleCreateRoom(char *buffer);
int handleListRoom(char *buffer);
int handleMessageToRoom(char *buffer);

#endif
