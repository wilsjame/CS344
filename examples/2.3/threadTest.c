/* Thread Example from lecture 2.3 Concurrency @23:30 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef NUM_THREADS
#define NUM_THREADS 5

/* Returns a void pointer: basically can return anything!
 * Argument is a void pointer: can take anything!
 */
void* perform_work(void* argument)
{
	int passed_in_value;

	/* Cast argument to an int pointer and deference. */
	passed_in_value = *((int *) argument);
	printf("Whats up dawg! It's me, thread with argument %d!\n", passed_in_value);
	return NULL;
}

int main(void)
{
	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int result_code, index;

	for (index = 0; index < NUM_THREADS; ++index) { 
		/* Create all threads one by one. */
		thread_args[index] = index;
		printf("In main: creating thread %d\n", index);
					/* Cast to void star because thats what is expected by the function. */
		result_code = pthread_create(&threads[index], NULL, perform_work, (void *) &thread_args[index]);
		assert(0 == result_code);
	}

	/* Wait for each thread to complete. */
	for (index = 0; index < NUM_THREADS; ++index)
	{
		/* Halt main process (thread) until "threads[index]" thread terminates. 
		 * IOW Block and do nothing else until thread 'index' completes. 
		 */
		result_code = pthread_join(threads[index], NULL);
		printf("In main: thread %d has completed\n", index);
		assert(0 == result_code);
	}

	printf("In main: All threads completed successfully\n");

	/* Bad style no return value >:( */
	exit(EXIT_SUCCESS);
}

#endif //ifndef NUM_THREADS

