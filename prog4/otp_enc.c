/*********************************************************************
** Author: James G Wilson
** Date: 11/25/2017
** Description: Program 4 - OTP CS 344 
** otp_enc.c
** Client side encryption.
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
void errorCheck(char* plaintextFileName, char* keyFileName, char* plaintext, char* key);

/*	USAGE
 * ./otp_enc plaintext key port
 *
 * port is the port otp_enc should attempt to connect to otp_enc_d on.
 * 
 * ./otp_enc myplaintext mykey 57171
 * ./otp_enc myplaintext mykey 57171 > myciphertext
 * ./otp_enc myplaintext mykey 57171 > myciphertext &
 *
 */

/*	OUTLINE
 * This program provides the input data for the server side encryption.
 * It receives ciphertext that it sends to stdout. 
 *
 * Verify the following for the command line arguments:
 * 	Plaintext file has no bad characters.
 * 	Key file is atleast as long as the plaintext file.
 * 	Port given can be found. 
 *
 * otp_enc should NOT be able to connect to otp_dec_d!
 *
 */

//TODO
// Check if the port given cannot be found
// Check if connecting to otp_dec_d (port?) not allowed!

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];

	char key[10533]; memset(key, '\0', sizeof(key));
	char plaintext[10533]; memset(plaintext, '\0', sizeof(plaintext));

	/* Usage. */
    	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

	/* Error checking and store command line arguments in local variables. */
	errorCheck(argv[1], argv[2], plaintext, key);

	/* Set up the server address struct. */
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	/* Set up the socket. Endpoint of communication with server. */
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	/* Connect socket to the server. */
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	/* Send message to server. */
	// single message shall contain encodingClient$$plaintext@@key@@@
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	/* Verify send by waiting until send buffer is clear. */
	int checkSend = -5; // Bytes remaining in send buffer
	do
	{
		ioctl(socketFD, TIOCOUTQ, &checkSend); // Check the send buffer for this socket
		//printf("checkSend: %d\n", checkSend); // Curiousity, check how many remaining bytes
	}
	while(checkSend > 0);

	if(checkSend < 0) // Check if loop stopped because of an error
		error("ioctl error");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket

	return 0;
}

void errorCheck(char* plaintextFileName, char* keyFileName, char* plaintext, char* key)
{
	FILE* plaintextFile = fopen(plaintextFileName, "r");
	FILE* keyFile = fopen(keyFileName, "r");
	long plaintextSize;
	long keySize;
	size_t i, n = 0;
	int c;
	char* badCharacters = "$*!(#*djs8301these-are-all-bad-characters@@@"; // '@' placeholder

	/* Get length of plaintext. */
	fseek(plaintextFile, 0, SEEK_END);
	plaintextSize = ftell(plaintextFile);
	fseek(plaintextFile, 0, SEEK_SET);

	/* Get length of key. */
	fseek(keyFile, 0, SEEK_END);
	keySize = ftell(keyFile);
	fseek(keyFile, 0, SEEK_SET);

	/* Compare lengths, key must be atleast as long as plaintext. */
	if(keySize < plaintextSize)
	{
		fprintf(stderr, "CLIENT: ERROR, key size too small\n"); exit(1); 
	}

	/* Check plaintext file for bad characters and store file contents. */
	while((c = fgetc(plaintextFile)) != EOF)
	{
		(char)c;
		i = 0; 

		while(badCharacters[i++] != '@')
		{

			if(c == badCharacters[i])
			{
				fprintf(stderr, "CLIENT: ERROR, bad plaintext character\n"); exit(1); 
			}

		}

		plaintext[n++] = c; 
	}

	plaintext[n] = '\0';
	n = 0;

	/* Store key file contents. */
	while((c = fgetc(keyFile)) != EOF)
	{
		key[n++] = (char)c; 
	}

	key[n] = '\0';

	return;

}

