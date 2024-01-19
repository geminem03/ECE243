.text  # The numbers that turn into executable instructions
.global _start
_start:

    movia r10, 0 # student number being searched for
    movia r11, Snumbers # pointer to the student numbers array
    movia r12, Grades   # pointer to the grades array
    
    ldw r15, (r11)  # Load the first student number into r15
    mov r18, r15    # Store the first student number in r18
    ldw r13, (r12)  # Load the first grade into r13
    mov r19, r13    # Store the first grade in r19
    movi r8 , -1    # Initialize r8 to -1 (default value for not found)
    movi r17 , 0    # Initialize loop counter

condition: 
    beq r17 , r15 , after  # If the loop counter is equal to the current student number, go to after
    
    beq r10, r15, equal  # If r10 (searched student number) is equal to the current student number, go to equal
    addi r11, r11, 4    # Move the pointer to the next student number
	addi r12, r12, 4    # Move the pointer to the next grade
    ldw r13, (r12)      # Load the next grade into r13
    ldw r15, (r11)      # Load the next student number into r15
    br condition        # Repeat the loop

equal:
    mov r8, r13  # If the student number is found, store the corresponding grade in r8

after: 
    br after  # End of loop

result: .word 0  # Initialize the result variable with 0
        
/* Snumbers is the "array," terminated by a zero, of the student numbers  */
Snumbers: .word 1,2,3,4,5,6,7,8,0
       
/* Grades is the corresponding "array" with the grades, in the same order*/
Grades: .word 11,12,13,14,15,16,17,18
