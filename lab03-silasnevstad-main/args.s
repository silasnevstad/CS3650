
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
  jge loop_end

  # print each argument
  mov $format, %rdi
  mov %r14, %rsi
  mov (%r13, %r14, 8), %rdx
  mov $0, %al
  call printf

  inc %r14
  jmp loop
  
loop_end:

  leave
  pop %r14
  pop %r13
  pop %r12
  ret

.data

format: 
  .asciz "%d: \"%s\"\n"

