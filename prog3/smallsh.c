/*********************************************************************
** Author: James G Wilson
** Date: 11/4/2017
** Description: Program 3 CS 344
** smallsh.h
*********************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* Function prototypes. */
void commandPrompt(char* userInput);
bool isCommand(char* userInput);
int  determineCommand(char* userInput);
void builtInExit(void);
void builtInCd(char* userInput);
void builtInStatus(void);
int formatCommand(char* userInput, char* args[]);
bool isBackground(int numArguments, char* arg[]);
void execute(char* args[]);

int main()
{
	char userInput[2048];
	char* args[512]; 
	bool isBuiltIn;
	int childExitMethod;
	int numArguments;
	pid_t spawnPid;

	/* Children PID tracking array. TODO Consider making dynamic and encapsulate in struct. */
	int pidIdx= 0;
	pid_t trackingArray[250]; 

	/* Main loop prompts and gets input from user. */
	while(1)
	{

		/* Reset flag before every loop iteration. */
		isBuiltIn = true;

		/* Get a command from the user. */
		do
		{
			commandPrompt(userInput);
		}while(!(isCommand(userInput)));

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

			/* Store command and any arguments in args array. */
			numArguments = formatCommand(userInput, args);

			/* Is it a background or foreground process? */
			if(isBackground(numArguments, args))
			{

				/* Background process:
				 *
				 */

				printf("Its a background process!\n");

				
			}
			else
			{

				/* Foreground process:
				 * Execute command and have shell (parent) wait
				 * in a blocked state until child terminates. */

				printf("Its a foreground process!\n");
				spawnPid = fork(); 

				switch(spawnPid)
				{
					case -1:
						perror("fork() failure!\n");
						exit(1);
						break;
					case 0:
						//in child
						execute(args);
						break;
				}

				waitpid(spawnPid, &childExitMethod, 0); //0 -> Block
			}
			
			//TODO Background 
			//background
			//	execute command and print its process id
			//	store process id in an array
			//	shell returns command line control immediately after forking off the child
			//	have shell periodically check for bg processes to complete
			//		use waitpid(..NOHANG...)
			//	cleanup completed bg processes as shell continues to run
			//	check and print any completed bg pid and exit status just before new prompt
			//	remove completed bg processes from array
			
			//in parent TESTING
			//printf("Waiting for child %d to finish. Parent is blocked\n", spawnPid);
			/*
			pid_t childPid = waitpid(spawnPid, &childExitMethod, 0); //0 -> Block
			if(childPid == -1)
			{
				perror("Wait failed!");
				exit(1);
			}

			if(WIFEXITED(childExitMethod))
			{
				//printf("Child terminated normally\n");
				int exitStatus = WEXITSTATUS(childExitMethod);
				//printf("exit status was: %d\n", exitStatus);
			}
			else
			{
				printf("By deduction.. child terminated by signal\n");
			}
			*/

			//printf("Done waiting for child to finish\n");
			//END TESTING


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

	/* Temporary variable to hold first word in user input's command. */
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

/* Stores command name and arguments in an array and returns its size. */
int formatCommand(char* userInput, char* args[])
{
	int argItr = 0;
	memset(args, '\0', sizeof(args));

	/* First token is the command name. */
	args[argItr] = strtok(userInput, " ");
	
	/* Remaining tokens (if any) are additional arguments. */
	while(args[argItr] != NULL)
	{
		argItr++;
		args[argItr] = strtok(NULL, " ");
	}

	/* Set the last argument as NULL for exec(). */
	args[argItr + 1] = NULL;

	//TEST
	printf("argItr is: %d\n", argItr);
	int i;
	for(i = 0; i < argItr; i++)
	{
		printf("args[%d]: %s\n", i, args[i]);
	}
	//END TEST*/

	/* return the number of arguments in the array. */
	return argItr;

}

/* Returns true if the command is to be run in the background. */ 
bool isBackground(int numArguments, char* arg[])
{
	
	if(strcmp(arg[numArguments - 1], "&") == 0) 
	{

		return true;
		
	}
	else
	{

		return false;

	}

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

