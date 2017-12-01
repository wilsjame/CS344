/*********************************************************************
** Author: James G Wilson
** Date: 11/30/2017
** Description: Program 4 - OTP CS 344 
** otp_dec_d.c
** Server side decryption.
**
 *	USAGE 
 * ./server listeningPort &
 * ./otp_dec_d listeningPort &
 *
 *	OUTLINE
 * Output error if it cannot connect to the listening port upon execution.
 * Listen on the command line assigned port/socket.
 * When a connection is made:
 * 	Call accept() generating the socket used for communication.
 * 	Use a seperate process to handle the rest of the transaction,
 * 		which will occur on the newly adccepted socket.
 *
 * Child process now:
 * 	Check/verfiy it is communicating with otp_dec (client sise decryption).
 * 	Receive from otp_dec:
 * 		ciphertext and key via communication socket (not the original listen socket).
 * 	Write back decoded ciphertext (plaintext) to otp_dec process it's connected to via the same communcation socket.
 * 	NOTE: Key passed in must be at least as big as the ciphertext.
 *
 * More notes:
 * 	Support up to 5 concurrent sockets.
 * 	Fork method or pool method.
 * 	Error situations, see Canvas.
 *
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
void extractPayload(char* payload, char* ciphertext, char* key);
void decode(char* ciphertext, char* key, char* decodedtext);


int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256]; memset(buffer, '\0', sizeof(buffer));
	struct sockaddr_in serverAddress, clientAddress;

	char key[100000]; memset(key, '\0', sizeof(key));
	char ciphertext[100000]; memset(ciphertext, '\0', sizeof(ciphertext));
	char payload[300000]; memset(payload, '\0', sizeof(payload));
	char decodedtext[100000]; memset(decodedtext, '\0', sizeof(decodedtext));
	int childExitMethod;
	int trackerSize = 0;
	pid_t spawnPid;

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
	if (listenSocketFD < 0) error("SERVER dec: ERROR opening socket");

	/* Bind socket to port # to enable the socket to begin listening for clients. */
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("SERVER dec: ERROR on binding");

	/* Begin queueing connection requests until limit is reached. */
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	/* Loop and accept clients indefinitely. */
	while(1)
	{

		/* Accept a connection, blocking if one is not available until one connects. 
		 * accept() generates a new socket to be used for actual communication,
		 * create a seperate process with fork() and use this socket to complete the transaction. */
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("SERVER dec: ERROR on accept");
		//printf("SERVER dec: Connected Client at port %d\n", ntohs(clientAddress.sin_port));

		/* Fork off child to complete the decoding transaction. */
		spawnPid = fork();

		switch(spawnPid)
		{
			case -1:
				perror("SERVER dec: fork() failure! \n");
				exit(1);

				break;

			case 0:		/* In child. */

				/* Read client's message from the socket.  
				 * verify it's from otp_dec, and extract 
				 * the payload in packets. */
				while(strstr(payload, "!") == NULL)
				{
					memset(buffer, '\0', sizeof(buffer));
					charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer) - 1, 0); 
					strcat(payload, buffer); // Append packet to payload
					if (charsRead < 0) error("SERVER dec: ERROR reading from socket");
				}
				
				int terminalLocation = strstr(payload, "!") - payload;
				payload[terminalLocation] = '\0';

				/* Verify it's from otp_dec, and extract the payload, */ 
				extractPayload(payload, ciphertext, key);
				decode(ciphertext, key, decodedtext);

				/* Add flag at end of decoded text for client, 
				 * and wipe stale payload. */
				strcat(decodedtext, "!");
				memset(payload, '\0', sizeof(payload));

				/* Send return message back to the client. */
				int payloadSize = strlen(decodedtext);
				int beginIndex = 0, endIndex = 250;
				int charsWritten, charsWrittenTotal = 0;
				int traceCounter = 0;

				/* Send packets to reach payload size:
				 * Copy packet into buffer,
				 * send and get return bytes,
				 * add return bytes to begin and end index. */
				while(charsWrittenTotal < payloadSize)
				{

					/* Copy packet into buffer and send. */
					memset(buffer, '\0', sizeof(buffer));
					strncpy(buffer, decodedtext + beginIndex, endIndex - beginIndex);
					charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Write to the server
					charsWrittenTotal += charsWritten;

					/* Hull breach! */
					if (charsWritten < 0) error("SERVER dec: ERROR writing to socket");

					/* Verify send by waiting until send buffer is clear. */
					int checkSend = -5; // Bytes remaining in send buffer
					do
					{
						ioctl(establishedConnectionFD, TIOCOUTQ, &checkSend); // Check the send buffer for this socket
						//printf("checkSend: %d\n", checkSend); // Curiousity, check how many remaining bytes
					}
					while(checkSend > 0);

					if(checkSend < 0) // Check if loop stopped because of an error
						error("SERVER dec: ioctl error");

					/* Hull breach! */
					if (charsWritten < 0) error("SERVER dec: ERROR writing to socket");

					/* Increment payload packet section. */
					if (charsWritten < strlen(buffer)) 
					{
						printf("SERVER dec: WARNING: Not all data written to socket!\n");
						beginIndex += charsWritten;
						endIndex += charsWritten;
					}
					else
					{
						beginIndex += 250;
						endIndex += 250;
					}

				}

				/* Close the existing socket which is connected to the client. */
				close(establishedConnectionFD); 

				/* Increment child counter and exit.
				 * TODO implement some form of IPC 
				 * between child and parent, pipes? */

				exit(0);

				break;

			default:	/* In parent. */

				/* Increment child counter and exit. */
				trackerSize++;

				break;

		} // End fork switch

		/* Check number of conncurrent (child) sockets running is less than 5, 
		 * and reap an orphan if needed. */
		if(trackerSize > 5)
		{
			waitpid(-1, &childExitMethod, WNOHANG); // -1 -> wait for any child
			trackerSize--;
		}
			
	} // End server while loop

	close(listenSocketFD); // Close the listening socket

	return 0; 

}

/* Parse and extract the payload contents into their respective variables. 
 * Format: d$PLAIN TEXT*KEY TEXT! */
void extractPayload(char* payload, char* ciphertext, char* key)
{

	/* Verify the client/sender. */
	char* verifySenderFlag = strtok(payload, "$");	
	
	if(strcmp(verifySenderFlag, "d") != 0) 
	{
		fprintf(stderr, "SERVER dec: ERROR, unverified sender!\n"); 
		
		return;

	}

	/* extract payload contents. */
	strcpy(ciphertext, strtok(NULL, "*"));
	strcpy(key, strtok(NULL, "!"));

	return;
}

/* Decode the ciphertext using an OTP algorithm. */
void decode(char* ciphertext, char* key, char* decodedtext)
{
	int i;
	int cipherChar, decodedChar;
	int cipherVal, keyVal;
	char chars[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	/* Testing. */
	/*
	printf("\n--- In decode() server side ---\n");
	printf("key       : %s\n", key);
	printf("ciphertext : %s\n", ciphertext);
	printf("------------------------------\n");
	*/
	
	/* Decoding. */
	for(i = 0; i < (int)strlen(ciphertext); i++)
	{
		
		/* Convert characters to literal alphabet indexes A = 0, B = 1, ..., ' ' = 26 */
		if(ciphertext[i] == ' ')
		{
			cipherVal = 26;
		}
		else
		{
			cipherVal = ciphertext[i] - 65; // A = 65, B = 66, ...
		}

		if(key[i] == ' ')
		{
			keyVal = 26;
		}
		else
		{
			keyVal = key[i] - 65;
		}

		/* B AVTCJCLSJPALYBZ : key
		 * JM N GIDPYRBNTKH  : cipher
		 *
		 * (66)B 	     : key
		 * (74)J 	     : cipher
		 *
		 * (74 - 66) = (8)   : cipher - key
		 *
		 * ***********************
		 * (8)               : if cipher - key < 27 add 27 until false
		 * (35)		     : result
		 *
		 * (35) % 27         : take remainder of dividing result by 27
		 * (8)I              : decoded :D */
		decodedChar = cipherVal - keyVal;  

		while(decodedChar < 27)
		{
			decodedChar += 27;
		}

		decodedChar %= 27;
		decodedtext[i] = chars[decodedChar];
	}

	/* Testing. */
	/*
	printf("decoded   : %s\n", decodedtext);
	printf("------------------------------\n");
	*/

	return;

}

