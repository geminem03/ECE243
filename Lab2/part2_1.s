.text
.global _start
_start:

    movia r10, 8        # student number being searched for
    movia r11, Snumbers # sointer to the student numbers array
    movia r12, Grades   # pointer to the grades array
    
    ldw r15, (r11)      # load first student number into r15
    mov r18, r15        # store first student number in r18
    ldw r13, (r12)      # load the first grade into r13
    mov r19, r13        # store the first grade in r19
	
    movi r17, 0         # initialize loop counter

condition: 
    beq r15, r0, invalid # if current student number is zero, go to invalid
    
    beq r17, r15, after  # if loop counter is equal to the current student number, go to after
    
    beq r10, r15, equal  # if r10 (searched student number) is equal to the current student number, go to equal
	
    addi r11, r11, 4     # point to the next student number
    addi r12, r12, 4     # point to the next grade
	
    ldw r15, (r11)       # load next student number into r15
    ldw r13, (r12)       # load next grade into r13
	
    br condition         # repeat loop

equal:
    # if the student number is found, store the corresponding grade in r13
    br after

invalid: 
    # if the student number is zero, return error code (-1)
    movi r13, -1
    br after

after: 
    br after # end program

/* Snumbers is the "array," terminated by a zero, of the student numbers  */
Snumbers: .word 1,2,3,4,5,6,7,8,0
       
/* Grades is the corresponding "array" with the grades, in the same order*/
Grades: .word 11,12,13,14,15,16,17,18
