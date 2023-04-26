# Tools of the Trade: GDB

In this lab, we'll experiment with an interesting tool that is useful in the development cycle of Assembly (and C) programs: [GDB](https://www.gnu.org/software/gdb/).

# Part 1 - GDB

GDB is a widely used debugger. It helps us track down errors in our program. As an example, you may use gdb to step through programs and execute one line at a time to understand what is going on. Learning how to debug errors through a debugger is an essential skill for all computer scientists!

## Informing the compiler you want to debug
To use GDB, you have to tell the compiler you want debugging information included in the executable. For GCC, use the ```-g``` switch.
```bash
$ gcc -g hello.c -o hello
$ gcc -g hello.s -o hello
```
Now, you should be able to see the program listing in the debugger. 

## GDB Training

1. To start, skim this tutorial and try out some of the examples: <https://beej.us/guide/bggdb/>

2. Alternatively, you may want to take a look at this Q&A style tutorial: <http://www.unknownroad.com/rtfm/gdbtut/gdbtoc.html>

3. Finally, skim this cheatsheet and keep it at hand if you need to figure out a command quickly: <https://cs.brown.edu/courses/cs033/docs/guides/gdb.pdf>


## Some useful gbd commands when working with assembly


1. `list`

   Shows a listing of the source code. Repeatedly calling `list` "scrolls down" in the file, while calling `list -` scrolls back up. You can center your listing on specific lines by referring to a function or the line itself. 


1. `break` 

   Allows you to set a *breakpoint*, that is, a point at which the execution will be interrupted. You can use line numbers or function names. 
  
   - `break main` - this will set the breakpoint at the entry to the `main` function. **Super useful.**

1. `run`

   Runs the program and stops at the first breakpoint if one is set. 

1. `continue`

   Will continue executing the program until the next breakpoint is hit, the program, finishes, or crashes. 
   
1. `next`

   Advances the program execution by one line. 

1. `step`

   Advances the program execution by one line, but, unlike `next`, enters functions when it comes across a function call. 

1. `print`

   You can inspect various data while the program is paused using the `print` command. To print the contents of a register: `print $rbp`.

1. `x`

    Examine memory while the program is paused.  To print the contents of the memory address a register points to: `x $rsi`.

1. `backtrace` or `bt`

   Remember Java stack traces? The output of `bt` should be familiar. It shows you the stack of functions.

1. `up` and `down`

   You can move up and down the function stack. 

1. `help`

   `help [command]` displays a short description of `command` and any parameters it can take
   
1. `info [subcommand]`

   Shows things about the program being debugged.  For example:
   
   - `info all-registers` -- List of all registers and their contents, for selected stack frame.
   - `info breakpoints` -- Status of specified breakpoints (all user-settable breakpoints if no argument).
   - `info frame` -- All about the selected stack frame.
   - `info registers` -- List of integer registers and their contents, for selected stack frame.
   - `info scope` -- List the variables local to a scope.

## Using the TUI mode

GDB provides a nice(ish) split screen mode. It can be enabled in gdb using `tui enable` or starting the debugger as follows, `gdb -tui [executable_program]`. The upper part of the screen will show the source code with the current line highlighted, the lower part will provide you with a gdb prompt where you can keep entering commands. Try `layout asm`, switch back with `layout src`, then try `layout split`. Do `next` a few times. Try `layout regs`.

More things to try in TUI mode:

- Ctrl+L will clear the screen after the text prints to clean it up.
- Now try hitting [Ctrl+x 2] (Ctrl-x followed by 2)
  - You will now see multiple windows with the assembly and the source code.
  - Pressing [Ctrl+x o] will cycle between the windows.
  - Hitting [Ctrl+x 2] a couple of times will cycle through the windows to show what is in the registers.
    - Typing `tui reg` will give some options to see specific registers.
      - For example, `tui reg float` shows the floating registers.
      - For example, `tui reg general` displays the general purpose registers.
 - Hit [Ctrl+x 1] to go back to the first view and see your source code.
   - Typing in some commands like 'layout src' will bring you back to your source view.
     - Typing in 'layout asm' will show you the assembly.
- More: [https://sourceware.org/gdb/onlinedocs/gdb/TUI-Commands.html#TUI-Commands](https://sourceware.org/gdb/onlinedocs/gdb/TUI-Commands.html#TUI-Commands)
- More: [https://sourceware.org/gdb/onlinedocs/gdb/TUI-Keys.html#TUI-Keys](https://sourceware.org/gdb/onlinedocs/gdb/TUI-Keys.html#TUI-Keys)
- More resources on GDB TUI: [https://sourceware.org/gdb/onlinedocs/gdb/TUI.html](https://sourceware.org/gdb/onlinedocs/gdb/TUI.html)




# Part 2 - Stack and Memory

Note: If you struggle to complete a working program, at least explain your thinking in the comments.


## Task 1: Experimenting with memory and the stack

The file [`exp.s`](exp.s) is a skeleton for the experiments in today's lab. 
For each experiment, copy the given code into the specified file (`exp1.s`, `exp2.s`, etc.).


There are two types of questions posed in the code:
1. `GUESS`: _Before_ running the program, write down your hypotheses about what the printed values will be. For addresses, write down relative values. For example:

   > After allocating *x* bytes, the *y* will be added to the contents of rsp.

   or

   > After entering the function, the new value of rsp will be `rsp' = rsp + 7` (where rsp is the state at the previous printout).

   or

   > The label `foo` comes 40 bytes after the label `bar`. 

   

2. `OBSERVE`:  Compile and run the experiment using `gcc -g [filename.s]  -o filename`.  Use gdb as `gdb -tui filenane`.  _While_ running the program in gdb, write down the values you observe in gdb for the each question. Briefly write down how your hypothesis differed.

Before diving in, skim through the following:

- [Stack frame layout on x86-64](https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64/)
- Wikipedia: [Call stack](https://en.wikipedia.org/wiki/Call_stack)

#### Experiment 1: Push/pop

We can access the hardware (call) stack similarly to any stack structure: using the *push* and *pop* operations. However, since the top of the stack is an address in the rsp register, we also have the freedom of accessing elements of the stack directly: by using offsets.

1. **Edit**: Copy `exp.s` to `exp1.s`.  Insert the code below where indicated in the file.   
1. **Compile**: `gcc -g exp1.s -o exp1`
1. **Run**: `gdb -tui exp1` 
  - You may find  `layout src`,  `layout reg` or `layout split` useful in gdb.

**Remember that the stack grows downward!**

```asm
main:
  # stack manipulation
  # what is the initial value of the stack, i.e., %rsp [print $rsp]
  push $1
  push $2
  push $3

  # GUESS: how many bytes were added to the stack, 
  # i.e. compare the current value of %rsp to its value before the pushes [print $rsp]

  pop %rax
  # GUESS: What is in %rax? [print $rax]

  # GUESS: What is on the top of the stack now, i.e., (%rsp)? [x $rsp]

  # GUESS: What is the item right below the top of the stack, i.e., 8(%rsp) [x $rsp+8]

  # drop the top of the stack
  add $8, %rsp

  pop %rbx
  # GUESS: What is in %rbx? [print $rbx]
  
  ret
```


#### Experiment 2: Sections and labels

Sections in an assembly file are directly mapped into sections in the resulting executable. These, in turn, map directly into the program's address space. Let's take a look how sections and symbolic labels are arranged in memory.

1. **Edit**: Copy `exp.s` to `exp2.s`.  Insert the code below where indicated in the file.   
1. **Compile**: `gcc -g exp2.s -o exp2`
1. **Run**: `gdb -tui exp2` 
  - You may find  `layout src`,  `layout reg` or `layout split` useful in gdb.

```asm
main:
  enter $0, $0

  # where does .text start? [p start_of_text]

  # where does .data start? [info var start_of_data]

  # addresses of the label in the .text section [p main]

  # GUESS: What are the addresses of label in the .data section? [i var stuff]

  # GUESS: Contents of address pointed to by `stuff` [p (long) stuff]

  # GUESS: What's the 5th element of `stuff`? [p $rax + (4*8)]
  mov $stuff, %rax

  # GUESS: How big is stuff? [p (long[8]) stuff]

  leave
  ret
```

#### Experiment 3: Return addresses

When calling a function, the call instruction will do the following:
1. Push the address of the instruction _immediatelly after_ the call instruction. This address is used by the `ret` instruction.
2. Perform an unconditional jump to the address (label) given as the operand.

The purpose of this task is to confirm this behavior.

1. **Edit**: Copy `exp.s` to `exp3.s`.  Insert the code below where indicated in the file.   
1. **Compile**: `gcc -g exp3.s -o exp3`
1. **Run**: `gdb -tui exp3` 

```asm
dummy1:
  # GUESS: What is on top of the stack? Here, compare with the value printed by `p $after_dummy` [x $rsp]

  # GUESS: What is the address of the top of the stack, relaive to the first showq in `main`? [p $rsp]
  nop
  ret

main:
  enter $0, $0

  # Print the current stack pointer before calling `dummy1` [p $rsp]

  call dummy1

after_dummy1:
  # GUESS: What is the current stack pointer? [p $rsp]
  
  # the address of the first instruction after returning from `dummy1`[p $after_dummy1 or x $rip]
  # compare with what's on the stack after entering `dummy1`
  nop

  leave
  ret
```

#### Experiment 4: Stack frames

The `enter` instruction sets up a [stack frame](https://en.wikipedia.org/wiki/Call_stack#STACK-FRAME), while `leave` destroys it. A stack frame is the area bounded by the register rbp on the top and the register rsp on the bottom. Usually, rbp does not change for a single function call, while rsp changes when we push or pop things to or off the stack. 

The `enter` instruction will:
1. `push %rbp` - save the bottom (base) of the *previous* stack frame
2. `mov %rsp, %rbp` - set the current position as the base of the *current* frame
3. `sub $N, %rsp` - allocate `N` bytes on the stack (for local variables)

The `leave` instruction will:
1. `mov %rbp, %rsp` - deallocate the stack frame
2. `pop %rbp` - restore the previous stack frame

Here, experiment with the effect the instructions have on the rbp/rsp register pair. 


1. **Edit**: Copy `exp.s` to `exp4.s`.  Insert the code below where indicated in the file.   
1. **Compile**: `gcc -g -no-pie exp4.s -o exp4`
1. **Run**: `gdb -tui exp4` 

   ```asm
   dummy2:
     # state of rbp/rsp before enter sets up the stack frame
     # [p $rsp]
     # [p $rbp]
     nop

     enter $0, $0

     # GUESS: What is the state of rbp/rsp after stack frame is set up?
     # [p $rsp]
     # [p $rbp]
     nop
     
     leave

     # GUESS: What is the state of rbp/rsp after the stack frame is destroyed?
     # [p $rsp]
     # [p $rbp]
     nop
     
     ret

   main:
     enter $0, $0

     call dummy2

     leave
     ret
   ```

2. Hypothesize and run the experiment as is.

3. **Modify the enter instruction in `dummy2` to allocate 8, 16, then 32 bytes on the stack.** What are your updated hypotheses?

## Task 2: Sum the args (array addressing)

The file [`args.s`](args.s) contains a program that prints all its command line arguments (including the program name) to the terminal. First, copy the file into `sum_args.s`. Use the existing loop as a template and modify `sum_args.s` to do the following:

1. Initialize a running sum as a stack-allocated variable `-8(%rbp)` to `0`. You will need to use `movq` to specify the size of the variable.
2. In a loop, convert each argument (remember to skip the program name!) to a long integer and add it to the running sum. Hint: you will want to use [`atol`](https://pubs.opengroup.org/onlinepubs/007904975/functions/atol.html).
3. Finally, print the sum of all the arguments.

```
$ ./sum_args 10 30 20 12
Sum: 72
```

## Task 3: Reverse the args (working with the stack)

Copy the file [`args.s`](args.s) into `reverse_args.s`. Modify this new program to reverse its arguments as follows. You will need to write two loops.

1. In the first loop, push each argument's address onto the stack. Again, skip the program name.
2. Once all arguments have been pushed, in the second loop, pop each argument and print it, adding a space after it.

```
$ ./reverse_args 1 2 3 4 5 6
6 5 4 3 2 1
```


## Deliverables

1. Commit and push an updated `exp.s` and any copies of it created, i.e., `exp1.s`, `exp2.s`, `exp3.s`,`exp4.s`
2. Commit and push your answers in `questions.txt`.
3. Commit and push `sum_args.s`.
4. Commit and push `reverse_args.s`.


## Additional Resources

- A full 90+ minutes of GDB debugging (don't watch in class without headphones!)
	- [https://www.youtube.com/watch?v=713ay4bZUrw](https://www.youtube.com/watch?v=713ay4bZUrw)
- GDB is very powerful, check out the documentation here!
	- <https://sourceware.org/gdb/current/onlinedocs/gdb/> 

