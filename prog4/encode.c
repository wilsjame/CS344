//B AVTCJCLSJPALYBZ key
//IN THE BEGINNING  plaintext

#include <stdio.h>
#include <string.h>

main()
{
	int i;
	int encodedChar, decodedChar;
	int plainVal, encodedVal, keyVal;
	char chars[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	char key[100000]; memset(key, '\0', sizeof(key));
	char plaintext[100000]; memset(plaintext, '\0', sizeof(plaintext));

	char encodedtext[420710]; memset(encodedtext, '\0', sizeof(encodedtext));
	char decodedtext[420710]; memset(encodedtext, '\0', sizeof(decodedtext));

	strcpy(key, "B AVTCJCLSJPALYBZ");
	strcpy(plaintext, "IN THE BEGINNING");

	printf("key       : %s\n", key);
	printf("plaintext : %s\n", plaintext);
	printf("------------------------------\n");

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

		/* B AVTCJCLSJPALYBZ : key
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

	printf("encoded   : %s\n", encodedtext);
	printf("------------------------------\n");

	/* Decoding. */
	for(i = 0; i < (int)strlen(encodedtext); i++)
	{
		
		/* Convert characters to literal alphabet indexes A = 0, B = 1, ..., ' ' = 26 */
		if(encodedtext[i] == ' ')
		{
			encodedVal = 26;
		}
		else
		{
			encodedVal = encodedtext[i] - 65; // A = 65, B = 66, ...
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
		decodedChar = encodedVal - keyVal;  

		while(decodedChar < 27)
		{
			decodedChar += 27;
		}

		decodedChar %= 27;
		decodedtext[i] = chars[decodedChar];
	}

	printf("decoded   : %s\n", decodedtext);
	printf("------------------------------\n");

	return;

}
