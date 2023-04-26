.global main

.text
start_of_text:


# BEGIN pasted code
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
# END pasted code

end_of_text:

.data
start_of_data:

stuff:
  .quad 1, 2, 3, 4, 5, 6, 7, 8

end_of_data:

size_of_stuff = end_of_data - stuff 
size_of_text = end_of_text - start_of_text
size_of_data = end_of_data - start_of_data
