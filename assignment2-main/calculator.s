# Usage: ./calculator <op> <arg1> <arg2>

.global main

.text

main:
  enter $0, $0

  movq 8(%rsi), %r12  # op = argv[1]
  movq 16(%rsi), %r13 # arg1 = argv[2]
  movq 24(%rsi), %r14 # arg2 = argv[3]


  # Hint: Convert 1st operand to long int
  mov %r13, %rdi
  call atol
  mov %rax, %r13
  # Hint: Convert 2nd operand to long int
  mov %r14, %rdi
  call atol
  mov %rax, %r14

  # Hint: Copy the first char of op into an 8-bit register
  mov %r12, %rdi

  mov $addD, %rsi
  call strcmp
  cmp $0, %rax
  je addition

  mov $subD, %rsi
  call strcmp
  cmp $0, %rax
  je subtraction

  mov $mulD, %rsi
  call strcmp
  cmp $0, %rax
  je multiply

  mov $divD, %rsi
  call strcmp
  cmp $0, %rax
  je division

  jne error


addition:
  add %r13, %rax
  add %r14, %rax
  jmp print_result

subtraction:
  mov %r13, %rax
  sub %r14, %rax
  jmp print_result

multiply:
  mov $1, %rax
  imul %r13
  imul %r14
  jmp print_result

division:
  mov $0, %rdx
  mov %r13, %rax
  idiv %r14
  jmp print_result

print_result:
  mov $resultString, %rdi
  #mov %r13, %rsi
  #mov %r12, %rdx
  #mov %r14, %rcx
  mov %rax, %rsi
  call printf
  jmp done

error:
  mov $errorString, %rdi
  mov %r12, %rsi
  call printf

done:
  leave
  ret


.data
  errorString: .string "Unknown operation\n"
  resultString: .string "%ld\n"
  addD: .string "+"
  subD: .string "-"
  mulD: .string "*"
  divD: .string "/"

format: 
  .asciz "%ld\n"

