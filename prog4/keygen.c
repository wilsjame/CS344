/*********************************************************************
** Author: James G Wilson
** Date: 11/25/2017
** Description: Program 4 - OTP CS 344
** keygen.c
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>

main(int argc, char *argv[])
{
	int keyLength;
	char key[1024] = {0};
	
	/* Generate key */
	keyLength = atoi(argv[1]); 

	while(keyLength-- > 0)
	{
		key[keyLength] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[random() % 27];
		printf("%c", key[keyLength]);

	}

	printf("\n");
	
	return;

}
