#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

void initializeMessageHandler();
int handleRegisterClient(char *buffer);
int handleChangeNick(char *buffer);
int handleChangeRoom(char *buffer);
int handleLeaveRoom(char *buffer);
int handleCreateRoom(char *buffer);

#endif
