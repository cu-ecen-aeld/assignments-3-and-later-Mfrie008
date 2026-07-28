#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) //printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) //printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
	struct thread_data* thisData = (struct thread_data*)thread_param;

	// TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
	// hint: use a cast like the one below to obtain thread arguments from your parameter
	//struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	
	//printf("%s\n", "waiting before lock");
	usleep(1000*thisData->waitGet);
	//printf("%lu: locking\n", now);
	
	int lockStat = pthread_mutex_lock(thisData->inputMutex);
	
	if(lockStat != 0)
	{
		perror(NULL);
		thisData->thread_complete_success = false;
		return thisData;
	}
	
	//printf("%s\n", "waiting after lock");
	usleep(1000*thisData->waitRelease);
	//printf("%lu: releasing\n", now);
	
	int relStat = pthread_mutex_unlock(thisData->inputMutex);
	
	if(relStat != 0)
	{
		perror(NULL);
		thisData->thread_complete_success = false;
		return thisData;
	}
	
	//printf("lock stat: %d, release stat: %d\n", lockStat, relStat);
	
	thisData->thread_complete_success = true;
	
	return thisData;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
	/**
	 * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
	 * using threadfunc() as entry point.
	 *
	 * return true if successful.
	 *
	 * See implementation details in threading.h file comment block
	 */
	//printf("Creating thread for waitGet: %d, and waitRel: %d\n", wait_to_obtain_ms, wait_to_release_ms);

	struct thread_data* myData = malloc(sizeof(struct thread_data));
	if(myData == NULL)
	{
		return false;
	}

	myData->inputMutex = mutex;
	myData->waitGet= wait_to_obtain_ms;
	myData->waitRelease = wait_to_release_ms;
	myData->thread_complete_success = true;

	int pthreadErr = pthread_create(thread, NULL, threadfunc, myData);
	
	if(pthreadErr != 0)
	{
		perror(NULL);
		free(myData);
		return false;
	}

	return true;
}

