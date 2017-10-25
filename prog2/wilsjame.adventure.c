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

#define MAXROOMS 7
#define MAXCONNECTIONS 6

/*** Files store rooms in exactly this form ***
 
ROOM NAME: Workaholics 
CONNECTION 1: EyeKnow
CONNECTION 2: Common
CONNECTION 3: Quavo
ROOM TYPE: START_ROOM

*/

struct Room
{
	char type[250];
	char name[250];
	int numOutboundConnections;
	struct Room* roomConnections[MAXCONNECTIONS];
};

//TODO
//read files into array of room structs
//design game loop structure
//design menu input functionality
//add time feature using threads

int main(void)
{
	int newestDirTime = -1; // Modified timestamp of newest subdir examined.
	char targetDirPrefix[32] = "wilsjame.rooms."; // Prefix to look for.
	char newestDirName[250]; // Holds name of newest dir containing prefix.
	memset(newestDirName, '\0', sizeof(newestDirName));

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

			/* Check if the entry has the prefix. */
			if(strstr(fileInDir->d_name, targetDirPrefix) != NULL)
			{
				printf("Found the prefix: %s\n", fileInDir->d_name);

				/* Get attributes of entry into stat struct. */
				stat(fileInDir->d_name, &dirAttributes);
			}

			/* Check for more recent timestamp. */
			if((int)dirAttributes.st_mtime > newestDirTime)
			{
				newestDirTime = (int)dirAttributes.st_mtime;
				memset(newestDirName, '\0', sizeof(newestDirName));
				strcpy(newestDirName, fileInDir->d_name);
				printf("Newer subdir: %s, new time: %d\n",
					fileInDir->d_name, newestDirTime);
			}

		}

	}

	/* Close directory stream. */
	closedir(dirToCheck);

	printf("Newest entry found is: %s\n", newestDirName);

	return 0;

}

