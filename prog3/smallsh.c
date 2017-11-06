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

/* Function prototypes. */
void commandPrompt(char* userInput);
bool isCommand(char* userInput);
int  determineCommand(char* userInput);
void exit();
void cd();
void status();

int main()
{

	/* Global-like user input string. */
	char userInput[2048];

	/* Main loop prompts and gets input from user. */
	while(1)
	{
		
		/* Get a command from the user. */
		do
		{
			commandPrompt(userInput);
		}while(isCommand(userInput) == false);

		determineCommand(userInput);

		//switch(determineCommand(userInput)
		//	Built In
		//case 0: exit()
		//case 1: cd(); break;
		//case 2: status(); break;
		//	Non Built In
		//default: ...; break;
		//
	}

	// Now we have a command
	//  switch on builtInCommads
	//  built in commands: exit, cd, (status is built-in but don't worry bout it yet)
	// 
	//  0 exit
	//  1 cd
	//  	get back to beginning of loop
	//  2 status
	//  	get back to beginning of loop
	//  3 default do nothing and carry on
	// 
	// Now we have a non built-in command...
	// do stuff...
	// get back to beginnin of loop
	//  

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

//switch(directCommand(userInput)
		//	Built In
		//case 0: exit()
		//case 1: cd(); break;
		//case 2: status(); break;
		//	Non Built In
		//default: ...; break;
		//

/* Built-in commands return 0, 1, or 2 otherwise return 3. */
int  determineCommand(char* userInput)
{
	/* Temporary variable to hold first word in user input's command. */
	char* command[250]; memset(command, '\0', sizeof(command));

	/* Store the first word in command. */
	sscanf(userInput, "%s", command);

	printf("The first word of the command line is: %s\n", command);

	return 3;

}

