/*********************************************************************
** Author: James G Wilson
** Date: 25 October 2017
** Description: Program 2 CS 344
**
** wilsjame.adventure.c
**
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#define MAXROOMS 7
#define MAXCONNECTIONS 6

struct Room
{
	char type[250];
	char name[250];
	int numOutboundConnections;
	char roomConnections[MAXCONNECTIONS][250];
};

/* Useful functions =^D */
static void getRoomDir(char* roomDirName);
static void getRoomData(struct Room* array, char* roomDirName);
static void getUserInput(char* userInput);
static int getStartRoomIndex(struct Room* array);
static void displayConnections(struct Room* array, int currentLocation);
static void writeTime();

//TODO
//add time feature using threads
//check output is correct format
//modularize game loop

int main(void)
{
	int i; // General use iterator
	int currentLocation;
	bool validInput;
	int stepCount = 0;

	struct Room rooms[MAXROOMS]; // Array of MAXROOMS (blank) Room structs
	char roomDirName[250]; memset(roomDirName, '\0', sizeof(roomDirName)); 
	char userInput[250]; memset(userInput, '\0', sizeof(userInput));
	char pathHistory[250]; memset(pathHistory, '\0', sizeof(pathHistory));
	
	/* Get the room subdirectory's name. */
	getRoomDir(roomDirName);

	/* Parse room files and store room data in rooms struct array. */
	getRoomData(rooms, roomDirName);

	/* Display starting location. */
	currentLocation = getStartRoomIndex(rooms);
	printf("CURRENT LOCATION: %s\n", rooms[currentLocation].name);
	displayConnections(rooms, currentLocation);

	/* Begin game loop. */ 
	while(1)
	{

		/* Reset before each iteration. */
		validInput = false;

		/* Have user input a valid connection. */
		while(validInput == false)
		{
			getUserInput(userInput);

			/* Check all outbound connections from the current room. */
			for(i = 0; i < rooms[currentLocation].numOutboundConnections; i++)
			{

				if(strcmp(userInput, rooms[currentLocation].roomConnections[i]) == 0)
				{
					validInput = true;
				}

			}

			if(validInput == false)
			{
				printf("\n\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n\n");
			}

		}

		/* Move current location to chosen room. */
		for(i = 0; i < MAXROOMS; i++)
		{

			/* Find room that matches the users valid next room choice. */
			if(strcmp(userInput, rooms[i].name) == 0)
			{

				/* Enter the room */
				currentLocation = i;

				/* Incrememnt steps taken. */
				stepCount++;

				/* Update path history. */
				strcat(pathHistory, userInput);
				strcat(pathHistory, "\n");

				/* Check if end room. */
				if(strcmp("END_ROOM", rooms[currentLocation].type) == 0)
				{
					printf("\n\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
					printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n%s", stepCount, pathHistory);

					return 0;

				}

			}

		}

		/* Set up menu for another move. */
		printf("CURRENT LOCATION: %s\n", rooms[currentLocation].name);

		/* Display possible connections. */
		displayConnections(rooms, currentLocation);
	}

}

/* Assigns the name of the most recent rooms subdirectory to the input string. */
static void getRoomDir(char* newestDirName)
{
	int newestDirTime = -1; // Modified timestamp of newest subdir examined.
	char targetDirPrefix[32] = "wilsjame.rooms."; // Prefix to look for.

	/* Create directory stream. */
	DIR* dirToCheck; 

	/* dirent holds directory entires returned by readdir(). */
	struct dirent* fileInDir; 

	/* stat holds information about a named file (subdir in this case) */
	struct stat dirAttributes; 

	/* Open a directory stream of directory this program was run in. */
	dirToCheck = opendir("."); 

	/* Step through it. */
	if(dirToCheck > 0) // Make sure current directory could be opened.
	{

		/* Check each entry in directory. */
		while((fileInDir = readdir(dirToCheck)) != NULL) 
		{

			/* Ignore directory pointers. */
			if(!strcmp(fileInDir->d_name, "."))
			{
				continue;
			}

			if(!strcmp(fileInDir->d_name, ".."))
			{
				continue;
			}

			/* Check if the entry has the prefix. */
			if(strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{

				/* Get attributes of entry into stat struct. */
				stat(fileInDir->d_name, &dirAttributes);
			}

			/* Check for more recent timestamp. */
			if((int)dirAttributes.st_mtime > newestDirTime)
			{
				newestDirTime = (int)dirAttributes.st_mtime;
				memset(newestDirName, '\0', sizeof(newestDirName));
				strcpy(newestDirName, fileInDir->d_name);
			}

		}

	}

	/* Close directory stream. */
	closedir(dirToCheck);

	return;
	
}

/* Parses room files in subdirectory and stores data in rooms array. */
static void getRoomData(struct Room* array, char* roomDirName)
{
	int roomItr = 0;
	int connectionItr;

	FILE* fileStream;

	char roomFilePath[250]; memset(roomFilePath, '\0', sizeof(roomFilePath));
	char roomRow[250]; memset(roomRow, '\0', sizeof(roomRow));
	char rowTitle_1[250]; memset(rowTitle_1, '\0', sizeof(rowTitle_1));
	char rowTitle_2[250]; memset(rowTitle_2, '\0', sizeof(rowTitle_2));
	char roomInfo[250]; memset(roomInfo, '\0', sizeof(roomInfo));

	/* Complete room directory path name. */
	char roomDirNamePath[250] = "./";
	strcat(roomDirNamePath, roomDirName);

	/* Create directory stream. */
	DIR* dirToCheck; 

	/* dirent holds directory entires returned by readdir(). */
	struct dirent* fileInDir; 

	/* Open a directory stream of directory this program was run in. */
	dirToCheck = opendir(roomDirNamePath); 

	/* Step through it. */
	if(dirToCheck > 0) // Make sure current directory could be opened.
	{

		/* Check each entry in directory. */
		while((fileInDir = readdir(dirToCheck)) != NULL) 
		{

			/* Ignore directory pointers. */
			if(!strcmp(fileInDir->d_name, "."))
			{
				continue;
			}

			if(!strcmp(fileInDir->d_name, ".."))
			{
				continue;
			}

			/* Create room file path name. */
			strcpy(roomFilePath, "./");
			strcat(roomFilePath, roomDirName);
			strcat(roomFilePath, "/");
			strcat(roomFilePath, fileInDir->d_name);

			/* Open room file for reading. */
			fileStream = fopen(roomFilePath, "r");

			/* Reset room connection iterator. */
			connectionItr = 0;

			/* Parse each line of room file. */
			while(fgets(roomRow, sizeof(roomRow), fileStream) != NULL)
			{
				
				/* Store contents of line into seperate strings. */
				sscanf(roomRow, "%s %s %s", rowTitle_1, rowTitle_2, roomInfo);

				/* Determine room name, type, or connections. */
				if(strcmp("ROOM", rowTitle_1) == 0) 
				{

					if(strcmp("NAME:", rowTitle_2) == 0)
					{

						/* Store room's name. */
						memset(array[roomItr].name, '\0', 250);
						strcpy(array[roomItr].name, roomInfo);
					}
					else
					{

						/* Store room's type. */
						memset(array[roomItr].type, '\0', 250);
						strcpy(array[roomItr].type, roomInfo);
					}

				}
				else
				{

					/* Store room's connections. */
					memset(array[roomItr].roomConnections[connectionItr], '\0', 250);
					strcpy(array[roomItr].roomConnections[connectionItr], roomInfo);

					/* Keep a running connection count. */
					array[roomItr].numOutboundConnections++;
					connectionItr++;
				}

				/* Clear string for next line parse. */
				memset(roomRow, '\0', sizeof(roomRow));
				memset(rowTitle_1, '\0', sizeof(rowTitle_1));
				memset(rowTitle_2, '\0', sizeof(rowTitle_2));
				memset(roomInfo, '\0', sizeof(roomInfo));
			}
			
			/* Keep a running room count. */
			roomItr++;

			/* Clear string(s) for next loop iteration. */
			memset(roomFilePath, '\0', sizeof(roomFilePath));

			/* Close file until next loop iteration. */
			fclose(fileStream);
		}

	}

	/* Close directory stream. */
	closedir(dirToCheck);

	return;

}

/* Gets and stores user input in a variable accessible in main. Does not validate. */
static void getUserInput(char* userInput)
{

	/* Clear any previous user inputs. */
	memset(userInput, '\0', sizeof(userInput));

	/* Magic settings
	 * size_t is unsigned integer type to hold result of sizeof(). */
	size_t bufferSize = 0; // Holds how large the allocated buffer is.
	char* lineEntered = NULL; // Points to a buffer allocated by getline() that holds entered string + \n + \0.

	/* Get input from user */
	printf("WHERE TO? >");

	/* Magic settings here, getline() buffers automatically use malloc. :^) */
	getline(&lineEntered, &bufferSize, stdin); // Get a line from the user.

	/* Remove trailing '\n' from pressing enter. */
	lineEntered[strcspn(lineEntered, "\n")] = 0;

	/* Store user input for use in main(). */
	strcpy(userInput, lineEntered);

	/* Free memory allocated by getline(), Free Lil B. */
	free(lineEntered);

	return;

}

/* Finds the starting room and returns its index in the room array. */
static int getStartRoomIndex(struct Room* array)
{
	int i;

	for(i = 0; i < MAXROOMS; i++)
	{

		if(strcmp("START_ROOM", array[i].type) == 0)
		{

			return i;

		}

	}

}

/* Displays the room connections given the current location index. */
static void displayConnections(struct Room* array, int currentLocation)
{
	int i;
	int numberOfConnections = array[currentLocation].numOutboundConnections;
	char possibleConnections[250]; memset(possibleConnections, '\0', sizeof(possibleConnections));;

	for(i = 0; i < numberOfConnections; i++)
	{

		if(i < numberOfConnections - 1)
		{

			/* Append connections seperated by commas. */
			strcat(possibleConnections, array[currentLocation].roomConnections[i]);
			strcat(possibleConnections, ", ");
		}
		else
		{

			/* Append last connection and end with a period ".". */
			strcat(possibleConnections, array[currentLocation].roomConnections[i]);
			strcat(possibleConnections, ".");
		}

	}

	printf("POSSIBLE CONNECTIONS: %s\n", possibleConnections);
	
	return;

}

/* Writes current time to an output file. */
static void writeTime()
{
	FILE* fileStream;
	char fileName[250] = "./currentTime.txt";
	char buffer[250]; memset(buffer, '\0', sizeof(buffer));

	/* Get time in formatted string. */
	time_t rawtime;
	struct tm *info;

	time(&rawtime);

	info = localtime(&rawtime);

	strftime(buffer, 250, "%I:%M%p, %A %d, %Y", info);

	/* Write time to file. */
	fileStream = fopen(fileName, "w");
	fprintf(fileStream, "%s", buffer);

	fclose(fileStream);

	return;

}
