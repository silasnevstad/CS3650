
.global main

.text

main:
  # we are using callee-save registers to preserve information when calling printf
  push %r12
  push %r13
  push %r14
  enter $8, $0

  mov %rdi, %r12   # number of arguments
  mov %rsi, %r13   # argument addresses
  mov $0, %r14     # counter

loop:
  cmp %r12, %r14
  jge loop2

  push %r13

  inc %r14
  jmp loop
  
loop2:
  pop %rax
  mov $0, %rcx
  cmp %rax, %rcx
  jge loop_end


  mov %rax, %rdx
  mov $0, %al
  mov $format, %rdi
  call printf
  
  inc %r14
  jmp loop2

loop_end:

  leave
  pop %r14
  pop %r13
  pop %r12
  ret

.data

format: 
  .asciz "%s\n"


