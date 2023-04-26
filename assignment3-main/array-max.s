# Write the assembly code for array_max
# Usage: ./array-max <number-of-elements> <element_1> <element_2> ...

.global main

.text

main:
  push %r12
  push %r13
  push %r14
  enter $0, $0
  
  mov %rdi, %r12 # number of arguments
  mov %rsi, %r13 # argument addresses
  mov $0, %r14 # counter
  mov $0, %r15 # initial max
  
loop:
  cmp %r12, %r14
  jge loop_end
  
  mov (%r13, %r14, 8), %r11

  mov %r11, %rdi
  call atol
  mov %rax, %rbx

  cmp %r15, %rbx
  jg newMax

  inc %r14
  jmp loop

loop_end:
  cmp $1, %r14
  je done

  mov $format, %rdi
  mov %r15, %rsi
  mov $0, %al
  call printf

  leave
  pop %r14
  pop %r13
  pop %r12
  ret

newMax:
  #mov $maxFoundString, %rdi
  #mov %r14, %rsi
  #mov $0, %al
  #call printf

  mov %rbx, %r15
  
  inc %r14
  jmp loop
  
done:
  mov $errorString, %rdi
  mov $0, %al
  call printf

  leave
  pop %r14
  pop %r13
  pop %r12
  ret

.data
  errorString: .string "Missing arguments.\n"
  maxFoundString: .string "New Max Found.\n"

format: 
  .asciz "%ld\n"
