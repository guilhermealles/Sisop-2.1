#ifndef ROOM_H
#define ROOM_H

#include "../interface.h"

#define ROOMS_ARRAY_STEP_SIZE 10

void initializeRoomsManager();
int createChatRoom(char *name);
int existsRoomWithId(int roomId);

#endif
