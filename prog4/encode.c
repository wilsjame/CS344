#include <stdio.h>
#include <string.h>

main(int argc, char*argv[])
{
	char* encodedtext[420710]; memset(encodedtext, '\0', sizeof(encodedtext));

	printf("H: %d + E: %d = %d mod 27 = %d\n", 'H', 'E', 'H' + 'E', ('H' + 'E') % 27);
	printf("The space char ' ' has a value of: %d\n", ' ');

	return;
}
