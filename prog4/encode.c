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

		encodedChar %= 27; // Redundant?
		encodedtext[i] = chars[encodedChar];
	}

	printf("encoded   : %s\n", encodedtext);

	return;

}
