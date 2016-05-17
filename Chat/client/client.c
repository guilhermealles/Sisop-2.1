#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>
#include "../interface.h"
#include <pthread.h>

#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)
#define SERVER_PORT 3010
#define BUFF 1024
#define IP_SERVIDOR "127.0.0.1"
#define CHECK_RESPONSE 0
#define SAVE_CLIENT_ID 1
#define SAVE_NUMBER_OF_ROOMS 2

void connectToServer();
void setNick();
void socketReceiver();
void printRooms();
void userActions();
void joinRoom();
void leaveRoom();
void requestRoomList();
void requestRegister();
void createRoom();
int readServerResponse(int id);

SOCKET s, receiver;
CHAT_ROOM *chat_room;
struct sockaddr_in  s_cli, s_serv;

int ID = -1;
int porta_cli;
char nick[MAX_NICK_LENGTH];
char byteInicio;
char *receiveBuffer;
int number_of_rooms = 0;
int selectedRoom = -1;
int enableToWrite = 0;



int main (int argc, char **argv){
	pthread_t thread;

	connectToServer();

	printf("Welcome to Earth chat!!!\n");
	requestRegister();
	if (pthread_create(&thread, NULL, (void *)socketReceiver, NULL) != 0) {
		fprintf(stderr, "Error when creating a thread.\n");
		exit(EXIT_FAILURE);
	}
	userActions();

	pthread_join(thread, NULL);
}

void userActions(){

	char text[MAX_MESSAGE_LENGTH];
	int in = 1, confirm;

	printf("Commands: \n");
	printf("*1 - join into a room chat\n");
	printf("*2 - leave a room chat\n");
	printf("*3 - create a room chat\n");
	printf("*4 - change nickname\n");
	printf("*5 - list rooms\n");
	printf("*6 - exit from Earth chat\n");


	while(in){
		fgets(text, MAX_MESSAGE_LENGTH, stdin);
		if(text[0] == '*'){
			switch(text[1]){
				case '1':
					printf("** Join room **\n");
					joinRoom();
					break;
				case '2':
					printf("** Leave room **\n");
					leaveRoom();
					break;
				case '3':
					printf("** Create room **\n");
					createRoom();
					break;
				case '4':
					setNick();
					break;
				case '5':
					printf("** Request room list **\n");
					requestRoomList();
					break;
				case '6':
					close(s);
					exit(0);
				default:
					break;
			}
		}
		else {
			if(enableToWrite){
				printf("mensagem: %s\n", text);
				if(strlen(text) <= MAX_MESSAGE_LENGTH){
					MESSAGE *message = malloc(sizeof(MESSAGE));
					message->clientId = ID;
					message->tag = MESSAGE_TO_ROOM;
					message->size = strlen(text) + sizeof(int) + sizeof(int);
					message->roomId = selectedRoom;
					strcpy(message->messageText, text);

					// envia para o socket de dados
					confirm = write(s, message, sizeof(MESSAGE));
					if (confirm < 0){
						printf("Erro na transmissão\n");
						close(s);
						return;
					}
					printf("mensagem enviada \n");
				}else{
					printf("Warning: message is to big. \n");
				}
			}
		}
	}

}

void socketReceiver(){
	int confirm;
	char firstByte[1];

	// enquanto nao tiver registro no socket de comando, nao abre o socket de dados
	while(ID == -1);

	if ((receiver = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("Erro iniciando socket\n");
		return;
	}

	s_serv.sin_family = AF_INET;
	s_serv.sin_addr.s_addr = inet_addr(IP_SERVIDOR);
	s_serv.sin_port = htons(DATA_SERVER_PORT);

	if(connect(receiver, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0){
		printf("Erro na conexao\n");
		close(receiver);
		exit(1);
	}

	// concatena informacoes do pacote
	CONFIRM_CLIENT_MESSAGE *client = malloc(sizeof(CONFIRM_CLIENT_MESSAGE));
	client->tag = CREATE_ROOM;
	client->size = sizeof(int);
	client->clientId = ID;

	// envia para o servidor
	confirm = write(receiver, client, sizeof(CONFIRM_CLIENT_MESSAGE));
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(receiver);
		return;
	}

	while(1){

		int bytes_read = 0;
		char buffer[BUFF];
		int notFound = 1, i=0;
		// se o cliente estiver dentro de uma sala
		if(enableToWrite){
			while (bytes_read < 1) {
				// Read at least the first byte
				int current_bytes_read = read(receiver, &buffer[bytes_read], BUFF);
				bytes_read += current_bytes_read;
			}

			int bytes_to_read = sizeof(MESSAGE) - bytes_read;
			while (bytes_to_read > 0) {
				int current_bytes_read = read(receiver, &buffer[bytes_read], BUFF);
				bytes_read += current_bytes_read;
				bytes_to_read -= current_bytes_read;
			}

			MESSAGE *message = (MESSAGE *)buffer;
			if(message->roomId == selectedRoom){
				while(notFound){
					if(chat_room[i].roomId == selectedRoom){
						notFound = 0;
					}else{
						i++;
					}
				}
				printf("%s @ %s: %s\n", message->nick, chat_room[i].roomName, message->messageText);
			}
		}
	}
}

int readServerResponse(int id){
	int bytes_read = 0;
	char buffer[BUFF];
	while (bytes_read < 1) {
		// Read at least the first byte
		int current_bytes_read = read(s, &buffer[bytes_read], BUFF);
		bytes_read += current_bytes_read;
	}

	int bytes_to_read = sizeof(SERVER_RESPONSE) - bytes_read;
	while (bytes_to_read > 0) {
		int current_bytes_read = read(s, &buffer[bytes_read], BUFF);
		bytes_read += current_bytes_read;
		bytes_to_read -= current_bytes_read;
	}
	// Here the message should be completely read

	SERVER_RESPONSE *response = (SERVER_RESPONSE *)buffer;
	if (response->tag == SERVER_REPLY) {
		if (response->response == SERV_REPLY_OK) {
			if (id == SAVE_CLIENT_ID) {
				ID = (int) strtol(response->message, NULL, 10);
				printf("Reply from server:\n\tID: %d.\n", ID);
			}
			else if (id == SAVE_NUMBER_OF_ROOMS) {
				number_of_rooms = (int) strtol(response->message, NULL, 10);
				printf("Reply from server:\n\tNumber of rooms: %d.\n", number_of_rooms);
			}
			return 1;
		}
		else if (response->response == SERV_REPLY_FAIL) {
			fprintf(stderr, "Error: Server responded FAILURE with message:\n\"%s\".\n", response->message);
			return 0;
		}
		else {
			fprintf(stderr, "Error: unrecognized response->response.\n");
			return 0;
		}
	}
	return 0;
}


void createRoom(){
	char roomName[MAX_ROOM_NAME_LENGTH];
	int package_length, confirm;
	int notValidRoomName = 1;

	do{
		printf("Please enter room name: \n");
		scanf("%s",roomName);

		if(strlen(roomName) > MAX_ROOM_NAME_LENGTH-1){
			printf("\n Room name must have up to %d characters.\n", MAX_ROOM_NAME_LENGTH-1);
		}else{
			notValidRoomName = 0;
		}
	}while(notValidRoomName);


	// seta tamanho dos pacotes
	package_length = sizeof(int) + strlen(roomName)+1;

	// concatena informacoes do pacote
	CREATE_ROOM_MESSAGE *create_message = malloc(sizeof(CREATE_ROOM_MESSAGE));
	create_message->tag = CREATE_ROOM;
	create_message->size = package_length;
	strcpy(create_message->roomName, roomName);
	create_message->clientId = ID;

	// envia para o servidor
	confirm = write(s, create_message, sizeof(CREATE_ROOM_MESSAGE));
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(s);
		return;
	}

	if(readServerResponse(CHECK_RESPONSE)){
		printf("Room created successfully.\n");
	}
	else {
		printf("Server returned error.\n");
	}
}

void requestRoomList(){
	int confirm, rec = 0;
	char pack_lenght[4];
	int convert_pack_lenght;
	char firstByte[1];

	// concatena informacoes do pacote
	REQUEST_ROOM_MESSAGE *req_message = malloc(sizeof(REQUEST_ROOM_MESSAGE));
	req_message->tag = LIST_ROOMS;
	req_message->size = 0;
	req_message->clientId = ID;

	// envia para o servidor
	confirm = write(s, req_message, sizeof(REQUEST_ROOM_MESSAGE));
	if (confirm < 0){
		printf("Erro na transmissão.\n");
		close(s);
		return;
	}
	if (readServerResponse(SAVE_NUMBER_OF_ROOMS) == 0) {
		fprintf(stderr, "Erro ao ler resposta do servidor.\n");
		close(s);
		return;
	}

	printRooms();
}

void leaveRoom(){
	int package_length, confirm;

	if(selectedRoom != -1){
		package_length = 0;

		// concatena informacoes do pacote
		LEAVE_MESSAGE *leave_message = malloc(sizeof(LEAVE_MESSAGE));
		leave_message->clientId = ID;
		leave_message->tag = LEAVE_ROOM;
		leave_message->size = 0;

		// envia para o servidor
		confirm = write(s, leave_message, sizeof(LEAVE_MESSAGE));
		if (confirm < 0){
			printf("Erro na transmissão\n");
			close(s);
			return;
		}

		if(readServerResponse(CHECK_RESPONSE)){
			printf("You left the room successfully.\n");
			enableToWrite = 0;
		}
		else{
			printf("You're already outside the room.\n");
		}
	}
}

void joinRoom(){
	int i, find = 0, confirm;
	int package_length;
	char join_tag = 'J';

	printf("Select a room:\n");
	scanf("%d",&selectedRoom);
	for(i=0; i<number_of_rooms; i++){
		if(chat_room[i].roomId == selectedRoom){
			find = 1;
		}
	}

	if(find == 1){
		package_length = sizeof(int);

		// concatena informacoes do pacote
		JOIN_MESSAGE *join_message = malloc(sizeof(JOIN_MESSAGE));
		join_message->tag = JOIN_ROOM;
		join_message->size = package_length;
		join_message->room = selectedRoom;
		join_message->clientId = ID;

		// envia para o servidor
		confirm = write(s, join_message, sizeof(JOIN_MESSAGE));
		if (confirm < 0){
			printf("Erro na transmissão\n");
			close(s);
			return;
		}
		if(readServerResponse(CHECK_RESPONSE)){
			printf("You entered in the room.\n");
			enableToWrite = 1;
		}
	}
	else {
		printf("Selected room doesn't exist. \n");
		selectedRoom = 0;
	}

}

void printRooms(){
	char buffer[BUFF];
	int bytes_read = 0;

	while (bytes_read < (sizeof(CHAT_ROOM)*number_of_rooms)) {
		int current_bytes_read = read(s, &buffer[bytes_read], sizeof(CHAT_ROOM));
		bytes_read += current_bytes_read;
		printf("bytes read %d \n", bytes_read);
	}
	// Here all the rooms should have already been read
	chat_room = malloc(sizeof(CHAT_ROOM) * number_of_rooms);
	int i=0;
	for(i=0; i<number_of_rooms; i++) {
		unsigned int buffer_offset = i * sizeof(CHAT_ROOM);
		CHAT_ROOM *room = (CHAT_ROOM*) &buffer[buffer_offset];
		chat_room[i].roomId = room->roomId;
		strcpy(chat_room[i].roomName, room->roomName);
		printf("\tRoom %d - %s\n", room->roomId, room->roomName);
	}
}

void requestRegister(){
	char* nick_package;
	int package_length;
	int confirm, notValidNick = 1;

	do{
		printf("Please enter your nick name: \n");
		scanf("%s",nick);

		if(strlen(nick) > MAX_NICK_LENGTH){
			printf("\n Nickname must have up to %d characters.\n", MAX_NICK_LENGTH);
		}else{
			notValidNick = 0;
		}
	}while(notValidNick);


	// seta tamanho dos pacotes
	package_length = strlen(nick);

	// concatena informacoes do pacote
	REQUEST_REGISTER *request_message = malloc(sizeof(REQUEST_REGISTER));
	request_message->tag = CLIENT_REGISTER;
	request_message->size = package_length;
	strcpy(request_message->nick, nick);

	// envia para o servidor
	confirm = write(s, request_message, sizeof(REQUEST_REGISTER));
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(s);
		return;
	}

	if(readServerResponse(SAVE_CLIENT_ID)){
		printf("You are registered.\n");
	}else{
		printf("Error: error to register, probably your nick already exists.\n");
		requestRegister();
	}

}
void setNick(){

	char* nick_package;
	int package_length;
	int confirm, notValidNick = 1;

	do{
		printf("Please enter your nick name: \n");
		scanf("%s",nick);

		if(strlen(nick) > MAX_NICK_LENGTH){
			printf("\n Nickname must have up to %d characters.\n", MAX_NICK_LENGTH);
		}else{
			notValidNick = 0;
		}
	}while(notValidNick);


	// seta tamanho dos pacotes
	package_length = sizeof(int) + strlen(nick);
	//printf("tamanho pacote: %d\n", package_length);

	// concatena informacoes do pacote
	NICK_MESSAGE *nick_message = malloc(sizeof(NICK_MESSAGE));
	nick_message->tag = SET_NICK;
	nick_message->size = package_length;
	nick_message->clientId = ID;
	strcpy(nick_message->nick, nick);

	// envia para o servidor
	confirm = write(s, nick_message, sizeof(NICK_MESSAGE));
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(s);
		return;
	}

	if (readServerResponse(CHECK_RESPONSE)) {
		printf("Nickname changed.\n");
	}
	else {
		fprintf(stderr, "Server replied error.\n");
	}
}

void connectToServer(){

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("Erro iniciando socket\n");
		return;
	}

	s_serv.sin_family = AF_INET;
	s_serv.sin_addr.s_addr = inet_addr(IP_SERVIDOR);
	s_serv.sin_port = htons(SERVER_PORT);

	if(connect(s, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0){
		printf("Erro na conexao\n");
		close(s);
		exit(1);
	}
}
