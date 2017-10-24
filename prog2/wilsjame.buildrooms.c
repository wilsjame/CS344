/*********************************************************************
** Author: James G Wilson
** Date: 23 October 2017
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
static void createRoomDir();
static void shuffle(char (*)[250], int n);

/* B^2 Room functions */
static bool IsGraphFull();
static void AddRandomConnection();
static struct Room GetRandomRoom();
static bool CanAddConnectionFrom(struct Room x);
static bool ConnectionAlreadyExists(struct Room* x, struct Room* y);
static void ConnectRoom(struct Room x, struct Room y);
static bool IsSameRoom(struct Room* x, struct Room* y);

int main(void)
{

	int i; // General use iterator.
	struct Room rooms[MAXROOMS]; // Array of seven (blank) Room structs 

	/* 2D array of roomNames[NUMBER_OF_STRINGS][STRING_LENGTH]. */
	char roomNames[10][250] = 
	{
		"one", "two", "three", 
		"four", "five", "six",
		"seven", "eight", "nine",
		"ten"
	};

	/* Shuffle the names. */
	srand(time(NULL));
	shuffle(roomNames, 10);
	
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

	//TEST 
	for(i = 0; i < MAXROOMS; i++)
	{
		printf("room #%d name: %s type: %s \n", i + 1, rooms[i].name, rooms[i].type);
	}

	/* Create all connections in graph. */
	/*
	while (IsGraphFull() == false)
	{
		AddRandomConnection();
	}
	*/

	// Now all connections are created
	// Output each room as a file to the proper directory
	createRoomDir();
	
	return 0;

}

/* Creates the room directory, wilsjame.rooms.<PROCESS ID>/. */
static void createRoomDir()
{
	int PID = getpid();
	char PIDString[250];
	memset(PIDString, '\0', 250);
	char dirName[250] = "wilsjame.rooms.";

	/* Convert the PID to a string */
	sprintf(PIDString, "%d", PID);

	/* Complete directory name */
	strcat(dirName, PIDString);

	/* mkdir() returns 0 upon successful completion, -1 otherwise. */
	if(mkdir(dirName, 0755) == 0)
	{
		return;
	}
	else
	{
		printf("mkdir() failed :(\n");
		return;
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
static bool isGraphFull()
{
	// Check number of rooms currently created
	// 	If its less than 7 return false
	// 	Else return true
}

/* Adds a random, valid outbound connection from a Room to another Room. */
static void AddRandomConnection()
{
	// Do
	// Get a random RoomA using GetRandomRoom()
	// Get a random RoomB using GetRandomRoom()
	// While
	// 	Rooms are the same, IsSameRoom() AND
	// 	Rooms are not connected, !ConnectionAlreadyExists() AND
	// 	RoomA can have a connection, CanAddConnectionFrom() AND
	// 	RoomB can have a connection, CanAddConnectionFrom() 
	
	/*
	struct Room A; //Maybe a struct, maybe a global arrays of ints
	struct Room B; 

	while(true)
	{
		A = GetRandomRoom();

		if(CanAddConnectionFrom(A) == true)
		{
			break;
		}
		
	}

	do
	{
		B = GetRandomRoom();
	}
	while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

	ConnectRoom(A, B); // TODO: Add this connection to the real variables,
	ConnectRoom(B, A); // because this A and B will be destroyed when this function terminates

	*/
	return;

}

/* Returns a random Room, does NOT validate if connection can be added. */
static struct Room GetRandomRoom()
{
}

/* Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise */
static bool CanAddConnectionFrom(struct Room x)
{
	// Check the number of outbound connections from Room x
	// 	If it's less than 6 return true
	// 	Else return false
}

// TODO test me ;)
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
static void ConnectRoom(struct Room x, struct Room y)
{
	// Connect Rooms x and y
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
