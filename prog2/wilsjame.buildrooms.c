/*********************************************************************
** Author: James G Wilson
** Date: 21 October 2017
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
#include <stdbool.h>

struct Room
{
	int x;
};

static bool IsGraphFull();
static void AddRandomConnection();
static struct Room GetRandomRoom();
static bool CanAddConnectionFrom(struct Room x);
static bool ConnectionAlreadyExists(x, y);
static void ConnectRoom(struct Room x, struct Room y);
static bool IsSameRoom(struct Room x, struct Room y);

int main(void)
{

	/* Create all connections in graph. */
	while (IsGraphFull() == false)
	{
		AddRandomConnection();
	}

	// Now all connections are created
	// Output each room as a file to the proper directory
	
	return 0;

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
}

/* Returns a random Room, does NOT validate if connection can be added. */
static struct Room GetRandomRoom()
{
	// Do some random magic to select a Room
	// 	Return it
}

/* Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise */
static bool CanAddConnectionFrom(struct Room x)
{
	// Check the number of outbound connections from Room x
	// 	If it's less than 6 return true
	// 	Else return false
}

/* Returns true if a connection from Room x to Room y already exists, false otherwise. */
static bool ConnectionAlreadyExists(x, y)
{
	// Check if a connection between the Rooms already exists
	// 	If it exists return true
	// 	Else return false
}

/* Connects Rooms x and y together, does not check if this connection is valid. */
static void ConnectRoom(struct Room x, struct Room y)
{
	// Connect Rooms x and y
}

/* Returns true if Rooms x and y are the same Room, false otherwise. */
static bool IsSameRoom(struct Room x, struct Room y)
{
	// Compare Rooms x and y 
	//	If equal return true
	//	Else return false
}
