#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include "../interface.h"

#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)
#define SERVER_PORT 3010
#define BUFF 1024
#define IP_SERVIDOR "127.0.0.1"
#define PORTA_CLI 2345

void connectToServer();
void setNick();

SOCKET s;
struct sockaddr_in  s_cli, s_serv;
int porta_cli;
char nick[32];


int main (int argc, char **argv){

	connectToServer();
	printf("Welcome to Earth chat!! \n");
	setNick();

	// acoes clientes

}

void setNick(){

	char nick_tag[1] = "N";
	char* nick_package;
	int package_length;
	int confirm;
   
	printf("Please enter your nick name: \n");
	fgets(nick, 32, stdin);	

	nick_package = malloc(strlen(nick) + 1 + 4); // tag + int
	package_length = strlen(nick_package);
	sprintf(nick_package,"%s%s%d",nick_tag, nick_tag, package_length);
	
	confirm = write(s, nick_package, sizeof(nick_package));  //(const char *)&
	if (confirm < 0){
		printf("Erro na transmissão\n");
		close(s);
		return;
	}
	   
   /* TODO: while até receber tudo
   // ler primeiro byte para ver o tamanho
   bzero(buffer, 1024);
   confirm = read(sockfd, buffer, 1024);
   
   if (confirm < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
	
   printf("%s\n",buffer);
   return 0;
*/


}
void connectToServer(){	

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("Erro iniciando socket\n");
		return;
	}

	s_cli.sin_family = AF_INET;
	s_cli.sin_addr.s_addr = htonl(INADDR_ANY);
	s_cli.sin_port = htons(PORTA_CLI);

	if ((bind(s, (struct sockaddr *)&s_cli, sizeof(s_cli))) != 0){
		printf("Erro no bind\n");
		close(s);
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
