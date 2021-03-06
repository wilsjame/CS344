/*********************************************************************
** Author: James G Wilson
** Date: 11/16/2017
** Description: Program 3 CS 344
** smallsh.h
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

/* Encapsulate IO redirection. */
struct redirect
{
	char stdinRedirect[250];
	char stdoutRedirect[250];
};

/* Function prototypes. */
void commandPrompt(char* userInput);
bool isCommand(char* userInput);
int  determineCommand(char* userInput);
void builtInExit(int size, pid_t trackingArray[]);
void builtInCd(char* userInput);
void builtInStatus(int exitMethod);
void remove_element(char* args[], int index, int size);
bool formatCommand(char* userInput, char* args[], struct redirect*);
void redirect(struct redirect*, bool isBackground);
void execute(char* args[]);
void checkTerminationStatus(int exitMethod);
void orphanCleanup(int size, pid_t trackingArray[]);

/* Signal handling trap. */
bool bgON = true; /* Global. Mr. Worldwide lool. */
void catchSIGTSTP(int signo) /* CTRL+Z toggles fg only mode. */
{

	if(bgON == true)
	{
		char* message = "Entering foreground-only mode (& is now ignored)\n: ";
		write(STDOUT_FILENO, message, 51);
		fflush(NULL);
		bgON = false;
	}
	else if(bgON == false)
	{
		char* message = "Exiting foreground-only mode\n: ";
		write(STDOUT_FILENO, message, 31);
		fflush(NULL);
		bgON = true;
	}
			
}

int main()
{
	char userInput[2048];
	char* args[512]; 
	int numArguments;
	int childExitMethod;
	int trackerSize = 0;
	bool isBuiltIn;
	bool isBackground;
	struct redirect inOut;
	pid_t trackingArray[250]; memset(trackingArray, '\0', 250); 
	pid_t spawnPid;			

	/* Signal handling. */
	struct sigaction SIGTSTP_action = {0};
	struct sigaction ignore_action = {0};
	struct sigaction default_action = {0};

	SIGTSTP_action.sa_handler = catchSIGTSTP;
	SIGTSTP_action.sa_flags = SA_RESTART;
	ignore_action.sa_handler = SIG_IGN;
	default_action.sa_handler = SIG_DFL;

	/* Register handlers. Note: children inherit. */
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);
	sigaction(SIGINT, &ignore_action, NULL); 
	
	/* Main loop prompts input from user among other things. */
	while(1)
	{

		/* Reap any terminated bg processes,
		 * and prompt a command from the user. */ 
		do
		{
			orphanCleanup(trackerSize, trackingArray);
			commandPrompt(userInput);
		}while(!(isCommand(userInput)));

		/* Reset flag every loop iteration. */
		isBuiltIn = true;

		/* Switch on built in commands and default to non built in. */
		switch(determineCommand(userInput))
		{
			case 0: 
				builtInExit(trackerSize, trackingArray);
				break;
			case 1:
				builtInCd(userInput);
				break;
			case 2:
				builtInStatus(childExitMethod);
				break;
			default:
				isBuiltIn = false;
				break;
		}

		if(!(isBuiltIn))
		{

			/* Store command and any arguments in args array. 
			 * Store any redirection files in inOut struct.
			 * Determine if it's a background  process. */
			isBackground = formatCommand(userInput, args, &inOut);

			/* Fork off child to execute the command. */
			spawnPid = fork(); 
			
			switch(spawnPid)
			{
				case -1:
					perror("fork() failure!\n");
					exit(1);
					break;
				case 0:		/* In child. */

					/* Ignore SIGTSTP and enable
					 * SIGINT if foreground. */
					sigaction(SIGTSTP, &ignore_action, NULL);

					if(!isBackground)
					{
						sigaction(SIGINT, &default_action, NULL);
					}

					/* Perform any necessary I/O redirection. */
					redirect(&inOut, isBackground);

					/* Exec hollows out rest of child program. */
					execute(args);
					break;
				default:	/* In parent. */
					break;
			}

			if(isBackground && bgON)
			{

				/* Background process:
				 * Print and store pid,
				 * immediately return 
				 * command line control.  */
				printf("background pid is %d\n", spawnPid);
				fflush(NULL);
				trackingArray[trackerSize++] = spawnPid;
			}
			else
			{

				/* Foreground process:
				 * Shell (parent) waits
				 * in a blocked state for 
				 * child termination. */
				waitpid(spawnPid, &childExitMethod, 0); // 0 -> Block 
				checkTerminationStatus(childExitMethod);
				
			}

		} // End of non built in command block.

	} // End of while(1).

	return 0;

}

/* Displays command line prompt and gets user input. No error checking. */
void commandPrompt(char* userInput)
{

	/* First things first, clear user input string. */
	memset(userInput, '\0', sizeof(userInput));

	/* Magic settings. */
	size_t bufferSize = 0;
	char* lineEntered = NULL;
	
	/* Display prompt and then flush all open output steams. */
	printf(": ");
	fflush(NULL);

	/* By magic settings, getline() buffers automatically use malloc. */
	getline(&lineEntered, &bufferSize, stdin);

	/* Remove trailing '\n' from pressing enter. */
	lineEntered[strcspn(lineEntered, "\n")] = 0;

	/* Store user input for use in main(). */
	strcpy(userInput, lineEntered);

	/* Free memory allocated by getline() aka free Lil B. */
	free(lineEntered);

	return;

};

/* Returns false if userInput is blank or a comment, true otherwise. */
bool isCommand(char* userInput)
{
	bool isCommand;

	if(strlen(userInput) == 0 || strncmp(userInput, "#", 1) == 0)
	{
		isCommand = false;
	}
	else
	{
		isCommand = true;
	}

	return isCommand;

}

/* Built-in commands return 0, 1, or 2 otherwise return 3. */
int determineCommand(char* userInput)
{

	/* Temporary variable to hold first 
	 * word in user input's command. */
	char command[250]; memset(command, '\0', sizeof(command));

	/* Store the first word as command. */
	sscanf(userInput, "%s", command);

	if(strcmp(command, "exit") == 0)
	{

		return 0;

	}
	else if(strcmp(command, "cd") == 0)
	{

		return 1;

	}
	else if(strcmp(command, "status") == 0)
	{

		return 2;

	}
	else
	{

		return 3;

	}

}

/* Kill other processes or jobs started by the shell then terminate itself. */
void builtInExit(int size, pid_t trackingArray[])
{
	int i;

	/* Parse bg process tracking array and kill. */
	for(i = 0; i < size; i++)
	{
		kill(trackingArray[i], SIGKILL);
	}

	exit(EXIT_SUCCESS);

}

/* Changes directories. By itself, change to the directory specified in 
 * the HOME enviornment variable. Can also take one argument, either the
 * absolute or relative path of the directory to change to. */
void builtInCd(char* userInput)
{
	
	if(strcmp(userInput, "cd") == 0)
	{

		if(chdir(getenv("HOME")) != 0)
		{
			printf("chdir(HOME) failure!\n");
		}

	}
	else
	{

		/* Temporary variable to hold directory path. */
		char path[250]; memset(path, '\0', sizeof(path));

		/* Store the specified path. */
		sscanf(userInput, "%*s %s", path);

		if(chdir(path) != 0)
		{
			printf("chdir(path) failure!\n");
		}

	}

	return;

}

/* Print exit status or terminating signal of the last foreground
 * process (not both, ps killed by signals don't have exit statuses!). */
void builtInStatus(int exitMethod)  
{

	if(WIFEXITED(exitMethod))
	{

		/* Child terminated normally. */
		printf("Foreground exit value %d\n", WEXITSTATUS(exitMethod));
		fflush(NULL);
	}
	else 
	{
		/* By deduction, child must've terminated by signal. */
		printf("Terminated by signal %d\n", WTERMSIG(exitMethod));
		fflush(NULL);
	}

	return;
}
/* Removes an element from string array. */
void remove_element(char* array[], int index, int size)
{
	int i;
	size++;

	for(i = index; i < size - 1; i++)
	{
		array[i] = array[i + 1];
	}

}

/* Formats command name and arguments in an array,
 * extracts any stdin or stdout redirection, and returns
 * true if the command is to be run in the background. */ 
bool formatCommand(char* userInput, char* args[], struct redirect* inOut)
{
	int index, argItr, argSize = 0;
	pid_t PID = getpid();
	bool isBackground = false;
	char PIDString[250]; memset(PIDString, '\0', 250);
	char tempArgument[250]; memset(tempArgument, '\0', 250);
	char expandTemp[250]; memset(expandTemp, '\0', 250);

	memset(args, '\0', sizeof(args));
	memset(inOut->stdinRedirect, '\0', 250);
	memset(inOut->stdoutRedirect, '\0', 250);

	/* First token is the command name. */
	args[argSize] = strtok(userInput, " ");
	
	/* Remaining tokens (if any) are additional arguments. */
	while(args[argSize] != NULL)
	{
		argSize++;
		args[argSize] = strtok(NULL, " ");

		/* Cool trick to expand $$ from Piazza. */
		memset(tempArgument, '\0', 250);
		memset(expandTemp, '\0', 250);

		strcpy(tempArgument, args[argSize - 1]);

		if(strstr(tempArgument, "$$") != NULL)
		{
			index = strstr(tempArgument, "$$") - tempArgument;
			tempArgument[index] = '%';

			index = strstr(tempArgument, "$") - tempArgument;
			tempArgument[index] = 'd';

			sprintf(expandTemp, tempArgument, getpid());
			strcpy(args[argSize - 1], expandTemp);
		}

	}

	/* Check if it's a background process. */
	if(strcmp(args[argSize - 1], "&") == 0) 
	{

		/* Remove '&' and set last 
		 * argument NULL for exec(). */
		args[--argSize] = NULL;
		isBackground = true;
	}

	/* Parse arguments for I/O redirection. */
	for(argItr = 0; argItr < argSize; argItr++)
	{

		if(strcmp(args[argItr], "<") == 0 || strcmp(args[argItr], ">") == 0) 
		{

			/* The next argument is the redirection file. */
			if(strcmp(args[argItr], "<" ) == 0)
			{
				strcpy(inOut->stdinRedirect, args[argItr + 1]);
			}
			else if(strcmp(args[argItr], ">" ) == 0)
			{
				strcpy(inOut->stdoutRedirect, args[argItr + 1]);
			}

			/* Remove '<' or '>' and 'input_file' from arguments. */
			remove_element(args, argItr, argSize--);
			remove_element(args, argItr, argSize--);
			argItr--;
		}

	}

	return isBackground;

}

//TODO condense logic
/* Perform I/O redirection. */
void redirect(struct redirect* inOut, bool isBackground)
{
	int sourceFD;
	int targetFD;

	if(!strcmp(inOut->stdinRedirect, "\0") == 0)
	{

		/* Input file open read only. */
		sourceFD = open(inOut->stdinRedirect, O_RDONLY);

		/* Cannot open, print error and set exit status to 1 (but don't exit shell). */
		if(sourceFD == -1) 
		{
			printf("IO redirection failure!\n");
			exit(1); // Child exits but not parent!
		}

		/* Perform redirection. */
		dup2(sourceFD, 0);
	}

	if(!strcmp(inOut->stdoutRedirect, "\0") == 0)
	{

		/* Output file open write only, truncate if it exists, create otherwise. */
		targetFD = open(inOut->stdoutRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);

		/* Cannot open, print error and set exit status to 1 (but don't exit shell). */
		if(targetFD == -1) 
		{
			printf("IO redirection failure!\n");
			exit(1); // Child exits but not parent!
		}

		/* Perform redirection. */
		dup2(targetFD, 1);
	}

	/* Background processes with no redirection redirect to /dev/null. */
	if(strcmp(inOut->stdinRedirect, "\0") == 0 && isBackground)
	{

		/* Input file open read only. */
		sourceFD = open("/dev/null", O_RDONLY);

		/* Cannot open, print error and set exit status to 1 (but don't exit shell). */
		if(sourceFD == -1) 
		{
			printf("IO redirection failure!\n");
			exit(1); // Child exits but not parent!
		}

		/* Perform redirection. */
		dup2(sourceFD, 0);
	}

	if(strcmp(inOut->stdoutRedirect, "\0") == 0 && isBackground)
	{

		/* Output file open write only, truncate if it exists, create otherwise. */
		targetFD = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);

		/* Cannot open, print error and set exit status to 1 (but don't exit shell). */
		if(targetFD == -1) 
		{
			printf("IO redirection failure!\n");
			exit(1); // Child exits but not parent!
		}

		/* Perform redirection. */
		dup2(targetFD, 1);
	}

	return;

}

/* Lil' function to encapsulate execvp(). */
void execute(char* args[])
{

	/* *args == args[0]. */
	if(execvp(*args, args) < 0)
	{
		printf("exec() failure!\n");
		fflush(NULL);
		exit(1);
	}

}

/* Prints termination signal, if any. */
void checkTerminationStatus(int exitMethod)
{

	if(WIFSIGNALED(exitMethod))
	{

		/* Child terminated by a signal. */
		printf("Terminated by signal %d\n", WTERMSIG(exitMethod));
		fflush(NULL);
	}

	return;

}

/* Checks for completed bg processes, determined exit status,
 * reaps, and prints completed bg pid and exit status. */
void orphanCleanup(int size, pid_t trackingArray[])
{

	int i, childExitMethod;

	/* Parse bg process tracking array. */
	for(i = 0; i < size; i++)
	{

		/* WNOHANG return immediately if no child has exited. */
		pid_t waitpidReturn = waitpid(trackingArray[i], &childExitMethod, WNOHANG);

		if(waitpidReturn == trackingArray[i])
		{

			if(WIFEXITED(childExitMethod))
			{

				/* Child terminated normally. */
				printf("background pid %d is done: exit value %d\n", trackingArray[i], WEXITSTATUS(childExitMethod));
				fflush(NULL);

			}
			else 
			{

				/* Terminated by signal. */
				printf("Background pid %d terminated by signal %d\n", trackingArray[i], WTERMSIG(childExitMethod));
				fflush(NULL);
			}

		}

	}

	return;

}

