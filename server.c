/* A simple server for handling incoming
 * connections from chat clients
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define		MEM_NAME_LEN		8
#define		MEM_PASS_LEN		8


// Chat member structure
struct member {
	char name[MEM_NAME_LEN];		// Name of chat member
	char password[MEM_PASS_LEN];		// Password for specific chat member
	int loggedIn;				// 1 if name/password verified; 0 otherwise
};


void recvMessage(int clntSocket);
void loginMember(int clntSocket, struct member *chatMember);


int main(int argc, char *argv[]) {
	// TODO: create separate header file for error handling 
	// 	 functions to replace printf() statements in main()


	struct member admin;
	strcpy(admin.name, "Matt");
	strcpy(admin.password, "Blue****");
	admin.loggedIn = 0;


	// Check for correct number of arguments
	if (argc != 2) {
		printf("Wrong number of arguments\n");
		exit(EXIT_FAILURE);
	}

	in_port_t servPort = atoi(argv[1]); 			// Port number for clients to connect to

	// Create TCP socket
	int servSock;
	if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Failed to create socket\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in servAddr;				// Local address
	memset(&servAddr, 0, sizeof(servAddr));			// Zero out the struct
	servAddr.sin_family = AF_INET;				// IPv$ address family
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);		// Any incoming address
	servAddr.sin_port = htons(servPort);			// Local port

	// Assign port number to socket with bind()
	if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		printf("bind() failed\n");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections on the port
	if (listen(servSock, 5) < 0) {
		printf("listen() failed\n");
		exit(EXIT_FAILURE);
	}

	int login = 0;
	while (!login) {
		struct sockaddr_in clntAddr;
		socklen_t clntAddrLen = sizeof(clntAddr);
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);

		loginMember(clntSock, &admin);
		//close(clntSock);
	}

	// Loop forever
	for (;;) {
		struct sockaddr_in clntAddr;			// Client address
		socklen_t clntAddrLen = sizeof(clntAddr);	// Length of client address structure
		
		// Call accept() to get a new socket for each new client connection
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
		if (clntSock < 0) {
			printf("accept() failed\n");
			exit(EXIT_FAILURE);
		}

		// Communicate with the client using send() and recv()
		char clntName[INET_ADDRSTRLEN];			// String to contain client address
		if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
			printf("Handling client %s %d\n", clntName, ntohs(clntAddr.sin_port));
		else
			puts("Unable to get client address");

		recvMessage(clntSock);

	}



	return 0;
}


void recvMessage(int clntSocket) {
	char bufRecv[BUFSIZ];

	// Receive message from client
	ssize_t numBytesRcvd = recv(clntSocket, bufRecv, BUFSIZ, 0);
	if (numBytesRcvd < 0) {
		printf("recv() failed\n");
		exit(EXIT_FAILURE);
	}
	
	if (numBytesRcvd > 0)
		fputs("Client: ", stdout);
	fputs(bufRecv, stdout);

	close(clntSocket);
}

void loginMember(int clntSocket, struct member *chatMember) {
	char nameLoginMessage[] = "Enter user name: ";
	//char passLoginMessage[] = "Enter password: ";
	int nameLoginMessageLen = strlen(nameLoginMessage);
	//int passLoginMessageLen = strlen(passLoginMessage);

	char nameLogin[MEM_NAME_LEN];
	//char passLogin[MEM_PASS_LEN];

	ssize_t numBytesSent = send(clntSocket, nameLoginMessage, nameLoginMessageLen, 0);
	if (numBytesSent < 0) {
		printf("send() failed\n");
		exit(EXIT_FAILURE);
	}

	ssize_t numBytesRcvd = recv(clntSocket, nameLogin, MEM_NAME_LEN, 0);
	if (numBytesRcvd < 0) {
		printf("recv() failed\n");
		exit(EXIT_FAILURE);
	}
	else {
		nameLogin[strcspn(nameLogin, "\n")] = '\0';	
		printf("%s\n", nameLogin);
		printf("%s\n", chatMember->name);
	}

	if (strcmp(nameLogin, chatMember->name) == 0) {
		ssize_t loginReply = send(clntSocket, "User name matches database.\n", BUFSIZ, 0);
		if (loginReply < 0) {
			printf("send() failed\n");
			exit(EXIT_FAILURE);
		}

		chatMember->loggedIn = 1;
	}
	else {
		ssize_t loginReply = send(clntSocket, "No user found by that name in the database.\n", BUFSIZ, 0);
	}
}
