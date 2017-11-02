#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
	/* Fork variables initialized to dummy variables. */
	pid_t spawnPid = -5;
	int childExitStatus = -5;

	/* Fork a child. */
	spawnPid = fork();
	switch(spawnPid)
	{
		case -1:
			perror("Hull breach! Erroar\n");
			exit(1);
			break;
		/* Child process returns 0. */
		case 0:
			printf("CHILD: Terminating!\n");
			break;
		/* We in the parent, still. */
		default:
			printf("PARENT: making child a zombie for ten seconds;\n");
			printf("PARENT: Type \"ps -elf | grep \'username\'\" to see the defunct child\n");
			printf("PARENT: Sleeping...\n");

			/* Make sure all text is outputted before sleeping. */
			fflush(stdout);
			sleep(10);

			/* Block parent (calling process) until the child terminated. */
			waitpid(spawnPid, &childExitStatus, 0);

			break;

	}

	printf("This will be executed by both of us!\n");
	exit(0);

}

	
