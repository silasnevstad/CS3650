#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

/*
  This is the templave for the solution producer-consumer problem
  using mutex and semaphores.  I have used 5 producers and 5
  consumers. You can always play with these values.
*/

#define MaxItems 5 // Maximum items a producer can produce or a consumer can consume
#define BufferSize 5 // Size of the buffer

// TODO: Declare mutex and semaphores
pthread_mutex_t mutex;
sem_t empty;
sem_t full;
int in = 0;
int out = 0;
int buffer[BufferSize];

// TODO: Modify producer() and consumer() to work
// properly using mutex and semaphores. 
void *producer(void *pno)
{   
    int item;
    for(int i = 0; i < MaxItems; i++) {
        item = rand(); // Produce an random item
        sem_wait(&empty); // wait if there are no empty slots
	pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno),buffer[in],in);
        in = (in+1)%BufferSize;
	pthread_mutex_unlock(&mutex);
	sem_post(&full); // signal to consumer that buffer has some data to be consumed
    }
    return NULL;
}

void *consumer(void *cno)
{   
    for(int i = 0; i < MaxItems; i++) {
        sem_wait(&full); // wait if there are no full slots
	pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("Consumer %d: Remove Item %d from %d\n",*((int *)cno),item, out);
        out = (out+1)%BufferSize;
	pthread_mutex_unlock(&mutex);
	sem_post(&empty); // signal the producer that buffer slots are emptied and more can be produced
    }
    return NULL;
}

int main()
{   

    pthread_t pro[5], con[5];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BufferSize);
    sem_init(&full, 0, 0);

    int a[5] = {1,2,3,4,5}; //Just used for numbering the producer and consumer

    // Turn off buffering for stdout so we get accurate output
    setvbuf(stdout, NULL, _IONBF, 0);

    // create producers
    for(int i = 0; i < 5; i++) {
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    }
    // create consumers
    for(int i = 0; i < 5; i++) {
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    }

    // join threads
    for(int i = 0; i < 5; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < 5; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
    
}
