# Threaded Merge Sort Experiments


## Host 1: Personal MacOS Machine

- CPU: Apple M1 Pro
- Cores: 8
- Cache size (if known):
- RAM: LPDDR5 (16GB)
- Storage (if known): 512GB
- OS: macOS Monterey

### Input data

A .txt file containing a shuffled list of 10 million numbers (ranging from 1 to 10000000). I created this with the shuf -i1l-N < filename.txt instruction. Using msort to sort this took an average of 0.273665 seconds over three attempts.

### Experiments

#### 1 Threads

Command used to run experiment: `MSORT_THREADS=1 ./tmsort 1000000 < million.txt`

Sorting portion timings:

1. 0.202014 seconds
2. 0.204514 seconds
3. 0.199026 seconds
4. 0.197740 seconds

#### 3 Threads

Command used to run experiment: `MSORT_THREADS=3 ./tmsort 1000000 < million.txt`

Sorting portion timings:

1. 0.158403 seconds
2. 0.162559 seconds
3. 0.157364 seconds
4. 0.154671 seconds

#### 8 Threads

Command used to run experiment: `MSORT_THREADS=8 ./tmsort 1000000 < million.txt`

Sorting portion timings:

1. 0.098438 seconds
2. 0.097602 seconds
3. 0.109671 seconds
4. 0.134576 seconds

#### 16 Threads

Command used to run experiment: `MSORT_THREADS=8 ./tmsort 1000000 < million.txt`

Sorting portion timings:

1. 0.098956 seconds
2. 0.120946 seconds
3. 0.097171 seconds
4. 0.122148 seconds

It seems after 8 threads (same as the amount of cores I have on my Mac), increasing the threads doesn't improve performance.


## Host 2: Personal Windows Machine running WSL2


- CPU: Intel Core i7-7700k
- Cores: 4
- Cache size (if known): 9.256MB
- RAM: 3000MHz (16GB)
- Storage (if known): 3TB
- OS: Windows 10

### Input data

A .txt file containing a shuffled list of 10 million numbers (ranging from 1 to 10000000). I created this with the shuf -i1l-N < filename.txt instruction. Using msort to sort this took an average of 0.194692 seconds over three attempts.

### Experiments

#### 1 Threads

Command used to run experiment: `MSORT_THREADS=1 ./tmsort 1000000 < million.txt`

Sorting portion timings:

1. 0.190804 seconds
2. 0.195889 seconds
3. 0.199556 seconds
4. 0.187369 seconds

#### 4 Threads

Command used to run experiment: `MSORT_THREADS=4 ./tmsort 1000000 < million.txt`

Sorting portion timings:

1. 0.152600 seconds
2. 0.140981 seconds
3. 0.149171 seconds
4. 0.119837 seconds

#### 8 Threads

Command used to run experiment: `MSORT_THREADS=8 ./tmsort 1000000 < million.txt`

Sorting portion timings:

1. 0.119919 seconds
2. 0.135585 seconds
3. 0.132395 seconds
4. 0.120882 seconds



## Observations and Conclusions

It seems as the number of threads approaches the number of cores on the machine, the perforamce gained from adding a thread decreases, and acts like it reaches a maximum. 
This is because a thread requires a core so work on it individually, meaning when the number of threads exceeds the number of cores, some cores will be responsible for have more than one thread.


