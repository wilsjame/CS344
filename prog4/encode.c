//12345678912345678
//B AVTCJCLSJPALYBZ
//IN THE BEGINNING

#include <stdio.h>
#include <string.h>

main()
{
	int i;
	int encodedChar;
	int plainVal, keyVal;
	char chars[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	char key[100000]; memset(key, '\0', sizeof(key));
	char plaintext[100000]; memset(plaintext, '\0', sizeof(plaintext));
	char encodedtext[420710]; memset(encodedtext, '\0', sizeof(encodedtext));

	strcpy(key, "B AVTCJCLSJPALYBZ");
	strcpy(plaintext, "IN THE BEGINNING");

	printf("key       : %s\n", key);
	printf("plaintext : %s\n", plaintext);

	for(i = 0; i < (int)strlen(plaintext); i++)
	{
		
		/* Convert characters to literal alphabet indexes A = 0, B = 1, ..., ' ' = 26 */
		if(plain[i] == ' ')
		{
			plainVal = 26;
		}
		else
		{
			plainVal = plain[i] - 65; // A = 65, B = 66, ...
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
		 * IN THE BEGINNING  : message
		 *
		 * (1)B : key
		 * (8)I : message
		 *
		 * (1+8) = (9) : key + message
		 *
		 * (9) mod 27  : if key + message > 27 keep subtracting 27 until false
		 *
		 * (9)J : ciphertext :D */

		encodedChar = plainVal + keyVal; /* 

		while(encryptedChar > 27)
		{
			

	return;
}
