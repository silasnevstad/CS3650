# Your code here

.global main

.text

main:
    enter $0, $0
	
    mov $message, %rdi
    mov $0, %al
    call printf
    
    mov $a, %rsi
    imul $2, %rsi
    
    mov $b, %rdx
    imul $c, %rdx
    
    add %rdx, %rsi
    sar $4, %rsi

    mov $long_format, %rdi
    mov $0, %al
    call printf
    
    mov $d, %rsi
    call labs
    mov $long_format, %rdi
    call printf
    
    mov $d, %rsi
    imul $e, %rsi
    shl $3, %rsi
    mov $long_format, %rdi
    call printf    
          
    leave
    ret

.data
message:
   .asciz "Hello World!\n"

a: .quad 0x3908

b: .quad 0x721

c: .quad 16

d: .quad -32

e: .quad 64

long_format:
    .asciz "%1d\n"
