.text
/* Program to Count the number of 1’s in a 32-bit word,
located at InputWord */
.global _start
_start:
	movia r9,InputWord
	ldw r4,(r9) # Load inputword from address in R4
	movi r2,0 #initialize 1-counter in R2 
	movi r7,32 # store word-size in R7 
	movi r10, 0 #initialize index to 0 in R10
	call ONES
	movia r12,Answer
	stw r2,(r12)
	br endiloop
	
ONES:
	andi r11,r4,1 #Check if the LSB of r4 is 1, if it is r11 = 1
	add r2,r2,r11 # 1-counter += r11
	addi r10,r10,1 # Incriment r10 by 1
	beq r10,r7,done
	srli r4,r4,1
	br ONES

done:
	ret
	
endiloop: 
	.equ LEDs, 0xFF200000
	movia r25, LEDs
	stwio r2, (r25)
	br endiloop

.data
InputWord: .word 0x01000000
Answer: .word 0