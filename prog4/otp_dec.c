/*********************************************************************
** Author: James G Wilson
** Date: 11/30/2017
** Description: Program 4 - OTP CS 344 
** otp_dec.c
** Client side decrpytion.
**
 *	USAGE
 * ./otp_dec ciphertext key port
 *
 *	OUTLINE
 * This program provides the input data for the server side decryption.
 * It receives decoded ciphertext (plaintext) that it sends to stdout. 
 *
 * Verify the following for the command line arguments:
 * 	Ciphertext file has no bad characters.
 * 	Key file is atleast as long as the ciphertext file.
 * 	Port given can be found. 
 *
 * otp_dec should NOT be able to connect to otp_enc_d!
 *
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/ioctl.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
void errorCheck(char* ciphertextFileName, char* keyFileName, char* ciphertext, char* key);
void buildPayload(char* payload, char* ciphertext, char* key);

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

	char buffer[256]; memset(buffer, '\0', sizeof(buffer)); 
	char completeMessage[300000]; memset(completeMessage, '\0', sizeof(completeMessage));

	char key[100000]; memset(key, '\0', sizeof(key));
	char ciphertext[100000]; memset(ciphertext, '\0', sizeof(ciphertext));
	char payload[300000]; memset(payload, '\0', sizeof(payload));

	/* Usage. */
    	if (argc < 4) { fprintf(stderr,"USAGE: %s ciphertext key port\n", argv[0]); exit(0); } // Check usage & args

	/* Error checking and store command line arguments in local variables. */
	errorCheck(argv[1], argv[2], ciphertext, key);

	/* Set up the server address struct. */
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT dec: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	/* Set up the socket. Endpoint of communication with server. */
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT dec: ERROR opening socket");

	/* Connect socket to the server. */
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT dec: ERROR connecting");

	/* Build message to send to server. Format: e$PLAIN TEXT*KEY TEXT! */
	buildPayload(payload, ciphertext, key);

	/* Send message to server. */
	int payloadSize = strlen(payload);
	int beginIndex = 0, endIndex = 250;
	int charsWrittenTotal = 0;

	/* Send packets to reach payload size:
	 * Copy packet into buffer,
	 * send and get return bytes,
	 * add return bytes to begin and end index. */
	while(charsWrittenTotal < payloadSize)
	{

		/* Copy packet into buffer and send. */
		memset(buffer, '\0', sizeof(buffer));
		strncpy(buffer, payload + beginIndex, endIndex - beginIndex);
		charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
		charsWrittenTotal += charsWritten;

		/* Hull breach! */
		if (charsWritten < 0) error("CLIENT dec: ERROR writing to socket");

		/* Verify send by waiting until send buffer is clear. */
		int checkSend = -5; // Bytes remaining in send buffer
		do
		{
			ioctl(socketFD, TIOCOUTQ, &checkSend); // Check the send buffer for this socket
			//printf("checkSend: %d\n", checkSend); // Curiousity, check how many remaining bytes
		}
		while(checkSend > 0);

		if(checkSend < 0) // Check if loop stopped because of an error
			error("CLIENT dec: ioctl error");

		/* Increment packet section in payload. */
		if (charsWritten < strlen(buffer)) 
		{
			printf("CLIENT dec: WARNING: Not all data written to socket!\n");
			beginIndex += charsWritten;
			endIndex += charsWritten;
		}
		else
		{
			beginIndex += 250;
			endIndex += 250;
		}

	}

	/* Clear stale payload. */
	memset(payload, '\0', sizeof(payload));

	/* Receive return message from server. */
	while(strstr(payload, "!") == NULL)
	{
		memset(buffer, '\0', sizeof(buffer));
		charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
		strcat(payload, buffer); // Append packet to payload
		if (charsRead < 0) error("CLIENT dec: ERROR reading from socket");
	}
	
	int terminalLocation = strstr(payload, "!") - payload;
	payload[terminalLocation] = '\0';

	//printf("CLIENT dec: I received this from the server: \"%s\"\n", payload);
	printf("%s", payload);
	
	close(socketFD); // Close the socket

	return 0;
}

/* Check the following for the command line arguments:
 * 	Plaintext file has no bad characters.
 * 	Key file is atleast as long as the ciphertext file. */
void errorCheck(char* ciphertextFileName, char* keyFileName, char* ciphertext, char* key)
{
	FILE* ciphertextFile = fopen(ciphertextFileName, "r");
	FILE* keyFile = fopen(keyFileName, "r");
	long ciphertextSize;
	long keySize;
	size_t i, n = 0;
	int c;
	char* badCharacters = "$*!(#*djs8301these-are-all-bad-characters@@@"; // '@' placeholder

	/* Get length of ciphertext. */
	fseek(ciphertextFile, 0, SEEK_END);
	ciphertextSize = ftell(ciphertextFile);
	fseek(ciphertextFile, 0, SEEK_SET);

	/* Get length of key. */
	fseek(keyFile, 0, SEEK_END);
	keySize = ftell(keyFile);
	fseek(keyFile, 0, SEEK_SET);

	/* Compare lengths, key must be atleast as long as ciphertext. */
	if(keySize < ciphertextSize)
	{
		fprintf(stderr, "CLIENT dec: ERROR, key size too small\n"); exit(1); 
	}

	/* Check ciphertext file for bad characters and store file contents. */
	while((c = fgetc(ciphertextFile)) != EOF)
	{
		(char)c;
		i = 0; 

		while(badCharacters[i++] != '@')
		{

			if(c == badCharacters[i])
			{
				fprintf(stderr, "CLIENT dec: ERROR, bad ciphertext character\n"); exit(1); 
			}

		}

		ciphertext[n++] = c; 
	}

	ciphertext[n] = '\0';
	n = 0;

	/* Store key file contents. */
	while((c = fgetc(keyFile)) != EOF)
	{
		key[n++] = (char)c; 
	}

	key[n] = '\0';

	return;

}

/* Build the single message to be sent to the server.
 * The following will be seperated by special characters:
 * flag character to signify the sender (this program),
 * ciphertext, and key. e$PLAIN TEXT*KEY TEXT! */
void buildPayload(char* payload, char* ciphertext, char* key)
{
	strcpy(payload, "e");
	strcat(payload, "$");
	strcat(payload, ciphertext);
	strcat(payload, "*");
	strcat(payload, key);
	strcat(payload, "!");

	return;

}

