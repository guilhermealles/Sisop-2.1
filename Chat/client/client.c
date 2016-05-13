#include <stdio.h>

#include <stdlib.h>

#include <string.h>

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
void printRooms();

SOCKET s;
struct sockaddr_in  s_cli, s_serv;
int porta_cli;
char nick[32];
char receiveBuffer[BUFF];



int main (int argc, char **argv){
	pthread_t thread;

	connectToServer();

	if (pthread_create(&thread, NULL, (void *)socketReceiver, NULL) != 0) {

		fprintf(stderr, "Error when creating a thread.\n");

		exit(EXIT_FAILURE);

	} 

	printf("Welcome to Earth chat!!!\n");
	setNick();


	// acoes clientes
	
	pthread_join(thread, NULL);
}

void printRooms(){


}


void socketReceiver(){
	int confirm; 

	//TODO: decidir se ler um byte inicial e depois varios, ou ler os 1024

	while(1){
		bzero(receiveBuffer, 1024);
		confirm = read(s, receiveBuffer, 1024);

		if (confirm < 0) {
		  perror("ERROR reading from socket");
		  exit(1);
		}

		printf("buffer: %s\n",receiveBuffer);

		switch(receiveBuffer[0]){

			case 'S':
				printRooms();
				break;

			default:
				break;

		}
	}
   return;


}


void setNick(){

	char nick_tag[1] = "N";
	char* nick_package;
	int package_length;
	int confirm;
   
	printf("Please enter your nick name: \n");
	fgets(nick, 32, stdin);	

	// seta tamanho dos pacotes
	nick_package = malloc(strlen(nick) + 1 + 4); // tag + int
	package_length = strlen(nick) + 5;
	printf("tamanho pacote: %d\n", package_length);

	// concatena informacoes do pacote
	sprintf(nick_package,"%s%s%d",nick_tag, nick_tag, package_length);
	
	// envia para o servidor
	confirm = write(s, nick_package, sizeof(nick_package));

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