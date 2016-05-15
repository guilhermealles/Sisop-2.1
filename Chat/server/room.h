#ifndef ROOM_H
#define ROOM_H

#include "../interface.h"

#define ROOMS_ARRAY_STEP_SIZE 10

void initializeRoomsManager();
int createChatRoom(char *name);
int destroyChatRoom(int roomId);
int clientLeaveRoom(char *name, int roomId);
int clientJoinRoom(int clientId, int roomId);

#endif
