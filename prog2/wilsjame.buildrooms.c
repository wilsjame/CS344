/*********************************************************************
** Author: James G Wilson
** Date: 24 October 2017
** Description: Program 2 CS 344
**
** wilsjame.buildrooms.c generates 7 different room files, one room
** per file, in a directory called "wilsjame.rooms.<PROCESS ID>". 
** Out of the 10 names hard coded in this program, 7 will be chosen
** randomly as the names of the different rooms. 
** 
** Each room will randomly have 3-6 outgoing connections and a room
** type. The connections are both ways, ex) A connects to B, then B
** connects to A. The room types are: START_ROOM, END_ROOM, and
** MID_ROOM. Naturally, there is only one START and END room. 
*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define ROOMPOOL 10 // Number of rooms to choose from.
#define MAXROOMS 7
#define MAXCONNECTIONS 6

struct Room
{
	char type[250];
	char name[250];
	int numOutboundConnections;
	struct Room* roomConnections[MAXCONNECTIONS];
};

/* Useful functions :-) */
static void createRoomDir(struct Room* array);
static void shuffle(char (*array)[250], int n);

/* B^2 room functions */
static bool IsGraphFull(struct Room* array);
static void AddRandomConnection(struct Room* array);
static struct Room* GetRandomRoom(struct Room* array);
static bool CanAddConnectionFrom(struct Room* x);
static bool ConnectionAlreadyExists(struct Room* x, struct Room* y);
static void ConnectRoom(struct Room* x, struct Room* y);
static bool IsSameRoom(struct Room* x, struct Room* y);

int main(void)
{

	int i; // General use iterator
	struct Room rooms[MAXROOMS]; // Array of MAXROOMS (blank) Room structs 

	/* 2D array of roomNames[NUMBER_OF_STRINGS][STRING_LENGTH]. */
	char roomNames[ROOMPOOL][250] = 
	{
		"one", "two", "three", 
		"four", "five", "six",
		"seven", "eight", "nine",
		"ten"
	};

	/* Shuffle the names. */
	srand(time(NULL));
	shuffle(roomNames, ROOMPOOL);
	
	/* Assign names types to rooms. */
	for(i = 0; i < MAXROOMS; i++)
	{
		memset(rooms[i].name, '\0', 250);
		strcpy(rooms[i].name, roomNames[i]);

		if(i == 0)
		{
			strcpy(rooms[i].type, "START_ROOM");
		}
		else if(i == MAXROOMS - 1)
		{
			strcpy(rooms[MAXROOMS - 1].type, "END_ROOM");
		}
		else
		{
			strcpy(rooms[i].type, "MID_ROOM");
		}

	}

	/* Create all connections in graph. */
	while (IsGraphFull(rooms) == false)
	{
		AddRandomConnection(rooms);
	}

	/* Output each room's info as a file to the proper directory */
	createRoomDir(rooms);
	
	return 0;

}

// TODO Poo Putt Platter of Stringhetti oh my! Clean this function up.
/* Creates the room directory, wilsjame.rooms.<PROCESS ID>/. */
static void createRoomDir(struct Room* array)
{
	int roomItr;
	int connectionItr;
	int PID = getpid();

	char connectionItrString[250]; memset(connectionItrString, '\0', 250);
	char PIDString[250]; memset(PIDString, '\0', 250);
	char fileName[250]; memset(fileName, '\0', 250);
	char nameTitle[250]; memset(nameTitle, '\0', 250);
	char connectionTitle[250]; memset(connectionTitle, '\0', 250);
	char typeTitle[250]; memset(typeTitle, '\0', 250);
	char dirName[250] = "wilsjame.rooms.";
	char fileExtension[250] = "_room";

	FILE* fileStream;

	/* Convert the PID to a string. */
	sprintf(PIDString, "%d", PID);

	/* Complete directory name. */
	strcat(dirName, PIDString);

	/* mkdir() returns 0 upon successful completion, -1 otherwise. */
	if(mkdir(dirName, 0755) != 0)
	{
		printf("mkdir() failed :(\n");
		exit(1);
	}
	
	/* Write room data to files. */
	for(roomItr = 0; roomItr < MAXROOMS; roomItr++)
	{

		/* Set row titles. */
		strcpy(nameTitle, "ROOM NAME: "); 
		strcpy(typeTitle, "ROOM TYPE: ");

		/* Create file destination. */
		strcat(fileName, "./");
		strcat(fileName, dirName);
		strcat(fileName, "/");
		
		/* Open file for writing. */
		strcat(fileName, array[roomItr].name);
		strcat(fileName, fileExtension);
		fileStream = fopen(fileName, "w");

		/* Write room name. */
		strcat(nameTitle, array[roomItr].name);
		fprintf(fileStream, "%s\n", nameTitle);

		/* Write room connections. */
		for(connectionItr = 0; connectionItr < array[roomItr].numOutboundConnections; connectionItr++)
		{

			/* Set row title. */
			strcpy(connectionTitle, "CONNECTION ");

			/* Convert connection # to a string. */
			sprintf(connectionItrString, "%d", connectionItr + 1);
			strcat(connectionItrString, ": ");
			strcat(connectionTitle, connectionItrString);

			/* Write room connection. */
			strcat(connectionTitle, array[roomItr].roomConnections[connectionItr]->name);
			fprintf(fileStream, "%s\n", connectionTitle);

			/* Clear strings for next loop iteration. */
			memset(connectionItrString, '\0', 250);
			memset(connectionTitle, '\0', 250);
		}

		/* Write room type. */
		strcat(typeTitle, array[roomItr].type);
		fprintf(fileStream, "%s\n", typeTitle);

		/* Clear strings for next loop iteration */
		memset(fileName, '\0', 250);
		memset(nameTitle, '\0', 250);
		memset(typeTitle, '\0', 250);

		/* Close file until next loop iteration. */
		fclose(fileStream);
	}

}

/* Arrange n elements of array in random order. */
static void shuffle(char (*array)[250], int n)
{
	char temp[250];
	memset(temp, '\0', 250);
	
	if(n > 1)
	{
		int i;

		for(i = 0; i < n - 1; i++)
		{
			int j = i + rand() / (RAND_MAX / (n - i) + 1);
			strcpy(temp, array[j]);
			strcpy(array[j], array[i]);
			strcpy(array[i], temp);
		}

	}

	return;

}

/* Returns true if all rooms have 3 to 6 outbound connections, false otherwise. */
static bool IsGraphFull(struct Room* array)
{
	int i;

	for(i = 0; i < MAXROOMS; i++)
	{

		if(array[i].numOutboundConnections < 3)
		{
			
			return false;

		}

	}

	return true;

}

/* Adds a random, valid outbound connection from a Room to another Room. */
static void AddRandomConnection(struct Room* array)
{

	/* Temporary pointers to the structs in rooms array */
	struct Room* roomA;
	struct Room* roomB;

	do
	{
		roomA = GetRandomRoom(array);
		roomB = GetRandomRoom(array);

	/* Keep getting two random rooms until pair is valid. */
	}while(CanAddConnectionFrom(roomA) == false || 
		CanAddConnectionFrom(roomB) == false ||
		ConnectionAlreadyExists(roomA, roomB) == true ||
		IsSameRoom(roomA, roomB) == true); 

	/* Now we have two rooms able to connect */
	ConnectRoom(roomA, roomB);
	
	return;

}

/* Returns a random Room, does NOT validate if connection can be added. */
static struct Room* GetRandomRoom(struct Room* array)
{
	int randomIndex = rand() % MAXROOMS;

	return &array[randomIndex];

}

/* Returns true if a connection can be added from Room x (< MAXCONNECTIONS outbound connections), false otherwise */
static bool CanAddConnectionFrom(struct Room* x)
{

	if(x->numOutboundConnections < MAXCONNECTIONS)
	{

		return true;

	}
	else
	{

		return false;

	}

}

/* Returns true if a connection from Room x to Room y already exists, false otherwise. */
static bool ConnectionAlreadyExists(struct Room* x, struct Room*  y)
{
	int i;
	
	for(i = 0; i < MAXCONNECTIONS; i++)
	{

		if(IsSameRoom(x->roomConnections[i], y))
		{

			return true;

		}

	}

	return false;

}

/* Connects Rooms x and y together, does not check if this connection is valid. */
static void ConnectRoom(struct Room* x, struct Room* y)
{
	x->roomConnections[x->numOutboundConnections] = y;
	x->numOutboundConnections++;
	y->roomConnections[y->numOutboundConnections] = x;
	y->numOutboundConnections++;

	return;
	
}

/* Returns true if Rooms x and y are the same Room, false otherwise. */
static bool IsSameRoom(struct Room* x, struct Room* y)
{

	if(x == y)
	{

		return true;

	}
	else
	{

		return false;

	}

}
