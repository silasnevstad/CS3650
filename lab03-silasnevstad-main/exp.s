
.global main

.text
start_of_text:


# BEGIN pasted code

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

