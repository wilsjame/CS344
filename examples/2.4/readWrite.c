#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
	int file_descriptor;
	char* newFilePath = "./newFile.txt";
	char* giveEm = "THE BUSINESS\n";

	/* Special type reserved for C in Unix 
	 * used to track number of bytes read
	 * and written.
	 */
	ssize_t nread, nwritten;
	char readBuffer[32];
					   /* Mode bits			Permissions      */
	file_descriptor = open(newFilePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	if (file_descriptor == -1)
	{
		printf("Hull breach = open() failed on \"%s\"\n", newFilePath);
		perror("In main()");
		exit(1);
	}

	/* Try writing to file. */
	nwritten = write(file_descriptor, giveEm, strlen(giveEm) * sizeof(char));

	/* Clear out the array before using it. */
	memset(readBuffer, '\0', sizeof(readBuffer)); 

	/* Move file pointer to beginning of file. */
	lseek(file_descriptor, 0, SEEK_SET);

	/* Read contents of file into buffer */
	nread = read(file_descriptor, readBuffer, sizeof(readBuffer));
	printf("File contents:\n%s", readBuffer);

	return 0;

}

