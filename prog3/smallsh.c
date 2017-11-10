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
<<<<<<< HEAD
void formatCommand(char* userInput, char* args[]);
=======
>>>>>>> parent of 6bda467... outline format non built in command function

int main()
{
	char userInput[2048];
<<<<<<< HEAD
	char* args[512];
=======
>>>>>>> parent of 6bda467... outline format non built in command function
	bool isBuiltIn;
	pid_t spawnPid = -5; //initialize to dummy value to trace any bugs

	/* Children PID tracking array. Consider making dynamic. */
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
		}while(isCommand(userInput) == false);

		/* Switch on built in commands and default to non built. */
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

		if(isBuiltIn != true)
		{
<<<<<<< HEAD
			//formatCommand(userInput, command, argVector);
			formatCommand(userInput, &args);

			/*
			spawnPid = fork(); 
=======
			spawnPid = fork();
>>>>>>> parent of 6bda467... outline format non built in command function

			switch(spawnPid)
			{
				case -1:
					perror("fork() failure!\n");
					exit(1);
					break;
				case 0:
					printf("I am the child!\n");
					break;
				default:
					printf("I am the parent!\n");
					break;
			}
			*/

		}

	} //end of while(1)

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

<<<<<<< HEAD
/* Parses user input and stores the command name and arguments. */
void formatCommand(char* userInput, char* args[])
{
	int argItr = 0;

	memset(command->commandName, '\0', sizeof(command->commandName));
	memset(command->argVector, '\0', sizeof(command->argVector));

	/* First token is the command name. */
	strcpy(command->commandName, strtok(userInput, " "));
	
	/* Remaining tokens (if any) are additional arguments. */
	do
	{
		command->argVector[argItr] = strtok(NULL, " ");
	}while(command->argVector[argItr++] != NULL);
/*
	while(command->argVector[argItr] != NULL)
	{
		command->argVector[++argItr] = strtok(NULL, " ");
	}
*/
	printf("commandName is: %s\n", command->commandName);
	printf("argItr is: %d\n", argItr);
	int i;
	for(i = 0; i < argItr; i++)
	{
		printf("command->argVector[%d]: %s\n", i, command->argVector[i]);
	}

	return;

}

=======
>>>>>>> parent of 6bda467... outline format non built in command function
