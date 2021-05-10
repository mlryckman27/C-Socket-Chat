/* A simple chat client for sending 
 * messages to other chat clients who
 * are connected to the main server.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void recvMessage(int sck);
void sendMessage(int sck);


int main(int argc, char *argv[]) {

	// TODO: Create header file for all error functions to
	// 	 use in main().
	//
	// TODO: Implement routine that asks user to enter IP
	// 	 address of the client they want to connect to.
	
	if (argc != 3) {
		printf("Wrong number of arguments\n");
		exit(EXIT_FAILURE);
	}

	char *servIP = argv[1];
	in_port_t servPort = atoi(argv[2]);

	// Create TCP socket using socket()
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		printf("socket() failed\n");
		exit(EXIT_FAILURE);
	}	

	// Establish connection to server using connect()
	struct sockaddr_in servAddr;							// Socket for server
	memset(&servAddr, 0, sizeof(servAddr));						// Zero server socket struct
	servAddr.sin_family = AF_INET;							// IPv4 address family
	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);		// Convert IP address of server
	if (rtnVal == 0) {
		printf("inet_pton() failed: invalid address string\n");
		exit(EXIT_FAILURE);	
	}
	else if (rtnVal < 0) {
		printf("inet_pton() failed\n");
		exit(EXIT_FAILURE);
	}

	servAddr.sin_port = htons(servPort);						// Server port

	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		printf("connect() failed\n");
		exit(EXIT_FAILURE);
	}

	// Communicate using send() and recv()
	for(;;) {
		recvMessage(sock);
	}	

	// Close the connection with close()

	return 0;
}


void recvMessage(int sck) {
	char buffer[BUFSIZ];

	ssize_t numBytesRcvd = recv(sck, buffer, BUFSIZ, 0);
	if (numBytesRcvd < 0) {
		printf("recv() failed\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Server: ");
	fputs(buffer, stdout);

	while (numBytesRcvd > 0) {
		numBytesRcvd = recv(sck, buffer, BUFSIZ, 0);
		if (numBytesRcvd < 0) {
			printf("recv() failed\n");
			exit(EXIT_FAILURE);
		}

		printf("Server: ");
		fputs(buffer, stdout);
	}

	close(sck);
}

//void sendMessage(int sck) {
//
//}

