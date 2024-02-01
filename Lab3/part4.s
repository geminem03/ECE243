.text
/* Program to Count the number of 1's and Zeroes in a sequence of 32-bit words,
and determines the largest of each */

/* DE1-Soc has a much faster processor. Were able to observe LED's switching with 1 call to Delay
	in CPULator. For the board we had to increase this to about 500 calls to notice the delay
*/

.global _start
_start:
	movia r4, TEST_NUM  # TEST_NUM in R4

	begin:
		ldw r5, (r4)  # Element in R5
		beq r5, r0, Done  # If element is 0 => Done

		movi r8, 32  # Constant 32 in R8
		movi r9, 0   # 1-counter
		movi r10, 0  # 0-counter
		movi r11, 0  # loop index

		call count

		call updateones
		call delay  # Add delay after updating ones

		call updatezeros
		call delay  # Add delay after updating zeros

		addi r4, r4, 4  # Update TEST_NUM to next element

		br begin

count:
	processword:
		/* same as part 1 */
		andi r12, r5, 0x1  # will output 1 if 1 in that bit
		add r9, r9, r12   # increment 1's counter

		xori r12, r5, 0x1  # will output 1 if 0 in that bit
		andi r12, r12, 0x1  # Keep only the least significant bit
		add r10, r10, r12  # increment 0's counter

		srli r5, r5, 1  # shift right

		addi r11, r11, 1  # increment counter form 0 to 31
		bne r11, r8, processword  # reached end of word

	ret

updateones:
	movia r12, LargestOnes
	ldw r13, (r12)  # LargestOnes in r13

	bgt r9, r13, changeones
	ret

changeones:
	stw r9, (r12)
	ret

updatezeros:
	movia r12, LargestZeroes
	ldw r13, (r12)

	bgt r10, r13, changezeroes
	ret

changezeroes:
	stw r10, (r12)
	ret

Done:
    movi r22, 3000  # Set the number of iterations to 1000

    call showone
    loop_delay_ones:
        call delay  # Call delay function for ones
        addi r22, r22, -1  # Decrement the counter
        bne r22, r0, loop_delay_ones  # Branch to loop_delay_ones if the counter is not zero

    movi r22, 3000  # Reset the counter

    call showzero
    loop_delay_zeros:
        call delay  # Call delay function for zeros
        addi r22, r22, -1  # Decrement the counter
        bne r22, r0, loop_delay_zeros  # Branch to loop_delay_zeros if the counter is not zero

    br Done

showone:
	.equ LEDs, 0xFF200000
	movia r23, LEDs
	movia r3, LargestOnes
	ldw r4, (r3)
	stwio r4, (r23)
	ret

showzero:
	.equ LEDs, 0xFF200000
	movia r23, LEDs
	movia r3, LargestZeroes
	ldw r4, (r3)
	stwio r4, (r23)
	ret

delay:
	movi r20, 10000  # Num of iterations
	movi r21, 0  # Initialize counter
	
	again: 
		addi r21, r21, 1
		bne r21, r20, again
		movi r21, 0
	
	ret

.data
TEST_NUM:  .word 0x4a01fead, 0xF677D671,0xDC9758D5,0xEBBD45D2,0x8059519D
            .word 0x76D8F0D2, 0xB98C9BB5, 0xD7EC3A9E, 0xD9BADC01, 0x89B377CD
            .word 0  # end of list

LargestOnes: .word 0
LargestZeroes: .word 0

LED: .word 0xff200000
