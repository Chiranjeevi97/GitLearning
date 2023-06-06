#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

buffer_item buffer[BUFFER_SIZE];
char *words[109583];
int counter = 0;
pthread_mutex_t mutex;
sem_t emptyBuffer;
sem_t fullBuffer;
int insert_item(int index);
int remove_item();

int insert_item(int index)
{
	if(counter <= 5){
		buffer[counter] = index;
		printf("[Producer thread ID: %ld] inserted an item (word:%s) to the buffer\n", pthread_self(), words[index]);
		counter++;
		return 0;
	}
	else{
		return -1;
	}
}

int remove_item()
{
	if(counter > 0){
		char *str = words[buffer[counter - 1]];
		counter--;
		printf("[Consumer thread ID: %ld] removed an item (word:%s) from the buffer\n", pthread_self(), str);
		return 0;
	}
	else{
		return -1;
	}
}

void *producer(void *param)
{
	while(1)
	{
		int p_sl_secs = rand() % 6;
		sleep(p_sl_secs);
		sem_wait(&emptyBuffer);
		pthread_mutex_lock(&mutex);
		int randomIndex = rand() % 109584;
		if(insert_item(randomIndex) == -1)
		{
			printf("report error condition while producing\n");
		}
		else
		{
			printf("Producer produced %d\n", randomIndex);
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&fullBuffer);
	}
}

void *consumer(void *param)
{
	while(1)
	{
		int c_sl_secs = rand() % 6;
		sleep(c_sl_secs);
		sem_wait(&fullBuffer);
		pthread_mutex_lock(&mutex);
		if(remove_item() == -1)
		{
			printf("report error condition while consuming\n");
		}
		else
		{
			printf("Consumer consumed\n");
		}
		pthread_mutex_unlock(&mutex);
		sem_post(&emptyBuffer);
	}
}

int main(int argc, char *argv[])
{
	int sl = atoi(argv[1]);
	int prod = atoi(argv[2]);
	int cons = atoi(argv[3]);
	int size = 109583;
	sem_init(&emptyBuffer, 0, BUFFER_SIZE);
	sem_init(&fullBuffer, 0 , 0);
	pthread_mutex_init(&mutex, NULL);
	char str[size];
	pthread_t pt_id[prod];
	pthread_t ct_id[cons];
	int i = 0;
	FILE *f = fopen("./wordsEn.txt", "r");
	while(fgets(str, size, f))
	{
		words[i] = strdup(str);
		i++;
	}
	fclose(f);
	for(int i = 0; i < prod; i++)
	{
		pthread_create(&pt_id[i], NULL, producer, NULL);
	}
	for(int j = 0; j < cons; j++)
	{
		pthread_create(&ct_id[j], NULL, consumer, NULL);
	}
	sleep(sl);
	exit(0);
}
