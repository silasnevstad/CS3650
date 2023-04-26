# x86-64 Assembly 

> "Nearing Machine Code Representation"


In today's lab you are going to get some practice looking at and writing assembly.

# Part 1 - Assembly - A first program

## Task 1

The first task is to actually just write a 'hello world' in assembly.

- Enter the following text into a file called [hello.s](./hello.s)
 
  ```asm	  
  # Hello World in Assembly using printf

  .global main

  .text

  main:
      enter $0, $0

      mov $message, %rdi
      mov $0, %al
      call printf

      leave
      ret

  .data
  message:
      .asciz "Hello, World!\n"
  ```
- Save the file and compile it using `gcc hello.s -o hello`. If you get errors mentioning PIE (position-independent executable), try `gcc -no-pie hello.s -o hello`.
    - This creates an executable file called `hello` in the local diretory
    - In this case, `gcc` will compile the assembly source and then use the linker (a tool called `ld`) with the appropriate options and insert boilerplate code serving as the entry point to the program
    - Run `man ld` and `man gcc` to learn more.
  - When this has been completed, push hello.s to your repo and take a look at the observations below.

There are a few interesting things with this program.
1. First, there is a *global* symbol `main`. This tells the compiler that `main` is not private to the current compilation unit and should be visible to other source / object files. You can learn more here: http://web.mit.edu/gnu/doc/html/as_7.html#SEC89
2. The next directive (remember, lines that start with a '.' are directives) is `.text`. This starts the `.text` section where our instructions live. https://stackoverflow.com/questions/14544068/what-are-data-and-text-in-x86
3. The next few lines are moving some values into registers. The first moves the *address* of the string defined as `message` into `%rdi`, the first (and only) argument to `printf`.
4. The line `mov $0, %al` is necessary for calling functions with variable numbers of arguments, such as `printf`. For now, just remember: if you call `printf`, `fprintf`, `scanf`, etc. in assembly, you need to zero `%al`
5. The directive `.data` starts the data section which contains definitions of some, well, data.
6. Finally, at the end, in the `.data` section, there is a label (`message:`) with a string literal (`.asciz "Hello, World\n"`). <http://bravegnu.org/gnu-eprog/asm-directives.html#_literal_asciz_literal_directive>

## Task 2

Here we will extend `hello.s` with some math and printing a numeric result.

- Now modify `hello.s` as follows:
  - Append the following lines to the bottom of `hello.s` (this will add it to the `.data` section):
    ```asm
    a: .quad 0x3908
    b: .quad 0x721
    c: .quad 16
    ```
  - Think of `a`, `b` and `c` as global variables. You can use a global variable as you would use an immediate value: e.g., `mov a, %r10`
  - In the `.text` section, after the "Hello world" call to `printf`, write assembly code calculating the formula: `(2 * a + b * c) >> 4`.
  - Save the result in a register of your choice
  - Recall that `>>` is a right shift and you can use the instruction `shr` or `sar`
  - Add a format string called `long_format` to the `.data` section:
    ```asm
    long_format:
        .asciz "%ld\n"
    ```
  - Call `printf` so that it prints the result using the above format string
  - This code corresponds to the C line:
    ```c
    printf("%ld\n", (2 * a + b * c) >> 4);
    ```
  - Feel free to copy and paste it into a C file and compile it to check your result

If you have trouble getting the math right here, move on to the next task after you are printing some determinate number using `printf`.

## Aside - Machine Representation of Numbers

  * Note: it may be beneficial to look at this ascii table to see how numbers and letters are represented. See https://www.asciitable.com or `man ascii`.  Remember, we do not have a 'text' datatype in assembly. Text instead is represented by numbers shown in the ascii table.

# Part 2 - Writing more Assembly


## Task 1

For this task, we will contine to extend `hello.s` with calling a function and printing a numeric result.

- Now modify `hello.s` as follows:
  - Append the following lines to the bottom of `hello.s` (this will add it to the `.data` section):
    ```asm
    d: .quad -32
    e: .quad 64
    ```
    
  - Write assembly code that calls the function `long labs(long)` with the value of `d` in input, i.e., `labs(d)`.  Remember the registers `%rdi` and `%rsi` are used to pass the first two arguments to called functions.  For more information on `labs`, see its man page, `man labs`.
  

  - Call `printf` so that it prints the result using the above format string



## Task 2

For this task, we will contine to extend `hello.s` with some math, calling a function, and printing a numeric result.

- Now modify `hello.s` as follows:
  - Write assembly code that calculates the formula: `labs((d * e) << 3)`.
  - Call `printf` so that it prints the result using the previously defined string







# More resources to help

- Matt Godbolt has written a great tool to help understand assembly generated from the compiler. 
  - https://godbolt.org/
- An assembly cheat sheet from Brown
  - https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf
- MIT Cheat sheet
  - http://6.035.scripts.mit.edu/sp17/x86-64-architecture-guide.html

# Deliverables


1. Complete the [hello.s](./hello.s) from parts 1 and 2.  There should be a single line of output for each of the 4 tasks.  
2. Add and commit hello.s.
3. Complete the Lab 2 Quiz in Canvas. 
4. Submit your repository to Gradescope.

  

# Going Further

- (Optional) Try the objdump example to read the disassembly from your programs executables. Observe how close the output is to the compiler generated output.
- (Optional) Using `objdump -d` on the `hello` executable, look for the label `_start`, this is the real entry point of the program. It calls `main`.
