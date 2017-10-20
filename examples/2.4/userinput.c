#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main()
{

	/* ProTurnip, see -5? Know future var assignment failed */
	int numCharsEntered = -5; // How many chars we entered
	int currChar = -5; // Tracks where we are when we print out every char

	/* Magic settings
	 * size_t is unsigned integer type to hold result of sizeof(). 
	 */
	size_t bufferSize = 0; // Holds how large the allocated buffer is 
	char* lineEntered = NULL; //Points to a buffer allocated by getline() that holds our entered string + \n + \0

	while(1)
	{

		/* Get input from user */
		printf("Enter in a line of text (CTRL-C to exit):");

		/* stdin or could be a regular file opened as a stream with fopen(). 
		 * Parameter 1: address to array or string where data is placed.
		 * Parameter 2: size of buffer you want opened up.
		 * Parameter 3: where to get data from.
		 */

			/* Magic settings here, getline() will buffer automatically using malloc :^) */
		numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // Get a line from the user
		printf("Allocated %zu bytes for the %d chars you entered.\n", bufferSize, numCharsEntered);
		printf("Here is the raw entered line: \"%s\"\n", lineEntered);

		/* Print out actual contents of the string that was entered */
		printf("Here are the contents of the entire buffer:\n");
		printf("  # CHAR INT\n");

		for(currChar = 0; currChar < bufferSize; currChar++) // Display every character in both dec and ASCII
		{

			/* Third % casts char as the ascii integer equivalent */ 
			printf("%3d '%c\' %3d\n", currChar, lineEntered[currChar], lineEntered[currChar]);
		}

		free(lineEntered); // Free memory allocated by getline() or else memory leak >:o
		lineEntered = NULL; // Make getline() automatically space on the next iteration through the loop

	}

}

