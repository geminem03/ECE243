/* Subroutine to Count the number of 1's in a 32-bit word */

.global ONES
ONES: 
    mov r2, r0 # initialize counter to zero

    countloop: 
        /* same as part 1 */
        andi r8, r4, 0x1 
	    add r2, r2, r8 
	    srli r4, r4, 1 

        bne r4, r0, countloop 

    ret


.global _start
_start: 
    movia r4, InputWord
	ldw r4, 0(r4) # load the value from InputWord into r4
    call ONES

    movia r9, Answer # load address of Answer in r12
	stw r2, 0(r9) # store value from counter into Answer
    br after

after: 
    br after

InputWord: .word 0x4a01fead
Answer: .word 0