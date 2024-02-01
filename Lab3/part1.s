.text
/* Program to Count the number of 1’s in a 32-bit word,
located at InputWord */
.global _start
_start:
	movia r9,InputWord
	ldw r8,(r9) # Load inputword from address in R8
	movi r9,0 #initialize 1-counter in R9 
	movi r7,32 # store word-size in R7 
	movi r10, 0 #initialize index to 0 in R10
	
count:
	andi r11,r8,1 #Check if the LSB of r8 is 1, if it is r11 = 1
	add r9,r9,r11 # 1-counter += r11
	addi r10,r10,1 # Incriment r10 by 1
	beq r10,r7,done
	srli r8,r8,1
	br count

done:
	movia r12,Answer
	stw r9,(r12)

	.equ LEDs, 0xFF200000
	movia r25, LEDs
	stwio r9, (r25)
	
endiloop: br endiloop

.data
InputWord: .word 0x4a01fead
Answer: .word 0