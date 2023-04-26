# Write the assembly code for main
#Usage: ./compare <arg1> <arg2>

.global main

.text

main:
  enter $0, $0

  movq 8(%rsi), %r12
  movq 16(%rsi), %r13
  movq 32(%rsi), %r14

  cmp $0, %r12
  je error
  cmp $0, %r13
  je error

  mov %r12, %rdi
  call atol
  mov %rax, %r12

  mov %r13, %rdi
  call atol
  mov %rax, %r13

  cmp $0, %r14
  je error

  cmp %r13, %r12
  je equal
  jg greater
  jl less

  jmp error
  

equal:
  mov $resultEqualString, %rdi
  mov $0, %al
  call printf
  jmp done

greater:
  mov $resultGreaterString, %rdi
  mov $0, %al
  call printf
  jmp done

less:
  mov $resultLessString, %rdi
  mov $0, %al
  call printf
  jmp done

error:
  mov $errorString, %rdi
  mov $0, %al
  call printf
  jmp done

done:
  leave
  ret


data:
  errorString: .string "Two arguments required.\n"
  resultLessString: .string "less\n"
  resultEqualString: .string "equal\n"
  resultGreaterString: .string "greater\n"


format:
  .asciz "%ld\n"


