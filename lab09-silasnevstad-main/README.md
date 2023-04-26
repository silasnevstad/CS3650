# Condition Variables and Semaphores

**Even though this lab is to be submitted individually, feel free to partner up and discuss the lab content with your partner.**

## Task 0 - Adding the POSIX man pages to your VM (DO NOT SKIP)

The man pages for much of the pthreads library is missng from your VM.  Install them using:
```
$ sudo apt install manpages-posix-dev
```
---
## Task 1 - Help the Olympic Relay Team Using Semaphores

<img align="right" src="./media/5786a4341a00002600dd1393.webp" width="200px" alt="Allison Felix">

Prof. Jackson thinks relay races are one of the most exciting parts of a track meet. The 4x100 meter relay has 4 runners, each running an equal distance (i.e. 100 meters), and then passing a baton to the next runner on their team. Runners are not allowed to start running until the previous runner has finished. The first runner is the lead, and the last runner (the fourth runner) is typically called the anchor. It is important that only one runner is ever running at a given time. 

Today, you are put in charge of several Olympic athletes who you will be coaching through a relay. Unfortunately, they are not very synchronized. Read on to the task!

You are provided a file [relay\_sem.c](./relay_sem.c) which you will work in. 

* You can compile the program using `make relay_sem`
* Then run the compiled program with `./relay_sem` or `make run_sem`

After you compile and run the program, you should receive the following output, which is the correct ordering of the athletes:

```
-bash-4.2$ ./relay_sem
Sha'Carri Richardson has taken off!
Allyson Felix has taken off
Dina Asher-Smith has taken off
Shelly-Ann Fraser-Pryce runs the anchor leg to the finish line!
```


However, when you run the program you might receive output that looks like this:

```
-bash-4.2$ ./relay_sem
Sha'Carri Richardson has taken off!
Shelly-Ann Fraser-Pryce runs the anchor leg to the finish line!
Dina Asher-Smith has taken off
Allyson Felix has taken off
```

Hmm, this order is not correct! And even worse, looking at the code, you will notice all of the runners are taking off at the same time in the relay race!


**Your task**: Correct the program, [relay\_sem.c](./relay_sem.c), using multiple *semaphores* to ensure that each runner runs in the correct order. In other words, each runner needs to wait to be passed the baton before they can run. **Do not change the code that starts and joins the threads.** 

Notes:

1. A semaphores is initialized using `sem_init`, which takes 3 arguments:
  - A pointer to the semaphore you want to initialize
  - Whether the semaphore is shared by threads or processes - **pass 0**
  - The initial value of the semaphore
2. A semaphore is decremented using `sem_wait`. If the semaphore is 0, then the function will wait until it is greater than 0 and then decrement it.
3. A semaphore is incremented using `sem_post`.
4. Destroying a semaphore is achieved using `sem_destroy`.

Check out [`man sem_overview`](https://linux.die.net/man/7/sem_overview) and the individual man pages for the above functions.


### Checking your solution

You can check your solution using `make check_sem`. This will run `./relay_sem` several times, comparing it against the correct output. If your race runs correctly each time you will get "Correct!" as the reply.

## Task 2 - The Producer/Consumer problem and Condition Variables

### The Producer/Consumer problem

The producer and consumer share a fixed size buffer.  The producer generates data and puts it in the buffer.  The consumer removes (consumes) data from the buffer, one item at a time.  A proper solution prevents:
 - producers from putting data into the buffer when it is full, 
 - comsumers from trying to remove data when the buffer is empty
 - more than one producer or consumer accessing the buffer at one time

Producer/Consumer problems occur frequently in modeling and implementing real systems.  There are multiple variants of the problem.  Here are a few in order of increasing difficulty:
 - Single producer & single consumer
 - Multiple producers & single consumer
 - Multiple producers & multiple consumers

You are provided a file [producer-consumer\_condvar.c](./producer-consumer_condvar.c) which you will work in. 

* You can compile the program using `make producer-consumer_condvar`
* Then run the compiled program with `./producer-consumer_condvar`.

After you compile and run the program, you might receive the following output:
```shell
$ ./producer-consumer_condvar
producer: inserted item 1 at 0
producer: inserted item 2 at 1
producer: inserted item 3 at 2
consumer: removed item 1 at 0
consumer: removed item 2 at 1
consumer: removed item 3 at 2
producer: inserted item 4 at 0
producer: inserted item 5 at 1
producer: inserted item 6 at 2
consumer: removed item 4 at 0
consumer: removed item 5 at 1
consumer: removed item 6 at 2
.
.
.
producer: inserted item 16 at 0
producer: inserted item 17 at 1
producer: inserted item 18 at 2
consumer: removed item 16 at 0
consumer: removed item 17 at 1
consumer: removed item 18 at 2
producer: inserted item 19 at 0
producer: inserted item 20 at 1
producer quiting
consumer: removed item 19 at 0
consumer: removed item 20 at 1
consumer quiting
Parent quiting
```

However, when you run the program you might receive output that looks like this:

```shell
$ ./producer-consumer_condvar
producer: inserted item 1 at 0
producer: inserted item 2 at 1
producer: inserted item 3 at 2
producer: inserted item 4 at 0
producer: inserted item 5 at 1
```
**Your task**: Correct the program, [producer-consumer\_condvar.c](./producer-consumer_condvar.c), using  *condition variables* and  mutex to ensure that:
 - producers can not add data into the buffer when it is full, 
 - comsumers can not remove data when the buffer is empty
 - no more than one producer or consumer can access the buffer at one time  

**Do not change the code that starts and joins the threads.** 

Notes:

1. A condition variable can be initialized two ways   
  - Using `pthread_cond_init`, which takes 2 arguments:
    - A pointer to the condition variable you want to initialize
    - With any attributes, you wish it to have -- **pass NULL**
  - or setting a condition variable to PTHREAD_COND_INITIALIZER when created
    - `pthread_t cond_var = PTHREAD_COND_INITIALIZER;`
2. To send a signal on a condition variable, use `pthread_cond_signal`, which unblocks at least one thread that is blocked on the condition variable
3. To block on a condition variable, use `pthread_cond_wait`, which takes two arguments:
  - a pointer to the condition variable 
  - a pointer to a mutex
  - The function atomically releases the mutex and causes the thread to block on the state of the condition variable
4. Destroying a condition variable is achieved using `pthread_cond_destroy`.

Below is a pseudocode solution using condition variables and a mutex. 

Initialization:
```c
int buffer[BUFFER_SIZE];  \\ Shared circular buffer
int add=0;                \\ Place to add iten
int rem=0;                \\ Place to remove item
int num=0                 \\ Number of items in buffer
pthread_mutex_t mutex;    \\ mutex lock for buffer
pthread_cond_t cv_cons;   \\ consumer waits on this cond var
pthread_cond_t cv_prod;   \\ producer waits on this cond var
```

Producer:
```c
while (true) {  \\ run forever
  \\ produce an item 
  pthread_mutex_lock (&mutex);           \\ get lock
  while ((add + 1) % BUFFER_SIZE == rem) \\ block if no slots are available
    pthread_cond_wait(&cv_prod, &mutex);
  buffer[add] = item;
  add = (add + 1) % BUFFER_SIZE;
  num++;
  pthread_mutex_unlock (&mutex);        \\ unlock mutex
  pthread_cond_signal (&cv_cons);       \\ send signal to consumer(s)
}
```

Consumer:
```c
while (true) {  \\run forever
  pthread_mutex_lock (&mutex);          \\ get lock
  while (add == rem)                    \\ block if nothing to consume
    pthread_cond_wait(&cv_cons, &mutex);
  \\ consuming an item
  item = buffer[rem];
  rem = (rem + 1) % BLOCK_SIZE;
  nunm--;
  pthread_mutex_unlock (&mutex);        \\ unlock mutex
  pthread_cond_signal (&cv_prod);       \\ send signal to producer(s)
}
```

## Task 3 - The Producer/Consumer problem and Semaphores 

Producer/Consumer can be also solved using semaphores.  The edge conditions can map nicely to the barrier semaphore model. 

You are provided a file [producer-consumer\_sem.c](./producer-consumer_sem.c) which you will work in. 

* You can compile the program using `make producer-consumer_sem`
* Then run the compiled program with `./producer-consumer_sem`.

After you compile and run the program, you might receive the following output:
```shell
$ ./producer-consumer_sem
Producer 1: Insert Item 1804289383 at 0
Producer 1: Insert Item 846930886 at 1
Producer 1: Insert Item 1681692777 at 2
Producer 1: Insert Item 1714636915 at 3
Producer 1: Insert Item 1957747793 at 4
Consumer 2: Remove Item 1804289383 from 0
Consumer 2: Remove Item 846930886 from 1
Consumer 2: Remove Item 1681692777 from 2
Consumer 2: Remove Item 1714636915 from 3
Producer 4: Insert Item 719885386 at 0
Producer 4: Insert Item 596516649 at 1
Producer 4: Insert Item 1189641421 at 2
Producer 4: Insert Item 1025202362 at 3
Consumer 1: Remove Item 1957747793 from 4
Consumer 1: Remove Item 719885386 from 0
Consumer 1: Remove Item 596516649 from 1
Producer 4: Insert Item 1350490027 at 4
Consumer 1: Remove Item 1189641421 from 2
Producer 5: Insert Item 1649760492 at 0
Consumer 1: Remove Item 1025202362 from 3
Producer 5: Insert Item 1102520059 at 1
Consumer 3: Remove Item 1350490027 from 4
Producer 3: Insert Item 424238335 at 2
Producer 2: Insert Item 783368690 at 3
Consumer 3: Remove Item 1649760492 from 0
Producer 3: Insert Item 1967513926 at 4
Consumer 5: Remove Item 1102520059 from 1
Producer 2: Insert Item 1365180540 at 0
Consumer 5: Remove Item 424238335 from 2
Producer 3: Insert Item 1540383426 at 1
Consumer 5: Remove Item 783368690 from 3
Producer 2: Insert Item 304089172 at 2
Consumer 5: Remove Item 1967513926 from 4
Producer 3: Insert Item 1303455736 at 3
Consumer 5: Remove Item 1365180540 from 0
Producer 2: Insert Item 35005211 at 4
Producer 5: Insert Item 2044897763 at 0
Consumer 2: Remove Item 1540383426 from 1
Producer 3: Insert Item 521595368 at 1
Consumer 4: Remove Item 304089172 from 2
Consumer 3: Remove Item 1303455736 from 3
Consumer 3: Remove Item 35005211 from 4
Consumer 3: Remove Item 2044897763 from 0
Producer 5: Insert Item 1726956429 at 2
Producer 5: Insert Item 336465782 at 3
Consumer 4: Remove Item 521595368 from 1
Consumer 4: Remove Item 1726956429 from 2
Consumer 4: Remove Item 336465782 from 3
Producer 2: Insert Item 294702567 at 4
Consumer 4: Remove Item 294702567 from 4
```
However, when you run the program you might receive output that looks like this:

```shell
$ ./producer-consumer_sem
Producer 1: Insert Item 1804289383 at 0
Producer 1: Insert Item 1681692777 at 1
Producer 1: Insert Item 1714636915 at 2
Producer 1: Insert Item 1957747793 at 3
Producer 1: Insert Item 424238335 at 4
Producer 2: Insert Item 846930886 at 0
Producer 2: Insert Item 719885386 at 1
Producer 2: Insert Item 1649760492 at 2
Producer 2: Insert Item 596516649 at 3
Producer 2: Insert Item 1189641421 at 4
Producer 3: Insert Item 1025202362 at 0
Producer 3: Insert Item 1350490027 at 1
Producer 3: Insert Item 783368690 at 2
Producer 3: Insert Item 1102520059 at 3
Producer 3: Insert Item 2044897763 at 4
Producer 4: Insert Item 1967513926 at 0
Producer 4: Insert Item 1365180540 at 1
Producer 4: Insert Item 1540383426 at 2
Producer 4: Insert Item 304089172 at 3
Producer 4: Insert Item 1303455736 at 4
Consumer 1: Remove Item 1967513926 from 0
Consumer 1: Remove Item 1365180540 from 1
Consumer 1: Remove Item 1540383426 from 2
Consumer 1: Remove Item 304089172 from 3
Consumer 1: Remove Item 1303455736 from 4
Consumer 2: Remove Item 1967513926 from 0
Consumer 2: Remove Item 1365180540 from 1
Consumer 2: Remove Item 1540383426 from 2
Consumer 2: Remove Item 304089172 from 3
Consumer 2: Remove Item 1303455736 from 4
Consumer 3: Remove Item 1967513926 from 0
Consumer 3: Remove Item 1365180540 from 1
Consumer 3: Remove Item 1540383426 from 2
Consumer 3: Remove Item 304089172 from 3
Consumer 3: Remove Item 1303455736 from 4
Consumer 4: Remove Item 1967513926 from 0
Consumer 4: Remove Item 1365180540 from 1
Consumer 4: Remove Item 1540383426 from 2
Consumer 4: Remove Item 304089172 from 3
Consumer 4: Remove Item 1303455736 from 4
Producer 5: Insert Item 35005211 at 0
Producer 5: Insert Item 521595368 at 1
Producer 5: Insert Item 294702567 at 2
Producer 5: Insert Item 1726956429 at 3
Producer 5: Insert Item 336465782 at 4
Consumer 5: Remove Item 35005211 from 0
Consumer 5: Remove Item 521595368 from 1
Consumer 5: Remove Item 294702567 from 2
Consumer 5: Remove Item 1726956429 from 3
Consumer 5: Remove Item 336465782 from 4
```

**Your task**: Correct the program, [producer-consumer\_sem.c](./producer-consumer_sem.c), using two *semaphores* and a single mutex to ensure that:
 - producers can not add data into the buffer when it is full, 
 - comsumers can not remove data when the buffer is empty
 - no more than one producer or consumer can access the buffer at one time  

**Do not change the code that starts and joins the threads.** 


Below is pseudocode implementation using two semaphores and a mutex. 

Initialization:
```c
pthread_mutex_t mutex; // Used to provide mutual exclusion for critical section
sem_t empty = N;       // Number of empty slots in buffer
sem_t full = 0         // Number of slots filled
int in = 0;            // Place to add iten
int out = 0;           // Place to remove item
int buffer[BUFFER_SIZE];
```

Producer:
```c
while(True) {
   // produce an item
   sem_wait(empty); // wait when there are no empty slots
   pthread_mutex_lock(&mutex);
   buffer[in] = item;
   in = (in+1)%BUFFER_SIZE;
   pthread_mutex_unlock(&mutex);
   sem_post(full); // Signal to consumer that buffer has some data to be consumed
}
```

Consumer:
```c
while(True) {
   sem_wait(full); // wait when there are no full slots
   pthread_mutex_lock(&mutex);
   item = buffer[out];
   out = (out+1)%BUFFER_SIZE;
   pthread_mutex_unlock(mutex);
   sem_post(empty); // Signal the producer that buffer slots are emptied and they can produce more
   //consume the item
}
```


# Deliverables

1. Commit and push a modified `relay_sem.c`.
2. Commit and push a modified `producer-consumer_condvar.c`
3. Commit and push a modified `producer-consumer_sem.c`
4. Upload a zipfile of your repository to Gradescope.
<!--- 5. Complete the [Lab 9 Quiz](https://northeastern.instructure.com/courses/86682/quizzes/383585) on canvas. --->

