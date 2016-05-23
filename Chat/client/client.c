#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>
#include "../interface.h"
#include <pthread.h>
#include <gtk/gtk.h>

#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)
#define SERVER_PORT 3010
#define BUFF 1024
#define IP_SERVIDOR "127.0.0.1"
#define CHECK_RESPONSE 0
#define SAVE_CLIENT_ID 1
#define SAVE_NUMBER_OF_ROOMS 2

void sendButtonCallback(GtkWidget *widget, gpointer data);
void doUserAction(char *buffer);
void connectToServer();
void setNick(char *buffer);
void socketReceiver();
void printRooms();
void joinRoom(char *buffer);
void leaveRoom();
void requestRoomList();
void requestRegister();
void createRoom(char *buffer);
int readServerResponse(int id);
void closeConnection();

SOCKET s, receiver;
CHAT_ROOM *chat_rooms = NULL;
struct sockaddr_in  s_cli, s_serv;

int ID = -1;
int porta_cli;
char nick[MAX_NICK_LENGTH];
char byteInicio;
char *receiveBuffer;
int number_of_rooms = 0;
int selectedRoom = -1;
int enableToWrite = 0;

// These variables are used to indicate the state of the client application, i.e
// what to do with the next message that comes from the GUI
unsigned int set_nick_action=0, join_room_action=0, create_room_action=0;

GtkWidget *entry1;
/* Para rodar: gcc -o client client.c `pkg-config --libs --cflags gtk+-2.0` -lpthread  */


int main (int argc, char **argv){

	pthread_t thread;

	connectToServer();

	printf("\nWelcome to Earth chat!!!\n");
	requestRegister();
	if (pthread_create(&thread, NULL, (void *)socketReceiver, NULL) != 0) {
		fprintf(stderr, "Error when creating a thread.\n");
		exit(EXIT_FAILURE);
	}

	/*
	* Tela de input
	*/

	GtkWidget *window;
	GtkWidget *table;
	GtkWidget *tableInside;
	GtkWidget *description;
	GtkWidget *button;
	PangoFontDescription *df;

	df = pango_font_description_from_string("Monospace");

	gtk_init(&argc, &argv);

	// janela

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "Earth Chat");
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);


	table = gtk_table_new(3, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(window), table);

	tableInside = gtk_table_new(2,1,FALSE);
	gtk_container_add(GTK_CONTAINER(table), tableInside);

	//  label e campo
	description = gtk_label_new("       Commands   \n\n *1 - Join a chat room\n *2 - Leave a chat room\n *3 - Create a chat room\n *4 - Change nickname\n *5 - List chat rooms\n *6 - Exit Earth chat\n");

	entry1 = gtk_entry_new();

	pango_font_description_set_size(df,10*PANGO_SCALE);
	gtk_widget_modify_font(description, df);

	gtk_table_attach(GTK_TABLE(tableInside), description, 1,2,1,2,
	  GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

	gtk_table_attach(GTK_TABLE(table), entry1, 0, 1, 2, 3,
	  GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

	// botao

	button = gtk_button_new_with_label("Enviar");
	gtk_widget_set_size_request(button, 80, 30);

	gtk_table_attach(GTK_TABLE(table), button, 1, 2, 2, 3,
	  GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

	g_signal_connect(button, "clicked", G_CALLBACK(sendButtonCallback), NULL);


	// finalizacao

	gtk_widget_show_all(window);

	g_signal_connect(window, "destroy",
	  G_CALLBACK(gtk_main_quit), &entry1);

	gtk_main();
	/*
	* Final tela de input
	*/


	pthread_join(thread, NULL);
}

void sendButtonCallback(GtkWidget *widget, gpointer data) {
	const gchar *entry_text1;
	GtkWidget* ld = (GtkWidget*)data;

	entry_text1 = gtk_entry_get_text(GTK_ENTRY(entry1));

	if (set_nick_action) {
		setNick((char*)entry_text1);
	}
	else if (join_room_action) {
		joinRoom((char*)entry_text1);
	}
	else if (create_room_action) {
		createRoom((char*)entry_text1);
	}
	else {
		doUserAction((char*)entry_text1);
	}
	gtk_entry_set_text((GtkEntry*)entry1, "");

}

void doUserAction(char *buffer) {
	if(buffer[0] == '*'){
		switch(buffer[1]){
			case '1':
				printf("\n** Join room **\n\n");
				printf("Please enter the ID of the room you wish to enter.\n");
				join_room_action = 1;
				break;
			case '2':
				printf("\n** Leave room **\n\n");
				leaveRoom();
				break;
			case '3':
				printf("\n** Create room **\n\n");
				printf("Please enter the room name.\n");
				create_room_action = 1;
				break;
			case '4':
				printf("\n** Set nickname **\n\n");
				printf("Please enter your new nickname.\n");
				set_nick_action = 1;
				break;
			case '5':
				printf("\n** Request room list **\n\n");
				requestRoomList();
				break;
			case '6':
				closeConnection();
				exit(0);
			default:
				break;
		}
	}
	else {
		if(enableToWrite){
			if(strlen(buffer) <= MAX_MESSAGE_LENGTH){
				MESSAGE *message = malloc(sizeof(MESSAGE));
				message->clientId = ID;
				message->tag = MESSAGE_TO_ROOM;
				message->size = strlen(buffer) + sizeof(int) + sizeof(int);
				message->roomId = selectedRoom;
				strcpy(message->senderNick, nick);
				strcpy(message->messageText, buffer);

				// envia para o socket de dados
				int confirm = write(s, message, sizeof(MESSAGE));
				free(message);
				if ((confirm < 0) || !(readServerResponse(CHECK_RESPONSE))){
					printf("Transmission error\n");
					close(s);
					return;
				}
			}else{
				printf("Warning: message is too big. \n");
			}
		}
	}
}

void socketReceiver() {
	int confirm;
	char firstByte[1];
	// enquanto nao tiver registro no socket de comando, nao abre o socket de dados
	while(ID == -1);
	if ((receiver = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("Error when starting socket.\n");
		return;
	}

	s_serv.sin_family = AF_INET;
	s_serv.sin_addr.s_addr = inet_addr(IP_SERVIDOR);
	s_serv.sin_port = htons(DATA_SERVER_PORT);

	if(connect(receiver, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0){
		printf("Connection error.\n");
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
	free(client);
	if (confirm < 0){
		printf("Transmissioon error\n");
		close(receiver);
		return;
	}

	while(1){
		int bytes_read = 0;
		char buffer[BUFF];
		int notFound = 1, i=0;
		// se o cliente estiver dentro de uma sala
		if(enableToWrite){
			if (confirm < 0) {
				  perror("ERROR reading from socket");
				  exit(1);
			}
			while (bytes_read < 1) {
				// Read at least the first byte
				int current_bytes_read = read(receiver, &buffer[bytes_read], sizeof(MESSAGE));
				bytes_read += current_bytes_read;
			}
			int bytes_to_read = sizeof(MESSAGE) - bytes_read;
			while (bytes_to_read > 0) {
				int current_bytes_read = read(receiver, &buffer[bytes_read], bytes_to_read);
				bytes_read += current_bytes_read;
				bytes_to_read -= current_bytes_read;
			}

			MESSAGE *message = (MESSAGE *)buffer;
			if(message->roomId == selectedRoom && enableToWrite){
				while(notFound){
					if(chat_rooms[i].roomId == selectedRoom){
						notFound = 0;
					}else{
						i++;
					}
				}
				printf("%s @ %s: %s\n", message->senderNick, chat_rooms[i].roomName, message->messageText);
			}
		}
	}
}

int readServerResponse(int id){
	int bytes_read = 0;
	char buffer[BUFF];
	while (bytes_read < 1) {
		// Read at least the first byte
		int current_bytes_read = read(s, &buffer[bytes_read], sizeof(SERVER_RESPONSE));
		bytes_read += current_bytes_read;
	}

	int bytes_to_read = sizeof(SERVER_RESPONSE) - bytes_read;
	while (bytes_to_read > 0) {
		int current_bytes_read = read(s, &buffer[bytes_read], bytes_to_read);
		bytes_read += current_bytes_read;
		bytes_to_read -= current_bytes_read;
	}
	// Here the message should be completely read

	SERVER_RESPONSE *response = (SERVER_RESPONSE *)buffer;
	if (response->tag == SERVER_REPLY) {
		if (response->response == SERV_REPLY_OK) {
			if (id == SAVE_CLIENT_ID) {
				ID = (int) strtol(response->message, NULL, 10);
				//printf("Reply from server:\n\tID: %d.\n", ID);
			}
			else if (id == SAVE_NUMBER_OF_ROOMS) {
				number_of_rooms = (int) strtol(response->message, NULL, 10);
				printf("Number of rooms: %d.\n", number_of_rooms);
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


void createRoom(char *buffer){
	char roomName[MAX_ROOM_NAME_LENGTH];
	int package_length, confirm;

	if (strlen(buffer) > MAX_ROOM_NAME_LENGTH-1) {
		printf("Room name must have a maximum of %d characters.\n", MAX_ROOM_NAME_LENGTH-1);
		printf("Please enter the room name.\n");
		return;
	}

	strcpy(roomName, buffer);

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
	free(create_message);
	if (confirm < 0){
		printf("Transmission error\n");
		close(s);
		create_room_action=0;
		return;
	}

	if(readServerResponse(CHECK_RESPONSE)){
		printf("Room created successfully.\n");
	}
	else {
		printf("Server returned error, please try again later.\n");
	}
	create_room_action=0;
	requestRoomList();
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
	free(req_message);
	if (confirm < 0){
		printf("Transmission error.\n");
		close(s);
		return;
	}
	if (readServerResponse(SAVE_NUMBER_OF_ROOMS) == 0) {
		fprintf(stderr, "Error when reading response from server.\n");
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
		free(leave_message);
		if (confirm < 0){
			printf("Transmission error\n");
			close(s);
			return;
		}
		int response1 = readServerResponse(CHECK_RESPONSE);

		MESSAGE *message = malloc(sizeof(MESSAGE));
		message->clientId = ID;
		message->tag = MESSAGE_TO_ROOM;
		message->roomId = selectedRoom;
		strcpy(message->senderNick, nick);
		sprintf(message->messageText, "%s saiu do chat.", nick);

		// envia para o socket de dados
		int confirm = write(s, message, sizeof(MESSAGE));
		free(message);
		if (confirm < 0){
			printf("Transmission error\n");
			close(s);
			return;
		}

		int response2 = readServerResponse(CHECK_RESPONSE);
		if(response1 && response2){
			printf("You left the room successfully.\n");
			selectedRoom = -1;
		}

		enableToWrite = 0;
		requestRoomList();
	}
}

void joinRoom(char *buffer){
	int i, find = 0, confirm;
	int package_length;
	char join_tag = 'J';

	int room = (int)strtol(buffer, NULL, 10);
	for(i=0; i<number_of_rooms; i++){
		if(chat_rooms[i].roomId == room){
			find = 1;
		}
	}

	if(find == 1){
		package_length = sizeof(int);

		// concatena informacoes do pacote
		JOIN_MESSAGE *join_message = malloc(sizeof(JOIN_MESSAGE));
		join_message->tag = JOIN_ROOM;
		join_message->size = package_length;
		join_message->room = room;
		join_message->clientId = ID;

		// envia para o servidor
		confirm = write(s, join_message, sizeof(JOIN_MESSAGE));
		free(join_message);
		if (confirm < 0){
			printf("Transmission error\n");
			close(s);
			return;
		}
		if(readServerResponse(CHECK_RESPONSE)){
			printf("You entered in the room!!\nStart talking:\n\n");
			selectedRoom = room;
			enableToWrite = 1;
		}
	}
	else {
		printf("Selected room wasn't found. Please try updating the rooms list and try again.\n");
		selectedRoom = -1;
	}

	// Disable the callback flag
	join_room_action = 0;
}

void printRooms(){
	char buffer[BUFF];
	int bytes_read = 0;

	while (bytes_read < (sizeof(CHAT_ROOM)*number_of_rooms)) {
		int current_bytes_read = read(s, &buffer[bytes_read], sizeof(CHAT_ROOM));
		bytes_read += current_bytes_read;
		//printf("bytes read %d \n", bytes_read);
	}
	if (chat_rooms != NULL) {
		free(chat_rooms);
	}
	// Here all the rooms should have already been read
	chat_rooms = malloc(sizeof(CHAT_ROOM) * number_of_rooms);
	int i=0;
	for(i=0; i<number_of_rooms; i++) {
		unsigned int buffer_offset = i * sizeof(CHAT_ROOM);
		CHAT_ROOM *room = (CHAT_ROOM*) &buffer[buffer_offset];
		chat_rooms[i].roomId = room->roomId;
		strcpy(chat_rooms[i].roomName, room->roomName);
		printf("\tRoom %d - %s\n", room->roomId, room->roomName);
	}
}

void requestRegister(){
	char new_nick;
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
	free(request_message);
	if (confirm < 0){
		printf("Transmission error\n");
		close(s);
		return;
	}

	if(readServerResponse(SAVE_CLIENT_ID)){
		printf("\nYou are registered.\n");
		requestRoomList();
	}else{
		printf("Error when registering, please try another nickname.\n");
		requestRegister();
	}

}
void setNick(char *buffer){
	char new_nick[MAX_NICK_LENGTH];
	int package_length;
	int confirm;

	if (strlen(buffer) > (MAX_NICK_LENGTH-1)) {
		printf("Nickname must have a maximum of %d characters.\n", MAX_NICK_LENGTH-1);
		printf("Please enter a new nickname.\n");
		return;
	}
	strcpy(new_nick, buffer);

	// seta tamanho dos pacotes
	package_length = sizeof(int) + strlen(new_nick);
	//printf("tamanho pacote: %d\n", package_length);

	// concatena informacoes do pacote
	NICK_MESSAGE *nick_message = malloc(sizeof(NICK_MESSAGE));
	nick_message->tag = SET_NICK;
	nick_message->size = package_length;
	nick_message->clientId = ID;
	strcpy(nick_message->nick, new_nick);

	// envia para o servidor
	confirm = write(s, nick_message, sizeof(NICK_MESSAGE));
	free(nick_message);
	if (confirm < 0){
		printf("Transmission error\n");
		close(s);
		set_nick_action=0;
		return;
	}

	if (readServerResponse(CHECK_RESPONSE)) {
		strcpy(nick, new_nick);
		printf("Nickname changed.\n");
	}
	else {
		fprintf(stderr, "Server replied error, please try again later.\n");
	}
	set_nick_action=0;
}

void connectToServer(){

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("Error when starting socket.\n");
		return;
	}

	s_serv.sin_family = AF_INET;
	s_serv.sin_addr.s_addr = inet_addr(IP_SERVIDOR);
	s_serv.sin_port = htons(SERVER_PORT);

	if(connect(s, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0){
		printf("Connection error.\n");
		close(s);
		exit(1);
	}
}

void closeConnection(){
	int confirm;

	CLOSE_CHAT_MESSAGE *close_message = malloc(sizeof(CLOSE_CHAT_MESSAGE));
	close_message->tag = CLOSE_CHAT;
	close_message->size = sizeof(int);
	close_message->clientId = ID;

	// envia para o servidor
	confirm = write(s, close_message, sizeof(CLOSE_CHAT_MESSAGE));
	free(close_message);
	if (confirm < 0){
		printf("Transmission error\n");
		close(s);
		return;
	}

	close(s);
	printf("Nice to see you! Come back soon!!\n");

}
