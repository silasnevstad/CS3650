
## Data Structures in C

- C only really provides two composite datatypes: arrays and structs
- Even these are basically just a slightly nicer interface to a chunk of memory
- Many other languages come with a library of common data structures; e.g.:
  - Java: `List`, `ArrayList`, `HashMap`, ...
  - C++: `vector`, `hash_map`, ...
- In C, we have to implement them ourselves or find a third-party library
- We build them from structs and arrays

### Variable-sized Arrays (Vectors)

- Arrays in C are kinda dumb: they don't grow automatically, they don't even know their size
- Let's call them *vectors*
- Interface we want:
    
    | Operation     | Meaning                                                       |
    |---------------|---------------------------------------------------------------|
    | `vect_new`    | Create an empty vector                                        |
    | `vect_add`    | Add an element to the end of the array                        |
    | `vect_set`    | Replace the element at the given index with the given element |
    | `vect_get`    | Return the element at the given index                         |

- Additionally we want the array to be **contiguous**, that means the elements are stored one after another in memory
- The above property allows us to iterate through the array efficiently in a for loop
- Linked lists are extendable, but iterating over them can be expensive - each element might be stored in a different region of the memory

Initial idea:
- `vect_new`: create an empty array
- `vect_add`: dynamically allocate a new slot for each new item
- need to store (in a struct): 
  - *size* - number of elements 
  - *data* - the C array containing the elements
- for extending the array, use [`realloc`](https://man7.org/linux/man-pages/man3/realloc.3.html):

  ```c
  long *my_array = malloc(old_size * sizeof(long));

    // ... do things ...

  my_array = realloc(my_array, new_size);
  ```

- read the documentation
- so, for every `vect_add`: 

   ```c 
   // ... 
   size += 1;
   data = realloc(data, size * sizeof(long));
   // ... add new element to the end
   ```

> Aside: think of realloc as doing this (pseudocode):
>
> ```c
> realloc(old, new_size) {
>     new = malloc(new_size);
>     copy(old, new);
>     free(old)
>     return new;
> }
> ```
>
> It is a little more fancy - it doesn't copy when it doesn't have to.

- How many array accesses do we need to perform to store 10 numbers in the array?


| item | size | steps      |                                         |
|------|------|------------|-----------------------------------------|
| 1    | 1    | 1          | start with empty, add 1 element         |
| 2    | 2    | 1 + 1 = 2  | copy original element and add a new one |
| 3    | 3    | 2 + 1 = 3  | copy elements; add a new one            |
| 4    | 4    | 3 + 1 = 4  | ...                                     |
| 5    | 5    | 4 + 1 = 5  | ...                                     |
| 6    | 6    | 5 + 1 = 6  | ...                                     |
| 7    | 7    | 6 + 1 = 7  | ...                                     |
| 8    | 8    | 7 + 1 = 8  | ...                                     |
| 9    | 9    | 8 + 1 = 9  | ...                                     |
| 10   | 10   | 9 + 1 = 10 | copy elements; add 10th                 |

- so, in total we are performing: `1 + 2 + ... + 10 = 55` steps
- in other words `(n * (n + 1)) / 2` (note: I made a mistake in class and replaced the + with -)
- so: `O(n^2)` (we ignore constant factors)
- not great...


Better idea: 
- Think ahead and preallocate more than needed
- Then reallocate as we run out of space
- Keep:
  - *size* - number of items
  - *capacity* - number of items we have memory for
  - *data* - the actual array storing the items
- Start with a small number for `capacity`, say 4
- Just keep adding elements while `size < capacity`
- If `size == capacity`, double `capacity` before adding element
- Rinse and repeat

| item | size | capacity | steps     |                                                   |
|------|------|----------|-----------|---------------------------------------------------|
| 1    | 1    | 4        | 1         |                                                   |
| 2    | 2    | 4        | 1         |                                                   |
| 3    | 3    | 4        | 1         |                                                   |
| 4    | 4    | 4        | 1         |                                                   |
| 5    | 5    | 8        | 4 + 1 = 5 | Ran out of space; double capacity; copy old items |
| 6    | 6    | 8        | 1         |                                                   |
| 7    | 7    | 8        | 1         |                                                   |
| 8    | 8    | 8        | 1         |                                                   |
| 9    | 9    | 16       | 8 + 1 = 9 | Ran out of space; double capacity; copy old items |
| 10   | 10   | 16       | 1         |                                                   |

- Now adding 10 items takes: `1 + 1 + 1 + 1 + 5 + 1 + 1 + 1 + 9 + 1 = 22 steps`
- That is toughly 2*n over time, so *amortized* it is O(n) - pretty good
- Note: an individual element might still take n steps (e.g., 5 and 9 in the example above)

- three fields: data, size (no of items), capacity (max no of items)

