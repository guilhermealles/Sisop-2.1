#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include "../interface.h"
#include <pthread.h>

#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)
#define SERVER_PORT 3010
#define BUFF 1024
#define IP_SERVIDOR "127.0.0.1"
#define PORTA_CLI 2345

void connectToServer();
void setNick();
void socketReceiver();
void printRooms(int size);
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

int ID;
int porta_cli;
char nick[MAX_NICK_LENGTH];
char byteInicio;
char *receiveBuffer;
int number_of_rooms;
int selectedRoom = 0;
int enableToWrite = 0;



int main (int argc, char **argv){
	pthread_t thread;

	connectToServer();

	if (pthread_create(&thread, NULL, (void *)socketReceiver, NULL) != 0) {
		fprintf(stderr, "Error when creating a thread.\n");
		exit(EXIT_FAILURE);
	}

	printf("Welcome to Earth chat!!!\n");
	requestRegister();
	requestRoomList();

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
	printf("*5 - exit from Earth chat\n");


	while(in){
		scanf("%s",text);
		if(text[0] == '*'){
			switch(text[1]){
				case '1':
					printf("join room - insere um numero \n");
					joinRoom();
					break;
				case '2':
					printf("leave room \n");
					leaveRoom();
					break;
				case '3':
					printf("create room - insere um numero\n");
					createRoom();
					break;
				case '4':
					setNick();
					break;
				case '5':
					close(s);
					exit(0);
				default:
					break;
			}		
		}else{
			if(enableToWrite){

				MESSAGE *message = malloc(sizeof(MESSAGE));
				message->clientId = ID;
				message->tag = MESSAGE_TO_ROOM;
				message->size = strlen(text) + sizeof(int) + sizeof(int);
				message->roomId = selectedRoom;
				strcpy(message->messageText, text);

				// envia para o servidor
				confirm = write(s, message, sizeof(MESSAGE));
				if (confirm < 0){
					printf("Erro na transmissão\n");
					close(s);
					return;
				}
				printf("mensagem enviada \n");
			}
		}
	}

}

void socketReceiver(){
	int confirm;
	char firstByte[1];

	if ((receiver = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("Erro iniciando socket\n");
		return;
	}

	s_serv.sin_family = AF_INET;
	s_serv.sin_addr.s_addr = inet_addr(IP_SERVIDOR);
	s_serv.sin_port = htons(SERVER_PORT);

	if(connect(receiver, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0){
		printf("Erro na conexao\n");
		close(s);
		exit(1);
	}

	while(enableToWrite){

		bzero(firstByte, 0);
		confirm = read(s, firstByte, 1);

		if (confirm < 0) {
			  perror("ERROR reading from socket");
			  exit(1);
		}


		if(firstByte[0] == MESSAGE_TO_ROOM){
		//TODO:
		}
	}
}

int readServerResponse(int id){
	int confirm;
	char firstByte[1];
	char pack_response[4], pack_id[4];
	int convert_pack_lenght, response;

	while(confirm < 0){
		bzero(firstByte, 0);
		confirm = read(s, firstByte, 1);
	}
	/*	if (confirm < 0) {
		  perror("ERROR reading from socket");
		  exit(1);
		}
	*/

	if(firstByte[0] == SERVER_REPLY){

		confirm = read(s, pack_response, 4);

		if (confirm < 0) {
		  perror("ERROR reading from socket");
		  exit(1);
		}
		response = *((int*)pack_response);

		if(response == SERV_REPLY_OK){
			if(id == 1){
				confirm = read(s, pack_id, 4);
				ID = (int) strtol(pack_id, NULL, 10);
				printf("id: %d\n", ID);
			}
			return 1;
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

		if(strlen(roomName) > MAX_ROOM_NAME_LENGTH){
			printf("\n Room name must have up to 51 characters.\n");
		}else{
			notValidRoomName = 0;
		}
	}while(notValidRoomName);


	// seta tamanho dos pacotes
	package_length = sizeof(int) + strlen(roomName);

	// concatena informacoes do pacote
	CREATE_ROOM_MESSAGE *create_message = malloc(sizeof(CREATE_ROOM_MESSAGE));
	create_message->tag = CREATE_ROOM;
	create_message->size = package_length;
	strcpy(create_message->roomName, roomName);

	// envia para o servidor
	confirm = write(s, create_message, sizeof(CREATE_ROOM_MESSAGE));
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(s);
		return;
	}

	if(readServerResponse(0)){
		printf("Room created successfully.\n");
	}
}

void requestRoomList(){
	int confirm, rec = 0;
	char pack_lenght[4];
	int convert_pack_lenght;

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
// SEGMENTATION FAULT POR AQUI
	while(rec != 4)
		rec = read(s, pack_lenght, 4);

	convert_pack_lenght = *((int*)pack_lenght);

	printRooms(convert_pack_lenght);
}

void leaveRoom(){
	int package_length, confirm;

	if(selectedRoom != 0){
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

		if(readServerResponse(0)){
			printf("You left the room successfully.\n");
			enableToWrite = 0;
		}
	}

	requestRoomList();
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

		if(readServerResponse(0)){
			printf("You entered into the room.\n");
			enableToWrite = 1;
		}
	}else{
		printf("Selected room doesn't exist. \n");
		selectedRoom = 0;
	}

}

void printRooms(int size){

	int confirm, i, ind = 1;
	char buffer[size];
	char room_name[MAX_ROOM_NAME_LENGTH];

	bzero(buffer, size);
	confirm = read(s, buffer, size);

	if (confirm < 0) {
	  perror("ERROR reading from socket");
	  exit(1);
	}

	// pega o byte que indica o numero de salas
	number_of_rooms = (int) strtol(&buffer[0], NULL, 10);
	printf("Numero de salas: %d\n", number_of_rooms);

	chat_room = (CHAT_ROOM*) malloc(size * sizeof(CHAT_ROOM));

    for (i=0; i < number_of_rooms; i++) {
		// guarda id da sala
		chat_room[i].roomId = buffer[ind] - '0';

		// seleciona os 21 bytes do buffer referentes ao nome - incluindo \0
		memcpy(room_name, &buffer[ind + 1 ], MAX_ROOM_NAME_LENGTH);
		strcpy(chat_room[i].roomName, room_name);

		printf("%d - %s\n", chat_room[i].roomId, chat_room[i].roomName);

		ind+= MAX_ROOM_NAME_LENGTH+1;
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
	printf("tamanho pacote: %d\n", package_length);

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
