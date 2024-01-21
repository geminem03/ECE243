.text
.global _start
_start:

    movia r10, 7       # student number being searched for
    movia r11, Snumbers # pointer to the student numbers array
    movia r12, Grades   # pointer to the grades array
    
    ldw r15, (r11)      # load first student number into r15
    mov r18, r15        # store first student number in r18
    ldb r13, (r12)      # load the first grade into r13 (using ldb for 8-bit byte)
    mov r19, r13        # store the first grade in r19
	
    movi r17, 0         # initialize loop counter

condition: 
    beq r15, r0, invalid # if the current student number is zero, go to invalid
    
    beq r17, r15, after  # if loop counter is equal to the current student number, go to after
    
    beq r10, r15, after  # if r10 (searched student number) is equal to the current student number, go to equal
	
    addi r11, r11, 4     # point to the next student number
    addi r12, r12, 1     # point to the next grade (using 1 for 8-bit byte)
	
    ldw r15, (r11)       # load the next student number into r15
    ldb r13, (r12)       # load the next grade into r13 (using ldb for 8-bit byte)
	
    br condition         # repeat loop

invalid: 
    # if the student number is zero, return error code (-1)
    movi r13, -1
    br after

after: 
	.equ LEDs, 0xFF200000
	movia r25, LEDs
	stwio r13, (r25)
    br after # end program

/* Snumbers is the "array," terminated by a zero, of the student numbers  */
Snumbers: .word 1,2,3,4,5,6,7,8,0
       
/* Grades is the corresponding "array" with the grades, in the same order */
Grades: .byte 11,12,13,14,15,16,17,18
