#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

#define N_ACCOUNTS 10
#define N_THREADS 20
#define N_ROUNDS 1000

#define INIT_BALANCE 100 // initial balance of $100 for each account (10 * 100 = $1000)

struct account {
  long balance;
  pthread_mutex_t lock;
} accts[N_ACCOUNTS];

/* Helkper for bankers to an account and amount at random.
   It came from Steve Semmit's C FAQ http://c-faq.com/lib/randrange.html */
int rand_range(int N) {
  return (int)((double)rand() / ((double)RAND_MAX + 1) * N);
}

/* each banker runs this function concurrently. The weird signature 
   is required from for a thread function. */
void *disburse(void *arg) {
  size_t i, from, to;
  long payment;

  for (i = 0; i < N_ROUNDS; i++) {
    /* pick distinct 'from' and 'to' accounts */
    from = rand_range(N_ACCOUNTS);
    do {
	  to = rand_range(N_ACCOUNTS);
    } while (to == from);
    
    /* using pthread_mutex_trylock to dodge deadlock */

    while (1) {
	  pthread_mutex_lock(&accts[from].lock);
	
	  if (pthread_mutex_trylock(&accts[to].lock) == 0)
	    break; /* got both locks */

	  /* didn't get the second one, so unlock the first */
	  pthread_mutex_unlock(&accts[from].lock);
	  /* force a sleep so another thread can try --
	     include <sched.h> for this function */
	  sched_yield();
	}

    if (accts[from].balance > 0) {
	  payment = 1 + rand_range(accts[from].balance);
	  accts[from].balance -= payment;
	  accts[to].balance   += payment;
	}

    pthread_mutex_unlock(&accts[to].lock);
    pthread_mutex_unlock(&accts[from].lock);
    /* notice we continue unlock in opposite order */
  }
  return NULL;
}

int main(void) {
  size_t i;
  long total;
  pthread_t bankers[N_THREADS];

  srand(time(NULL));

  for (i = 0; i < N_ACCOUNTS; i++) {
    accts[i].balance = INIT_BALANCE;
    accts[i].lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    printf("Account %ld balance = $%ld\n", i, accts[i].balance);
  }
	
  printf("Initial money in system: %d\n\n",
	 N_ACCOUNTS * INIT_BALANCE);

  /* start the threads, using whatever parallelism the
     system happens to offer. Note that pthread_create
     is the *only* function that creates concurrency */
  for (i = 0; i < N_THREADS; i++) {
    pthread_create(&bankers[i], NULL, disburse, NULL);
  }

  /* wait for the threads to all finish, using the
     pthread_t handles pthread_create gave us */
  for (i = 0; i < N_THREADS; i++) {
    pthread_join(bankers[i], NULL);
  }

  for (total = 0, i = 0; i < N_ACCOUNTS; i++) {
    total += accts[i].balance;
    printf("Account %ld balance = $%ld\n", i, accts[i].balance); 
  }
	
  printf("Final money in system: %ld\n", total);
}

