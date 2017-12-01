/*********************************************************************
** Author: James G Wilson
** Date: 11/25/2017
** Description: Program 4 - OTP CS 344 
** otp_enc_d.c
** Server side encryption.
**
 *	USAGE 
 * ./server listeningPort &
 * ./otp_enc_d listeningPort &
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
void extractPayload(char* payload, char* plaintext, char* key);
void encode(char* plaintext, char* key, char* encodedtext);


int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256]; memset(buffer, '\0', sizeof(buffer));
	struct sockaddr_in serverAddress, clientAddress;

	char key[100000]; memset(key, '\0', sizeof(key));
	char plaintext[100000]; memset(plaintext, '\0', sizeof(plaintext));
	char payload[300000]; memset(payload, '\0', sizeof(payload));
	char encodedtext[100000]; memset(encodedtext, '\0', sizeof(encodedtext));
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
	if (listenSocketFD < 0) error("SERVER enc: ERROR opening socket");

	/* Bind socket to port # to enable the socket to begin listening for clients. */
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("SERVER enc: ERROR on binding");

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
		if (establishedConnectionFD < 0) error("SERVER enc: ERROR on accept");
		//printf("SERVER enc: Connected Client at port %d\n", ntohs(clientAddress.sin_port));

		/* Fork off child to complete the encoding transaction. */
		spawnPid = fork();

		switch(spawnPid)
		{
			case -1:
				perror("SERVER enc: fork() failure! \n");
				exit(1);

				break;

			case 0:		/* In child. */

				/* Read client's message from the socket.  
				 * verify it's from otp_enc, and extract 
				 * the payload in packets. */
				while(strstr(payload, "!") == NULL)
				{
					memset(buffer, '\0', sizeof(buffer));
					charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer) - 1, 0); 
					strcat(payload, buffer); // Append packet to payload
					if (charsRead < 0) error("SERVER enc: ERROR reading from socket");
				}
				
				int terminalLocation = strstr(payload, "!") - payload;
				payload[terminalLocation] = '\0';

				/* Verify it's from otp_enc, and extract the payload, */ 
				extractPayload(payload, plaintext, key);
				encode(plaintext, key, encodedtext);

				/* Add flag at end of encoded text for client, 
				 * and wipe stale payload. */
				strcat(encodedtext, "!");
				memset(payload, '\0', sizeof(payload));

				/* Send return message back to the client. */
				int payloadSize = strlen(encodedtext);
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
					strncpy(buffer, encodedtext + beginIndex, endIndex - beginIndex);
					charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0); // Write to the server
					charsWrittenTotal += charsWritten;

					/* Hull breach! */
					if (charsWritten < 0) error("SERVER enc: ERROR writing to socket");

					/* Verify send by waiting until send buffer is clear. */
					int checkSend = -5; // Bytes remaining in send buffer
					do
					{
						ioctl(establishedConnectionFD, TIOCOUTQ, &checkSend); // Check the send buffer for this socket
						//printf("checkSend: %d\n", checkSend); // Curiousity, check how many remaining bytes
					}
					while(checkSend > 0);

					if(checkSend < 0) // Check if loop stopped because of an error
						error("SERVER enc: ioctl error");

					/* Hull breach! */
					if (charsWritten < 0) error("SERVER enc: ERROR writing to socket");

					/* Increment payload packet section. */
					if (charsWritten < strlen(buffer)) 
					{
						printf("SERVER enc: WARNING: Not all data written to socket!\n");
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
 * Format: e$PLAIN TEXT*KEY TEXT! */
void extractPayload(char* payload, char* plaintext, char* key)
{

	/* Verify the client/sender. */
	char* verifySenderFlag = strtok(payload, "$");	
	
	if(strcmp(verifySenderFlag, "e") != 0) 
	{
		fprintf(stderr, "SERVER enc: ERROR, unverified sender!\n"); 
		
		return;

	}

	/* extract payload contents. */
	strcpy(plaintext, strtok(NULL, "*"));
	strcpy(key, strtok(NULL, "!"));

	return;
}

/* Encode the plaintext using an OTP algorithm. */
void encode(char* plaintext, char* key, char* encodedtext)
{
	int i;
	int encodedChar;
	int plainVal, keyVal;
	char chars[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	/* Testing. */
	/*
	printf("\n--- In encode() server side ---\n");
	printf("key       : %s\n", key);
	printf("plaintext : %s\n", plaintext);
	printf("------------------------------\n");
	*/
	
	/* Encoding. */
	for(i = 0; i < (int)strlen(plaintext); i++)
	{
		
		/* Convert characters to literal alphabet indexes A = 0, B = 1, ..., ' ' = 26 */
		if(plaintext[i] == ' ')
		{
			plainVal = 26;
		}
		else
		{
			plainVal = plaintext[i] - 65; // A = 65, B = 66, ...
		}

		if(key[i] == ' ')
		{
			keyVal = 26;
		}
		else
		{
			keyVal = key[i] - 65;
		}

		/* *** OTP algorithm ***
		 * B AVTCJCLSJPALYBZ : key
		 * IN THE BEGINNING  : message
		 *
		 * (66)B 	     : key
		 * (73)I 	     : message
		 *
		 * (66+73) = (139)   : key + message
		 *
		 * (139)             : if key + message > 27 subtract 27 until false
		 * (4)		     : result
		 *
		 * (4) % 27          : take remainder of dividing result by 27
		 * (4)J              : ciphertext :D */
		encodedChar = plainVal + keyVal;  

		while(encodedChar > 27)
		{
			encodedChar -= 27;
		}

		encodedChar %= 27;
		encodedtext[i] = chars[encodedChar];
	}

	/* Testing. */
	/*
	printf("encoded   : %s\n", encodedtext);
	printf("------------------------------\n");
	*/

	return;

}

