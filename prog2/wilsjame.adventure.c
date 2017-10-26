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

//TODO
// [DONE] read files into array of room structs
//design game loop structure
//design menu input functionality
//add time feature using threads

int main(void)
{
	int i; // General use iterator
	int currentLocation;
	int connectionItr;

	struct Room rooms[MAXROOMS]; // Array of MAXROOMS (blank) Room structs
	char roomDirName[250]; memset(roomDirName, '\0', sizeof(roomDirName)); 
	char userInput[250]; memset(userInput, '\0', sizeof(userInput));
	
	/* Get the room subdirectory's name. */
	getRoomDir(roomDirName);

	/* Parse room files and store room data in rooms struct array. */
	getRoomData(rooms, roomDirName);

	/* Display starting location. */
	currentLocation = getStartRoomIndex(rooms);
	printf("CURRENT LOCATION: %s\n", rooms[currentLocation].name);

	/* Display possible connections. */
	displayConnections(rooms, currentLocation);



	/* Begin game loop. */
	//TEST
	printf("rooms array data:\n");
	for(i = 0; i < MAXROOMS; i++)
	{
		printf("Room Name: %s\n", rooms[i].name);

		for(connectionItr = 0; connectionItr < rooms[i].numOutboundConnections; connectionItr++)
		{
			printf("CONNECTION: %s\n", rooms[i].roomConnections[connectionItr]);
		}

		printf("Room Type: %s\n", rooms[i].type);
	}

	return 0;
	
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

	printf("Newest rooms directory found is: %s\n", newestDirName);

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

