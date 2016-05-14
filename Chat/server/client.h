#ifndef CLIENT_H
#define CLIENT_H

#define CLIENTS_ARRAY_STEP_SIZE 10

typedef struct client {
    unsigned int    clientId;
    char*           name;
    unsigned int    chatRoom;
} CLIENT;

int registerNewClient(char *nick);

#endif
