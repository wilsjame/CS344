/*********************************************************************
** Author: James G Wilson
** Date: 11/4/2017
** Description: Program 3 CS 344
** smallsh.h
*********************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Function prototypes. */
void commandPrompt(char* userInput);
bool isCommand(char* userInput);
int  determineCommand(char* userInput);
void builtInExit(void);
void builtInCd(char* userInput);
void builtInStatus(void);
bool formatCommand(char* userInput, char* args[]);
void execute(char* args[]);
void orphanCleanup(int size, pid_t trackingArray[]);

int main()
{
	char userInput[2048];
	char* args[512]; 
	int numArguments;
	int childExitMethod;
	int trackerSize = 0;
	bool isBuiltIn;
	bool isBackground;
	pid_t trackingArray[250]; memset(trackingArray, '\0', 250); 
	pid_t spawnPid;			

	/* Main loop prompts and gets input from user. */
	while(1)
	{

		/* Reap any terminated bg processes,
		 * and prompt a command from the user. */ 
		do
		{
			orphanCleanup(trackerSize, trackingArray);
			commandPrompt(userInput);
		}while(!(isCommand(userInput)));

		/* Reset flag before every loop iteration. */
		isBuiltIn = true;

		/* Switch on built in commands and default to non built in. */
		switch(determineCommand(userInput))
		{
			case 0: 
				builtInExit();
				break;
			case 1:
				builtInCd(userInput);
				break;
			case 2:
				printf("call status\n");
				break;
			default:
				isBuiltIn = false;
				break;
		}

		if(!(isBuiltIn))
		{

			/* Store command and any arguments in args array,
			 * and determine if it's a background  process. */
			isBackground = formatCommand(userInput, args);
			
			/* Fork off child to execute the command. */
			spawnPid = fork(); 
			
			switch(spawnPid)
			{
				case -1:
					perror("fork() failure!\n");
					exit(1);
					break;
				case 0:		/* In child. */
					execute(args); /* Exec hollows out rest of child program. */
					break;
				default:	/* In parent. */
					break;
			}

			if(isBackground)
			{

				/* Background process:
				 * Print and store pid,
				 * immediately return 
				 * command line control.  */
				printf("background pid is %d\n", spawnPid);
				trackingArray[trackerSize++] = spawnPid;
			}
			else
			{

				/* Foreground process:
				 * Shell (parent) waits
				 * in a blocked state for 
				 * child termination. */
				waitpid(spawnPid, &childExitMethod, 0); // 0 -> Block 
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
int  determineCommand(char* userInput)
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
void builtInExit()
{
	//TODO
	//kill any processes or jobs that the shell has started 
	//consider using an array of PID's 
	
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

/* Stores command name and arguments in an array and returns 
 * true if the command is to be run in the background. */ 
bool formatCommand(char* userInput, char* args[])
{
	int argItr = 0;
	bool isBackground = false;
	memset(args, '\0', sizeof(args));

	/* First token is the command name. */
	args[argItr] = strtok(userInput, " ");
	
	/* Remaining tokens (if any) are additional arguments. */
	while(args[argItr] != NULL)
	{
		argItr++;
		args[argItr] = strtok(NULL, " ");
	}

	//printf("args[argItr - 1] is: %s\n", args[argItr - 1]);

	/* Check if it's a background process. */
	if(strcmp(args[argItr - 1], "&") == 0) 
	{

		/* Remove '&' and set last 
		 * argument NULL for exec(). */
		args[argItr - 1] = NULL;
		isBackground = true;
	}

	return isBackground;

}

/* Lil' function to encapsulate execvp(). */
void execute(char* args[])
{

	/* *args == args[0]. */
	if(execvp(*args, args) < 0)
	{
		perror("Exec failure!");
		exit(1);
	}

}

//	have shell periodically check for bg processes to complete
			//		use waitpid(..NOHANG...)
			//	cleanup completed bg processes as shell continues to run
			//	check and print any completed bg pid and exit status just before new prompt
			//	remove completed bg processes from array
void orphanCleanup(int size, pid_t trackingArray[])
{

	int i, childExitMethod;;

	//iterate through array 
	for(i = 0; i < size; i++)
	{
		//WNOHANG return immediately if no child has exited
		pid_t childPid = waitpid(trackingArray[i], &childExitMethod, WNOHANG);

		if(childPid == -1)
		{
			//perror("Wait failed in deathCleanup()!");
			//TODO Remove completed bg ps from array, make array dynamic in nature?
		}

		if(WIFEXITED(childExitMethod))
		{
			//child terminated normally
			int exitStatus = WEXITSTATUS(childExitMethod);
			printf("background pid %d is done: exit value %d\n", trackingArray[i], exitStatus);
		}
		else 
		{
			//by deduction.. child must've been terminated by signal
		
		}

	}

	return;

}

