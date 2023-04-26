# Crazy Bankers 
(liberally borrowed from https://begriffs.com/posts/2020-03-23-concurrent-programming.html)

Let's assume you are in charge of a bunch of crazy bankers that send random amounts of money from one account to another, and
you want to model (simulate) if they are doing job correctly.  The bankers do not talk to each other, and you 
concerned the lack of communication could lead to errors.  

## Task 1: A concurrent simulation
You want write a program to simulate the bankers moving money from one account to another without talking to each other, 
where the bankers are modeled as threads which 
give concurrency (each banker operating independenty)  without synchronization (bankers communicating).

Recalling our last lecture, you remember that the pthread library has the following calls to create threads and for the main thread to wait until all the spawned threads complete: 
- [man pthread_create](https://man7.org/linux/man-pages/man3/pthread_create.3.html)
- [man pthread_join](https://man7.org/linux/man-pages/man3/pthread_join.3.html)

Let's start small with only 10 accounts, 20 bankers, and modest values held in each account, $100, and some maximum number of transactions to model (N_ROUNDS). 

Each account's balance will be represented by a struct :
```c
struct account
{
  long balance;
}
```

We'll need a helper function to select a random account and how much to move on each transaction.  The routine below comes from Steve Summit's  C FAQ
   http://c-faq.com/lib/randrange.html.
```c
int rand_range(int N)
{
  return (int)((double)rand() / ((double)RAND_MAX + 1) * N);
}
```

The function passed to each thread is ```disburse()```.  It takes no arguments.  It selects a random account to take money from and a random one to move it to.  The amount transferred is a random portion of the from account's balance. 
``` c
void *disburse(void *arg)
{
  size_t i, from, to;
  long payment;

  for (i = 0; i < N_ROUNDS; i++)
    {
      /* pick distinct 'from' and 'to' accounts */
      from = rand_range(N_ACCOUNTS);
      do {
	      to = rand_range(N_ACCOUNTS);
      } while (to == from);

      /* go nuts sending money, try not to overdraft */
      if (accts[from].balance > 0)
	{
	  payment = 1 + rand_range(accts[from].balance);
	  accts[from].balance -= payment;
	  accts[to].balance   += payment;
	}
    }
  return NULL;
}
```

```main()``` will spawn off the threads (the bankers) who each will independently move funds around, then join all threads after they are done.  We will show the status of the accounts before and after N_ROUNDS of transfers and the total value of all the accounts before and after all the transfers.

``` c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N_ACCOUNTS 10
#define N_THREADS  20
#define N_ROUNDS   10000

/* 10 accounts with $100 apiece means there's $1,000
   in the system. Let's hope it stays that way...  */
#define INIT_BALANCE 100

/* making a struct here for the benefit of future
   versions of this program */
struct account
{
  long balance;
} accts[N_ACCOUNTS];

/* Helper for bankers to choose an account and amount at
   random. It came from Steve Summit's excellent C FAQ
   http://c-faq.com/lib/randrange.html */
int rand_range(int N)
{
  return (int)((double)rand() / ((double)RAND_MAX + 1) * N);
}

/* each banker will run this function concurrently. The
   weird signature is required for a thread function */
void *disburse(void *arg)
{
  size_t i, from, to;
  long payment;

  for (i = 0; i < N_ROUNDS; i++)
    {
      /* pick distinct 'from' and 'to' accounts */
      from = rand_range(N_ACCOUNTS);
      do {
	      to = rand_range(N_ACCOUNTS);
      } while (to == from);

      /* go nuts sending money, try not to overdraft */
      if (accts[from].balance > 0)
	{
	  payment = 1 + rand_range(accts[from].balance);
	  accts[from].balance -= payment;
	  accts[to].balance   += payment;
	}
    }
  return NULL;
}

int main(void)
{
  size_t i;
  long total;
  pthread_t bankers[N_THREADS];

  srand(time(NULL));

  for (i = 0; i < N_ACCOUNTS; i++) {
    accts[i].balance = INIT_BALANCE;
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
```

Copy this into bankers.c and compile it with the following command: ```gcc -pthread bankers.c -o bankers```,

## Data Races
What did you notice?  Was the total value before equal to the total value afterward?  Or was it something more like below?

```bash
$ ./bankers
Account 0 balance = $100
Account 1 balance = $100
Account 2 balance = $100
Account 3 balance = $100
Account 4 balance = $100
Account 5 balance = $100
Account 6 balance = $100
Account 7 balance = $100
Account 8 balance = $100
Account 9 balance = $100
Initial money in system: 1000

Account 0 balance = $10
Account 1 balance = $244
Account 2 balance = $13
Account 3 balance = $39
Account 4 balance = $88
Account 5 balance = $10
Account 6 balance = $127
Account 7 balance = $7
Account 8 balance = $28
Account 9 balance = $45
Final money in system: 611
```

Remember, threads share memory.  Each thread can read and write variables in shared memory without any overhead.  
But when threads simutaneously read and write the same data we have a **data race** which can lead to problems.  

The threads in ```bankers.c``` have data races when they read and write account balances.  While the threads move
money between accounts, the total amount of money in the system does not stay constant. 
Exactly how the program behaves depends on thread scheduling policies of the operating system.  This code could run
very differently on different OSes or versions of the same OS. 

The property that money is neither created or destroyed by the bank is an example of a **program invariant** that is 
violated by the data races.

Below is the problematic code in  ```disburse```.
``` c
	  payment = 1 + rand_range(accts[from].balance);
	  accts[from].balance -= payment;
	  accts[to].balance   += payment;
```

Threads running this code can be paused at any time.  Not just between statements, but partway through operations that
are not atomic (always run to completion) on the hardware.  Relying on **thread inertia**, that a thread will always 
finish a group of statements will lead you to concurrency heartbreak. 

### An Overdraft Scenario
Here's a timeline with two threads reading the same account balance could creates an overdraft.   
The columns of the table below are threads, and the rows are moments in time.

| Thread A | Thread B |
| ---- | ---- |
| payment = 1 + rand_range(accts[from].balance); |  |
| |  payment = 1 + rand_range(accts[from].balance); |
|  | At this time, thread B’s payment-to-be may be in excess of the true balance because thread A has already earmarked some of the money unbeknownst to B.|
| accts[from].balance -= payment; |  |
|  | accts[from].balance -= payment; |
| Some of the same dollars could have been transferred twice and the originating account could even go negative if the overlap of the payments is big enough.|

### A Lost Debit Scenario

Below is a scenario where the debit of one thread can be undone by another. 

| Thread A | Thread B |
| ---- | ---- |
| accts[from].balance -= payment;| accts[from].balance -= payment; |
| If ```-=``` is not atomic, the threads might switch execution after reading the balance and after doing arithmetic, but before assignment. | Thus one assignment would be overwritten by the other. The “lost update” creates extra money in the system.|

Similar problems can happen with data races in the destinaion accounts, which tend to increase the total supply of money.

## Task 2: Locks 

Above we found that certian lines of the ```bankers.c``` were vulnerable to data races.  These sections are called 
**critical sections**. For correct operation, we must guarantee that each thread has exclusive access to the critical 
section before another thread is allowed to enter it. 

To give threads mutually exclusive access to a critical section, the pthreads library provides a mutually exclusive lock or **mutex** for short.
- [man pthread_mutex_lock](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [man pthread_mutex_unlock](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [man pthread_mutex_init](https://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html)

The XOA VMs are missing these and other POSIX man pages. Use the following command to install these man pages on your VM:
```bash
$ sudo apt-get install manpages-posix-dev
```
The pattern is: 

```c
pthread_mutex_lock(&some_mutex);

/* ... critical section ... */

pthread_mutex_unlock(&some_mutex);
```

Any thread calling ```pthread_mutex_lock(&some_mutex);``` on an already locked mutex with block until the mutex becomes available.  

Another way to look at mutexes is that their job is to preserve program invariants. The critical section between 
locking and unlocking is a place where a certain invariant may be temporarily broken, as long as it is restored 
by the end. Experts on concurrent programming recommend adding an ```assert()``` statement before unlocking, to help 
document the invariant.  

A function is called **thread-safe** if concurrent invocations can run safely, i.e., have no data races or deadlocks.  
A simple and inefficient way to make any function thread-safe it to give it a mutex and lock the entire function. 

```c
/* inefficient but effective way to protect a function */

pthread_mutex_t myfunc_mtx = PTHREAD_MUTEX_INITIALIZER;

void myfunc(/* some arguments */)
{
	pthread_mutex_lock(&myfunc_mtx);

	/* we're safe in here, but it's a bottleneck */

	pthread_mutex_unlock(&myfunc_mtx);
}

```

To see why this is inefficient, imagine if ```myfunc()``` was designed to output characters to a file specified in its arguments. 
Because the function takes a global lock, no two threads could run it at once, even if they wanted to write to different files. 
What we really want to protect against are two threads concurrently writing the same file.

For this section, create a file ```bankers-deadlock.c``` that adds a mutex to each account and prevents the data races discussed in the previous section.

To add the mutex, update the account struct:
```c
struct account
{
  long balance;
  /* add a mutex to account to prevent races on balance */
  /* remember to initialize the mutex before using it */ 
  pthread_mutex_t mtx;
}
```

Protect the critical section by adding locks on both accounts:

```c
...
      /* get an exclusive lock on both balances before
	 updating (there may be a problem with this) */
	 /* lock in one order */
      pthread_mutex_lock(&accts[from].mtx);
      pthread_mutex_lock(&accts[to].mtx);
      int balance_in = accts[from].balance + accts[to].balance;
      if (accts[from].balance > 0)
	{
	  payment = 1 + rand_range(accts[from].balance);
	  accts[from].balance -= payment;
	  accts[to].balance   += payment;
	}
	assert(balance_in == (accts[from].balance + accts[to].balance));
	/* unlock in the reverse order of locking */
      pthread_mutex_unlock(&accts[to].mtx);
      pthread_mutex_unlock(&accts[from].mtx);
 ...
```

Now everything should be safe. No money being created or destroyed, only perfect exchanges between the accounts. The invariant is that the total balance of the source and destination accounts is the same before we transfer the money as after. It’s broken only inside the critical section.

## Addressing Deadlock

Try to run the program. It gets stuck forever and never prints the final balance! Its threads are **deadlocked**.

Deadlocking is another challenge to concurrent programming. This happens when threads waits for another thread's locks, but no thread unlocks for other threads.  Below is an example of a form of deadlock called **deadly embrace**.

| Thread A | Thread B |
| ---- | ---- |
| lock account 1 |  |
| |  lock account 2 |
|  lock account 2 | |
| Thread A is blocked bacause Thread by holds a lock on account 2|  |
|  | Lock account 1 |
| |Thread B is blocked because thread A holds a lock on account 1. However thread A will never unlock account 1 because thread A is blocked!|
 

The problem happens because threads lock resources in different orders, and because they refuse to give locks up. We can solve the problem by addressing either of these causes.

The first approach to preventing deadlock is to enforce a locking hierarchy. This means the programmer comes up with an arbitrary order for locks, and always takes “earlier” locks before “later” ones. 

Since we store all the accounts in an array, so we can use the array index as the lock order.

```c
/* lock mutexes in earlier accounts first */

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

pthread_mutex_lock(&accts[MIN(from, to)].mtx);
pthread_mutex_lock(&accts[MAX(from, to)].mtx);
/* move money */
pthread_mutex_unlock(&accts[MAX(from, to)].mtx);
pthread_mutex_unlock(&accts[MIN(from, to)].mtx);

/* notice we still unlock in reverse order */
```

For this section, create a file ```bankers-lock.c``` that adds order the locks and unlocks to avoid the deadlock created in the previous section.

## Task 4: Trylock

A locking hierarchy is the most efficient way to prevent deadlock, but it isn’t always easy to design. It’s also creates a potentially undocumented coupling between different parts of a program which need to collaborate.

**Backoff** is a different way to prevent deadlock which works for locks taken in any order. It takes a lock, but then checks whether the next is obtainable. If not, it unlocks the first to allow another thread to make progress, and tries again.

 - [man pthread_mutex_trylock](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)

```c
      /* using pthread_mutex_trylock to dodge deadlock */

      while (1)
	{
	  pthread_mutex_lock(&accts[from].mtx);
	
	  if (pthread_mutex_trylock(&accts[to].mtx) == 0)
	    break; /* got both locks */

	  /* didn't get the second one, so unlock the first */
	  pthread_mutex_unlock(&accts[from].mtx);
	  /* force a sleep so another thread can try --
	     include <sched.h> for this function */
	  sched_yield();
	}

      if (accts[from].balance > 0)
	{
	  payment = 1 + rand_range(accts[from].balance);
	  accts[from].balance -= payment;
	  accts[to].balance   += payment;
	}

      pthread_mutex_unlock(&accts[to].mtx);
      pthread_mutex_unlock(&accts[from].mtx);
      /* notice we continue unlock in opposite order */

```

A tricky part of using trylock is the call to ```sched_yield()```. Without it the loop will immediately try to grab the lock again, competing as hard as it can with other threads who could make more productive use of the lock. This causes **livelock**, where threads fight for access to the locks.  ```sched_yield()``` puts the calling thread at the back of the scheduler’s run queue.

For this section, create a file ```bankers-trylock.c``` that adds a a trylock and prevents the deadlock created in the previous section.

Despite its flexibility, backoff is definitely less efficient than a locking hierarchy because it can make wasted calls to lock and unlock mutexes. Try comparing the runtime of  ```banker-trylock``` versus ```banker-lock``` and measure how fast they run.






## Deliverables
- Update and checkin ```bankers.c```.
- Create and checkin ```bankers-deadlock.c```.
- Create and checkin ```bankers-lock.c```.
- Create and checkin ```bankers-trylock.c```.

