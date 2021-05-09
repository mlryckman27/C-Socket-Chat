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


void handleTCPClient(int clntSocket);


int main(int argc, char *argv[]) {
	// TODO: create separate header file for error handling 
	// 	 functions to replace printf() statements in main()
	
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

		handleTCPClient(clntSock);

		// close the client connection with close() once the client is done (done by handleTCPCleint())

	}



	return 0;
}


void handleTCPClient(int clntSocket) {
	char buffer[BUFSIZ];					// Buffer for echo string
	char sendMsg[] = "Hello, world!\n";			// Echo this message
	int sendMsgLen = strlen(sendMsg);
	
	// Receive message from client
	ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZ, 0);
	if (numBytesRcvd < 0) {
		printf("recv() failed\n");
		exit(EXIT_FAILURE);
	}

	// Sent received string and receive again unil end of stream
	while (numBytesRcvd > 0) {
		//ssize_t numBytesSent = send(clntSocket, buffer, numBytesRcvd, 0);
		ssize_t sendBytes = send(clntSocket, sendMsg, sendMsgLen, 0);
		if (sendBytes < 0) {
			printf("send() failed\n");
			exit(EXIT_FAILURE);
		}
		//if (numBytesSent < 0) {
		//	printf("send() failed\n");
		//	exit(EXIT_FAILURE);
		//}
		//else if (numBytesSent != numBytesRcvd) {
		//	printf("send() error: numBytesSent != numBytesRcvd\n");
		//	exit(EXIT_FAILURE);
		//}

		numBytesRcvd = recv(clntSocket, buffer, BUFSIZ, 0);
		if (numBytesRcvd < 0) {
			printf("recv() failed\n");
			exit(EXIT_FAILURE);
		}
	}

	close(clntSocket);
}

