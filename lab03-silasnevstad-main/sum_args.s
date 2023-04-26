
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

  mov $skipString, %rsi
  call strcmp
  cmp $1, %rax
  je adder

  inc %r14
  jmp loop
  
loop_end:

  leave
  pop %r14
  pop %r13
  pop %r12
  ret

adder:
  mov %r14, %rdi
  call atol
  mov %rax, %r14

  push %rsi

  inc %r14
  jmp loop 

.data
  skipString: .string "./sum_args"
format: 
  .asciz "%d: \"%s\"\n"


