
.global main

.text
start_of_text:


# BEGIN pasted code
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


