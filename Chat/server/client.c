#include "client.h"

unsigned int registeredClientsCount;
CLIENT *clientsArray;

void initializeClientsManager() {
    registeredClientsCount = 0;
    clientsArray = malloc(sizeof(CLIENT) * CLIENTS_ARRAY_STEP_SIZE);
}

int registerNewClient(char *nick) {
    
}
