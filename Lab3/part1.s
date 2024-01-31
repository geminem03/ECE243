/* Program to Count the number of 1's in a 32-bit word,
located at InputWord */

.global _start
_start:
	movia r8, InputWord
    ldw r9, 0(r8)  # load value at the address stored in r8 into r9

	movi r10, 0 # initialize counter to zero

countloop: 
	andi r11, r9, 0x1 # multiplies 1 with least sig bit (furthest left)
	add r10, r10, r11 # stores result in counter 
	srli r9, r9, 1 # shift right to next bit

	bne r9, r0, countloop # if input is not zero still have bits to check 

	movia r12, Answer # load address of Answer in r12
	stw r10, 0(r12) # store value from counter into Answer
	

InputWord: .word 0x00001111
Answer: .word 0
