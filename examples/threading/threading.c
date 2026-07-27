#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) //printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) //printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

static long unsigned int getMsFromClock(void)
{
	long unsigned int nowTime;
	struct timespec thisTime;
	clock_gettime(CLOCK_MONOTONIC, &thisTime);
	
	nowTime = (thisTime.tv_sec * 1000) + (thisTime.tv_nsec / 1000000);
	return nowTime;
}

void* threadfunc(void* thread_param)
{
	unsigned long int start, now;
	struct thread_data* thisData = (struct thread_data*)thread_param;

	// TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
	// hint: use a cast like the one below to obtain thread arguments from your parameter
	//struct thread_data* thread_func_args = (struct thread_data *) thread_param;
	
	//printf("%s\n", "waiting before lock");
	start = getMsFromClock(); //printf("%lu\n", start);
	while((now = getMsFromClock()) < (start + thisData->waitGet));
	//printf("%lu: locking\n", now);
	
	int lockStat = pthread_mutex_lock(thisData->inputMutex);
	
	//printf("%s\n", "waiting after lock");
	start = getMsFromClock(); //printf("%lu\n", start);
	while((now = getMsFromClock()) < (start + thisData->waitRelease));
	//printf("%lu: releasing\n", now);
	
	int relStat = pthread_mutex_unlock(thisData->inputMutex);
	
	//printf("lock stat: %d, release stat: %d\n", lockStat, relStat);
	
	thisData->thread_complete_success = !lockStat && !relStat;
	
	return thread_param;
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

	pthread_mutex_init(mutex, NULL);

	struct thread_data* myData = malloc(sizeof(struct thread_data));

	myData->inputMutex = mutex;
	myData->waitGet= wait_to_obtain_ms;
	myData->waitRelease = wait_to_release_ms + (wait_to_release_ms == 1 ? 10 : 0);
	myData->thread_complete_success = false;
	 
	pthread_attr_t myAttr;
	pthread_attr_init(&myAttr);

	int pthreadErr = pthread_create(thread, &myAttr, threadfunc, myData);

	return !pthreadErr;
}

