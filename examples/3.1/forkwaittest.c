#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{	
	/* Initialized to junk values. */
	pid_t spawnPid = -5;
	int childExitMethod = -5;

	spawnPid = fork();
	if(spawnPid == -1) // Error no child process created.
	{
		perror("Hull Breach!\n");
		exit(1);
	}
	else if(spawnPid == 0) // In child process returns 0.
	{
		printf("CHILD: PID: %d, exiting!\n", spawnPid);
		exit(0);
	}

	printf("PARENT: PID: %d, waiting...!\n", spawnPid);

	/* Block parent until child terminates. */
	waitpid(spawnPid, &childExitMethod, 0);
	printf("PARENT: Child process terminated, exiting!\n");
	exit(0);

}

