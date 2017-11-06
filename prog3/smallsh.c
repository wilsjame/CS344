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
int  directCommand(char* userInput);
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

		return; //for testing

		//switch(direcCommandt(userInput)
		//	Built In
		//case 0: exit()
		//case 1: cd(); break;
		//case 2: status(); break;
		//	Non Built In
		//default: ...; break;
		//
	}

	/* Rough Outline */
	//main loop will get user input "The Prompt"
	//
	//prompt user in the correct format
	//get an input from the user
	//store the input in a variable
	//
	// In detail..
	// loop continously.. while(1)
	//
	// loop until a command is entered
	// do 
	// create command line prompt
	// 	use a colon :
	// 	flush output buffers after every print using fflush()
	// 	assume command is words made up of spaces
	// Get input from user
	// while is command is false
	// 	Check if,
	// 	blank line -> FALSE do nothing and reprompt
	// 	comment -> FALSE do nothing and reprompt
	//
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
	bool isCommand = false;

	if(strlen(userInput) == 0)
	{
		printf("blank line entered!\n");
	}
	else if(strncmp(userInput, "#", 1) == 0)
	{
		printf("comment entered!\n");
	}
	else
	{
		printf("command entered!\n");
		isCommand = true;
	}

	return isCommand;

}

