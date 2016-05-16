#ifndef CLIENT_H
#define CLIENT_H

#define CLIENTS_ARRAY_STEP_SIZE 10

void initializeClientsManager();
int registerNewClient(char *nick, struct sockaddr clientAddress);
int changeClientNick(int clientId, char *newNick);
int changeClientRoom(int clientId, unsigned int newRom);
int leaveRoom(int clientId);
int bindDataSocket(int clientId, int socket);

#endif
