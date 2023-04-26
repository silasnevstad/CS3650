/** 
 * Threaded Merge Sort
 *
 * Modify this file to implement your multi-threaded version of merge sort. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <unistd.h>

#include <assert.h>

#include <pthread.h>

#define tty_printf(...) (isatty(1) && isatty(0) ? printf(__VA_ARGS__) : 0)

#ifndef SHUSH
#define log(...) (fprintf(stderr, __VA_ARGS__))
#else 
#define log(...)
#endif

// a struct to hold arguements for a threadTask
typedef struct threadTask {
    long *arr; // the array to be sorted
    long *res; // the array to store the result
    int from; // the start index of the array to be sorted
    int to; // the end index of the array to be sorted
} threadTask;

/** The number of threads to be used for sorting. Default: 1 */
int thread_count = 4;

long *res = NULL; // global variable to store the result

/**
 * Compute the delta between the given timevals in seconds.
 */
double time_in_secs(const struct timeval *begin, const struct timeval *end) {
  long s = end->tv_sec - begin->tv_sec;
  long ms = end->tv_usec - begin->tv_usec;
  return s + ms * 1e-6;
}

/**
 * Print the given array of longs, an element per line.
 */
void print_long_array(const long *array, int count) {
  for (int i = 0; i < count; ++i) {
    printf("%ld\n", array[i]);
  }
}

/**
 * Merge two slices of nums into the corresponding portion of target.
 */
void merge(long nums[], int from, int mid, int to, long target[]) {
  int left = from;
  int right = mid;

  int i = from;
  for (; i < to && left < mid && right < to; i++) {
    
    if (nums[left] <= nums[right]) {
      target[i] = nums[left];
      left++;
    }
    else {
      target[i] = nums[right];
      right++;
    }
  }
  if (left < mid) {
    memmove(&target[i], &nums[left], (mid - left) * sizeof(long));
  }
  else if (right < to) {
    memmove(&target[i], &nums[right], (to - right) * sizeof(long));
  }
}

/**
 * Sort the given slice of nums into target.
 *
 * Warning: nums gets overwritten.
 */
void merge_sort_aux(long nums[], int from, int to, long target[]) {
  if (to - from <= 1) {
    return;
  }

  int mid = (from + to) / 2;
  merge_sort_aux(target, from, mid, nums);
  merge_sort_aux(target, mid, to, nums);
  merge(nums, from, mid, to, target);
}


/**
 * Sort the given array and return the sorted version.
 *
 * The result is malloc'd so it is the caller's responsibility to free it.
 *
 * Warning: The source array gets overwritten.
 */
long *merge_sort(long nums[], int count) {
  long *result = calloc(count, sizeof(long));
  assert(result != NULL);

  memmove(result, nums, count * sizeof(long));

  merge_sort_aux(nums, 0, count, result);

  return result;
}

/*
 * Sort the given array given a thread.
 * This means this only sorts the section the given thread is responsible for.
 * 
 * Warning: The source arrays get overwritten.
 */
void *merge_sort_mt(void* arg) {
  threadTask *threadT = arg; // cast the arg to a threadTask

  merge_sort_aux(threadT->arr, threadT->from, threadT->to, res); // sort the portion of the array the thread is responsible for
}


/**
 * Based on command line arguments, allocate and populate an input and a 
 * helper array.
 *
 * Returns the number of elements in the array.
 */
int allocate_load_array(int argc, char **argv, long **array) {
  assert(argc > 1);
  int count = atoi(argv[1]);

  *array = calloc(count, sizeof(long));
  assert(*array != NULL);

  long element;
  tty_printf("Enter %d elements, separated by whitespace\n", count);
  int i = 0;
  while (i < count && scanf("%ld", &element) != EOF)  {
    (*array)[i++] = element;
  }

  return count;
}

/*
 * Divide the numbers into thread tasks so that each task focues on a
 * seperate part of the list (no two threads work on the same part).
 * Therefore, there is no need to lock the array.
 * 
 * Return the array of thread tasks.
 */
threadTask *divide_list_to_threads(threadTask *tasks, int count, long *nums, long *result) {
    int maxIndex = count; // the max index of the list

    for (int i = 0; i < thread_count; i++) { // loop through the number of threads requested
      threadTask currTask;

      currTask.arr = nums; // the array stays the same

      currTask.res = result; // so does the result

      // now we give each thread it's own portion of the numbers to focus on
      currTask.from = i * (count / thread_count); // from is this thread * (total numbers / total threads)

      if (i + 1 == thread_count) {
       currTask.to = maxIndex; // if this is the last thread make sure to inlcude the last index.
      } else {
        currTask.to = (i + 1) * (count / thread_count); // else to is this thread + 1 * (total numbers / total threads)
      }

      tasks[i] = currTask; // append this thread task to the array of tasks
    }

    return tasks; // return the array of tasks
}

/*
 * Sort the given array using multi-threading.
 * 
 * Does this by dividing the array into sections and giving each section to a
 * seperate thread. No two threads deal with the same section, therefore no
 * locking is needed.
 * 
 * Returns the sorted array.
 */
long *sort_mt(long *nums, int count) {
  pthread_t threads[thread_count]; // array of threads

  
  res = calloc(count, sizeof(long)); // result from threads

  assert(res != NULL);

  memmove(res, nums, count * sizeof(long)); // move the numbers to the result array

  threadTask threadTasks[thread_count]; // array of thread tasks
  memcpy(threadTasks, divide_list_to_threads(threadTasks, count, nums, res), sizeof(threadTasks)); // have to memcpy because divide_list_to_threads returns a pointer to a local variable
                                                                                                   // which is destroyed after the function returns

  // Creating the threads
  for (int i = 0; i < thread_count; i++) {
    assert(pthread_create(&threads[i], NULL, merge_sort_mt, &threadTasks[i]) != -1); // create the thread and assert that it was created successfully
  }

  // Joining the threads
  for (int i = 0; i < thread_count; i++) {
    assert(pthread_join(threads[i], NULL) != -1); // join the thread and assert that it was joined successfully
  }

  /* At this stage, each thread has sorted its part of the array
   * now we have a list with sorted sections we have to merge sort to get our final result
  */
  int i = 1;

  while (i < thread_count) { // loop through number of threads
    memmove(nums, res, count * sizeof(long)); // move the result to the numbers array
 
    int mid = i * (count / thread_count); // mid is this thread * (total numbers / total threads)
    int to = (i + 1) * (count / thread_count); // to is this thread + 1 * (total numbers / total threads)
    if (i + 1 == thread_count) {
      to = count; // if this is the last thread make sure to inlcude the last index.
    }
    
    merge(nums, 0, mid, to, res); // merge the current section with the next one
    i++;
  }

  return res; // return the result
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <n>\n", argv[0]);
    return 1;
  }

  struct timeval begin, end;

  // get the number of threads from the environment variable SORT_THREADS
  if (getenv("MSORT_THREADS") != NULL)
    thread_count = atoi(getenv("MSORT_THREADS"));

  log("Running with %d thread(s). Reading input.\n", thread_count);

  // Read the input
  gettimeofday(&begin, 0);
  long *array = NULL;
  int count = allocate_load_array(argc, argv, &array);
  gettimeofday(&end, 0);

  log("Array read in %f seconds, beginning sort.\n", 
      time_in_secs(&begin, &end));
 
  /*
  * If the number of threads is 1, then we 
  * just use the single threaded version.
  */   
  if (thread_count > 1) {
    // Sort the array using multi-threading
    gettimeofday(&begin, 0);
    long *result = sort_mt(array, count);
    gettimeofday(&end, 0);

    log("Sorting completed in %f seconds.\n", time_in_secs(&begin, &end));

    // print the sorted array
    gettimeofday(&begin, 0);
    print_long_array(result, count);
    gettimeofday(&end, 0);

    // free the memory allocated for the array and result
    free(array);
    free(result);
  }
  else {
    // Sort the array using a single thread
    gettimeofday(&begin, 0);
    long *result = merge_sort(array, count);
    gettimeofday(&end, 0);

    log("Sorting completed in %f seconds.\n", time_in_secs(&begin, &end));

    // print the sorted array
    gettimeofday(&begin, 0);
    print_long_array(result, count);
    gettimeofday(&end, 0);

    // free the memory allocated for the array and result
    free(array);
    free(result);
  }

  log("Array printed in %f seconds.\n", time_in_secs(&begin, &end));

  return 0;
}
