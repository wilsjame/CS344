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

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

void errorChecking(char* plaintextFileName, char* keyFileName);

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

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];

	char key[10533]; memset(key, '\0', sizeof(key));
	char plaintext[10533]; memset(key, '\0', sizeof(key));

	/* Usage. */
	// ./otp_enc plaintext key port
    	if (argc < 4) { fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[4]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	/* Error checking. */
	// Check key file for bad characters
	// Check plaintext file for bad characters
	// Check if key is shorter than plaintext
	errorChecking(argv[1], argv[2]);

	// Check if the port given cannot be found
	// Check if connecting to otp_dec_d (port?) not allowed!
	
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Get input message from user
	printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds

	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}

void errorChecking(char* plaintextFileName, char* keyFileName)
{
	FILE* plaintextFile = fopen(plaintextFileName, "r");
	FILE* keyFile = fopen(keyFileName, "r");
	long plaintextSize;
	long keySize;

	if(plaintextFile == NULL || keyFile == NULL)
	{
		printf("Error opening plaintext or key!\n");
	}

	/* Get length or plaintext. */
	/* Get length of */
	// get length of key
	// compare lengths
	


	return;

}

