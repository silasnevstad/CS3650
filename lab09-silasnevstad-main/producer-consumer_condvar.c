#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* Fix this Producer/consumer program using mutex and conditional variables */

/* Size of shared buffer */
#define BUFFER_SIZE 3

int buffer[BUFFER_SIZE];/* shared buffer */
int add=0;		/* place to add next element */
int rem=0;		/* place to remove next element */
int num=0;		/* number elements in buffer */

pthread_mutex_t mutex;  /* mutex lock for the buffer */
pthread_cond_t cv_cons; /* conditional variable for index to add next element */
pthread_cond_t cv_prod; /* conditional variable for index to remove the next element */


//void *producer(void *param);
//void *consumer(void *param);

// TODO: Modify producer() and consumer() to
// work properly using mutex and condition variables

/* Produce value(s) */
void *producer(void *param)
{
	int i, j;

        for (i=1; i<=20; i++) {
		pthread_mutex_lock(&mutex);
		/* Insert into buffer */
		if (num > BUFFER_SIZE) exit(1);	/* overflow */
		while ((add + 1) % BUFFER_SIZE == rem)		/* block if there are no available slots */
                  pthread_cond_wait(&cv_prod, &mutex);
		/* if executing here, there is available slot in buffer so add element */
		buffer[add] = i;
		j = add;
		add = (add+1) % BUFFER_SIZE;
		num++;
		printf ("producer: inserted item %d at %d\n", buffer[j], j);  fflush(stdout);
                pthread_mutex_unlock(&mutex);
                pthread_cond_signal(&cv_cons);
	}
	printf ("producer quiting\n");  fflush (stdout);
	return NULL;
}

/* Consume value(s) */
void *consumer(void *param)
{
	int i, j;

	for (j=0; j<20; j++) {
		pthread_mutex_lock(&mutex);
		if (num < 0) exit(1);   /* underflow */
		while (num == 0)	/* block if buffer empty */
                  pthread_cond_wait(&cv_cons, &mutex);
		/* if executing here, buffer not empty so remove element */
		i = buffer[rem];
		rem = (rem+1) % BUFFER_SIZE;
		num--;
		printf ("consumer: removed item %d at %d\n", buffer[i], i);  fflush(stdout);
                pthread_mutex_unlock(&mutex);
                pthread_cond_signal(&cv_prod);
	}
	printf ("consumer quiting\n");  fflush (stdout);
	return NULL;
}


// Do not modify the code that starts and joins the threads
int main (int argc, char *argv[])
{
	pthread_t tid1, tid2;		/* thread identifiers */

	/* create the threads; in general, may be any number */
	if (pthread_create(&tid1,NULL,producer,NULL) != 0) {
		fprintf (stderr, "Unable to create producer thread\n");
		exit (1);
	}

	if (pthread_create(&tid2,NULL,consumer,NULL) != 0) {
		fprintf (stderr, "Unable to create consumer thread\n");
		exit (1);
	}

	/* wait for created threads to exit */
	pthread_join(tid2,NULL);
	pthread_join(tid1,NULL);

	// Destroy mutexes
	printf ("Parent quiting\n");
	pthread_mutex_destroy(&mutex);
	return 0;
}
