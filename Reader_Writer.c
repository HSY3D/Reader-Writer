/*
	Hannan Syed
	260510904

	compile: gcc -pthread -o filename filename.c
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define READERS 100 //set number of reader to 100
#define WRITERS 10 //set number of readers to 10

//create the semaphore variables
sem_t rw_mutex;
sem_t mutex;

//init all variables
int rc, integer = 0, iter = 0;
long minR, maxR, sumR, minW, maxW, sumW, countR, countW;

void *Reader (void *arg);
void *Writer (void *arg);

//this function initializes all the threads 
int main(int argc, char const *argv[])
{
	int i, numR_threads = READERS, numW_threads = WRITERS; //init variables for main

	/*
		min = HIGHVALUE
		max = LOWVALUE
		sum = 0 (used for average)
		count = 0 (used for average)
	*/

	minR = 1000000;
	maxR = 0;
	sumR = 0;
	minW = 1000000;
	maxW = 0;
	sumW = 0;
	countW = 0;
	countR = 0;

	//init semaphores
	sem_init(&rw_mutex, 0, 1);
	sem_init(&mutex, 0, 1);

	//create thread variables
	pthread_t R_threads[100], W_threads[10];

	if(argv[1] == NULL) {
		iter = 10; //default numberofiterations
	} else {
		iter = atoi(argv[1]); //set iterations from args
	}

	/*
		create the reader thread
		create the writer threads

		join the reader thread
		join the writed thread
	*/

	for (i = 0; i < numR_threads; ++i)
		pthread_create(&R_threads[i], NULL, Reader, (void *) i);

	for (i = 0; i < numW_threads; ++i)
		pthread_create(&W_threads[i], NULL, Writer, (void *) i);

	for (i = 0; i < numW_threads; ++i)
		pthread_join(W_threads[i], NULL);

	for (i = 0; i < numR_threads; ++i)
		pthread_join(R_threads[i], NULL);

	//PRINT EVERYTHING
	printf("\nMin Time Writer: %ld microseconds", minW);
	printf("\nMax Time Writer: %ld microseconds", maxW);
	printf("\nAvg Time Writer: %ld microseconds", sumW/countW);
	printf("\nMin Time Reader: %ld microseconds", minR);
	printf("\nMax Time Reader: %ld microseconds", maxR);
	printf("\nAvg Time Reader: %ld microseconds\n", sumR/countR);

	//destroy the semaphores
	sem_destroy(&rw_mutex);
	sem_destroy(&mutex);

	return 0;
}

//this function is a reader for the threads
void *Reader (void *arg)
{
	/*
		waittime is between 0 and 100 ms
		tmp is the number of the reader thread
		i is the iteration of reads
	*/
	int waittime = (rand() % 100), tmp = (int)arg, i = iter;
	struct timeval start, end; //create a timeval

	do {
		gettimeofday(&start, NULL); //take the initial time
		
		sem_wait(&mutex); //start the readCount semaphore
		rc++; //increment the readCount
		if (rc == 1)
		{
			sem_wait(&rw_mutex); //start the readWrite semaphore
			printf("\nReader %d: %d", tmp, integer);
		}
		sem_post(&mutex); //end the readCount semaphore
		sem_wait(&mutex); //start the readCount sempahore
		rc--; //decrement the readCount
		if (rc == 0)
		{
			sem_post(&rw_mutex); //end the readWrite semaphore
		}
		sem_post(&mutex); //end the readCount semaphore
		usleep(waittime * 1000);

		gettimeofday(&end, NULL); //take final time

		long difference = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec); //calculate the difference
		// printf("\nTime: %ld", difference);

		countR++; //increase the read thread counter
		sumR += difference; //add the difference to the sum
		if (difference > maxR)	
		{
			maxR = difference; //set the new max
		}

		if (difference < minR)
		{
			minR = difference; //set the new min
		}

		i--;
	} while (i >= 0);
}

void *Writer (void *arg)
{
	int waittime = (rand() % 100), tmp = (int)arg, i = iter;
	struct timeval start, end;

	do {
		gettimeofday(&start, NULL);

		sem_wait(&rw_mutex);
		integer = integer + 10;
		sem_post(&rw_mutex);
		usleep(waittime * 1000);

		gettimeofday(&end, NULL);

		long difference = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
		// printf("\nTime: %ld", difference);

		countW++;
		sumW += difference;
		if (difference > maxW)	
		{
			maxW = difference;
		}

		if (difference < minW)
		{
			minW = difference;
		}

		i--;
	} while (i >= 0);
}