/*********************************************************************
** Author: James G Wilson
** Date: 11/4/2017
** Description: Program 3 CS 344
** smallsh.h
*********************************************************************/
#include <stdio.h>
#include <stdbool.h>

/* Function prototypes. */
void commandPrompt(char* userInput);
bool isCommand(char* userInput);
int  directCommand(char* userInput);
void exit();
void cd();
void status();

int main()
{
	printf("Greetings from smallsh!\n");
	//declare user input string

	/* Clean Outline */
	while(1)
	{
		//memset user input string
		do
		{
		//call commandPrompt(user input string)
		}while(//isCommand(userInput) is false)

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

