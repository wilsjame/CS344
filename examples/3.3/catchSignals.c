#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* Two functions of interest. 
 * Both are passed an int indicating the signal number. */

/* This function raises SIGUSR2 which inevitably kills. */
void catchSIGINT(int signo)
{

	/* Note: Can't use printf() or strlen() in a signal handler,
	 * as they are non-reentrant. */
	char* message = "Caught SIGINT! sleeping for 5 sec ZZzzz\n";
	write(STDOUT_FILENO, message, 38);
	raise(SIGUSR2); // Send thyself SIGUSR2. It goes in the queue until this function finishes.
	sleep(5);
}

/* This function actually kills it. */
void catchSIGUSR2(int signo)
{
	char* message = "Caught SIGUSR2, exiting!\n";
	write(STDOUT_FILENO, message, 25);
	exit(0);
}

/* Set up signal sets that set up sigaction function to run. */
main()
{
	/* Define three seperate sigaction structs 
	 * initialized to empty. */
	struct sigaction SIGINT_action = {0};
	struct sigaction SIGUSR2_action = {0};
	struct sigaction ignore_action = {0};

		/* Define member variables of SIGINT_action struct. */
	SIGINT_action.sa_handler = catchSIGINT; // sa_handler is a function pointer to catchSIGINT.

	/* Register all signal types so none of them are allowed to interrupt with the processing
	 * of the SIGINT signal. */
	sigfillset(&SIGINT_action.sa_mask); // Set signal set to block to full, aka: block all signals.
	SIGINT_action.sa_flags = 0; // No flags.

		/* Define member variables of SIGUSR2_action struct. */
	SIGUSR2_action.sa_handler = catchSIGUSR2;
	sigfillset(&SIGUSR2_action.sa_mask);
	SIGUSR2_action.sa_flags = 0;

	ignore_action.sa_handler = SIG_IGN; // Ignore signals. 

	/* Next five calls actually register these things. */
	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGUSR2, &SIGUSR2_action, NULL);
	sigaction(SIGTERM, &ignore_action, NULL);
	sigaction(SIGHUP, &ignore_action, NULL);
	sigaction(SIGQUIT, &ignore_action, NULL);

	printf("SIGTERM, SIGHUP, and SIGQUIT are disabled. \n");
	printf("Send a SIGUSR2 signal to kill this program. \n");
	printf("Send a SIGINT signal to sleep 5 seconds, then kill this program. \n");

	while(1)
	{
		pause();
	}

}

