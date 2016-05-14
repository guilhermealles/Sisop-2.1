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

SOCKET s;
CHAT_ROOM *chat_room;
struct sockaddr_in  s_cli, s_serv;
int porta_cli;
char nick[32];
char byteInicio;
char *receiveBuffer;
int number_of_rooms;
int selectedRoom = 0;



int main (int argc, char **argv){
	pthread_t thread;

	connectToServer();

	if (pthread_create(&thread, NULL, (void *)socketReceiver, NULL) != 0) {
		fprintf(stderr, "Error when creating a thread.\n");
		exit(EXIT_FAILURE);
	}

	printf("Welcome to Earth chat!!!\n");
	setNick();
	requestRoomList()

	// TODO: imprime salas  - tratar atraso: usar flag, ou ..., ...
	userActions();


	pthread_join(thread, NULL);
}

void userActions(){

	char option;
	int in = 1;

	printf("Commands: \n");
	printf("1 - join into a room chat\n");
	printf("2 - leave a room chat\n");
	printf("3 - create a room chat\n");
	printf("4 - change nickname\n");
	printf("5 - exit from Earth chat\n");


	while(in){
		scanf("%c",&option);

		switch(option){
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
	}

}

void requestRoomList(){
	char req_tag = 'R';
	int confirm;

	// concatena informacoes do pacote
	REQUEST_ROOM_MESSAGE *req_message = malloc(sizeof(REQUEST_ROOM_MESSAGE));
	req_message->tag = req_tag;
	req_message->size = 0;

	// envia para o servidor
	confirm = write(s, req_message, sizeof(REQUEST_ROOM_MESSAGE));
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(s);
		return;
	}
}

void leaveRoom(){
	int package_length, confirm;
	char leave_tag = 'L';

	if(selectedRoom != 0){
		package_length = 0;

		// concatena informacoes do pacote
		LEAVE_MESSAGE *leave_message = malloc(sizeof(LEAVE_MESSAGE));
		leave_message->tag = leave_tag;
		leave_message->size = 0;

		// envia para o servidor
		confirm = write(s, leave_message, sizeof(LEAVE_MESSAGE));
		if (confirm < 0){
			printf("Erro na transmissão\n");
			close(s);
			return;
		}
	//TODO: ler resposta
	}

	requestRoomList();
}

void joinRoom(){
	int i, find = 0, confirm;
	int package_length;
	char join_tag = 'J';
	
	printf("Select a room:\n");
	scanf("%d",&selectedRoom);

	printf("sala escolhida %d\n", selectedRoom);

	for(i=0; i<number_of_rooms; i++){
		if(chat_room[i].roomId == selectedRoom){
			find = 1;
		}
	}

	if(find == 1){
		package_length = sizeof(char) + sizeof(int);

		// concatena informacoes do pacote
		JOIN_MESSAGE *join_message = malloc(sizeof(JOIN_MESSAGE));
		join_message->tag = join_tag;
		join_message->size = package_length;
		join_message->room = selectedRoom;

		// envia para o servidor
		confirm = write(s, join_message, sizeof(JOIN_MESSAGE));
		if (confirm < 0){
			printf("Erro na transmissão\n");
			close(s);
			return;
		}
	//TODO: ler resposta
		
	}else{
		printf("Selected room doesn't exist. \n");
		selectedRoom = 0;
	}	

}

void printRooms(int size){
	int confirm, i, ind = 1;
	char buffer[size];
	char room_name[21];  

	bzero(buffer, size);
	confirm = read(s, buffer, size);

	if (confirm < 0) {
	  perror("ERROR reading from socket");
	  exit(1);
	}
	
	// pega o byte que indica o numero de salas
	number_of_rooms = buffer[0] - '0';
	printf("Numero de salas: %d\n", number_of_rooms);

	chat_room = (CHAT_ROOM*) malloc(size * sizeof(CHAT_ROOM));	

    for (i=0; i < number_of_rooms; i++) {   
		// guarda id da sala		
		chat_room[i].roomId = buffer[ind] - '0';

		// seleciona os 21 bytes do buffer referentes ao nome - incluindo \0
		memcpy(room_name, &buffer[ind + 1 ], 21 );		
		strcpy(chat_room[i].roomName, room_name); 

		printf("%d - %s\n", chat_room[i].roomId, chat_room[i].roomName);

		ind+= 22;
	}

}


void socketReceiver(){
	int confirm;
	char firstByte[1];
	char pack_lenght[4];
	int convert_pack_lenght;

	while(1){
		bzero(firstByte, 0);
		confirm = read(s, firstByte, 1);

		if (confirm < 0) {
		  perror("ERROR reading from socket");
		  exit(1);
		}

	//	printf("buffer: %s\n",receiveBuffer);

		switch(firstByte[0]){

			case 'S':
				confirm = read(s, pack_lenght, 4);

				if (confirm < 0) {
				  perror("ERROR reading from socket");
				  exit(1);
				}
				convert_pack_lenght = *((int*)pack_lenght);
				
				printRooms(convert_pack_lenght);

				break;

			default:
				break;

		}
	}
   return;


}


void setNick(){

	char nick_tag = 'N';
	char* nick_package;
	int package_length;
	int confirm, notValidNick = 1;

	do{
		printf("Please enter your nick name: \n");
		scanf("%s",nick);

		if(strlen(nick) > 31){
			printf("\n Nickname must have up to 31 characters.\n");
		}else{
			notValidNick = 0;
		}
	}while(notValidNick);


	// seta tamanho dos pacotes
	nick_package = malloc(sizeof(char) + sizeof(int) + strlen(nick)); // tag + int   - somar \0 ??
	package_length = sizeof(char) + sizeof(int) + strlen(nick);
	printf("tamanho pacote: %d\n", package_length);

	// concatena informacoes do pacote
	NICK_MESSAGE *nick_message = malloc(sizeof(NICK_MESSAGE));
	nick_message->tag = nick_tag;
	nick_message->size = package_length;
	strcpy(nick_message->nick, nick);

	// envia para o servidor
	confirm = write(s, nick_message, sizeof(NICK_MESSAGE));
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(s);
		return;
	}

	//TODO: ler resposta
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
