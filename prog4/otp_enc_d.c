/*********************************************************************
** Author: James G Wilson
** Date: 11/25/2017
** Description: Program 4 - OTP CS 344 
** otp_enc_d.c
** Server side encryption.
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
void extractPayload(char* payload, char* plaintext, char* key);

/*	USAGE 
 * ./server listeningPort &
 * ./otp_enc_d listeningPort &
 */

/*	OUTLINE
 * Output error if it cannot connect to the listening port upon execution.
 * Listen on the command line assigned port/socket.
 * When a connection is made:
 * 	Call accept() generating the socket used for communication.
 * 	Use a seperate process to handle the rest of the transaction,
 * 		which will occur on the newly adccepted socket.
 *
 * Child process now:
 * 	Check/verfiy it is communicating with otp_enc (client sise encryption).
 * 	Receive from otp_enc:
 * 		plaintext and key via communication socket (not the original listen socket).
 * 	Write back ciphertext to otp_enc process it's connected to cia the same communcation socket.
 * 	NOTE: Key passed in must be at least as big as the plaintext.
 *
 * More notes:
 * 	Support up to 5 concurrent sockets.
 * 	Fork method or pool method.
 * 	Error situations, see Canvas.
 * 	Anything else on Canvas.
 *
 */

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	char key[100000]; memset(key, '\0', sizeof(key));
	char plaintext[100000]; memset(plaintext, '\0', sizeof(plaintext));
	char payload[300000]; memset(payload, '\0', sizeof(payload));

	/* Usage. */
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	/* Set up the address struct for this process (the server). */
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	/* Set up the socket. Listening endpoint of communcation with client. */
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	/* Bind socket to port # to enable the socket to begin listening for clients. */
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");

	/* Begin queueing connection requests until limit is reached. */
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	/* Loop and accept clients indefinitely. */
	while(1)
	{

		/* Accept a connection, blocking if one is not available until one connects. */
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));

		/* Get the message from the client, verify it's from otp_enc, and extract the payload, */
		memset(payload, '\0', sizeof(payload));
		charsRead = recv(establishedConnectionFD, payload, sizeof(payload) - 1, 0); // Read the client's message from the socket
		if (charsRead < 0) error("ERROR reading from socket");
		printf("SERVER: I received this from the client: \"%s\"\n", payload);

		extractPayload(payload, plaintext, key);

		// Send a Success message back to the client
		charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
		if (charsRead < 0) error("ERROR writing to socket");
		close(establishedConnectionFD); // Close the existing socket which is connected to the client

	}

	close(listenSocketFD); // Close the listening socket

	return 0; 

}
/* Parse and extract the payload contents into their respective variables. 
 * Format: e$PLAIN TEXT*KEY TEXT! */
void extractPayload(char* payload, char* plaintext, char* key)
{

	/* Verify the client/sender. */
	char* verifySenderFlag = strtok(payload, "$");	
	
	if(strcmp(verifySenderFlag, "e") != 0) 
	{
		fprintf(stderr, "SERVER: ERROR, unverified sender!\n"); 
		
		return;

	}

	/* extract payload contents. */
	strcpy(plaintext, strtok(NULL, "*"));
	strcpy(key, strtok(NULL, "!"));

	// test
	/*
	printf("plaintext is: %s\n", plaintext);
	printf("key is: %s\n", key);
	*/

	return;
}

