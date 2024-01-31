.text
/* Program to Count the number of 1's and Zeroes in a sequence of 32-bit words,
and determines the largest of each */

.global ONES
ONES: 
    movi r2, 0      # counter for 1's
    movi r3, 0      # counter for 0's
    movi r20, 0     # counter for 32 bits
    movi r21, 31    # constant for counter 

    process_word: 
        /* same as part 1 */
        andi r8, r4, 0x1 # will output 1 if 1 in that bit
        add r2, r2, r8      # increment 1's counter

        xori r9, r4, 0x1 # will output 1 if 0 in that bit
        andi r9, r9, 0x1  # Keep only the least significant bit
        add r3, r3, r9      # increment 0's counter 

        srli r4, r4, 1 # shift right 

        addi r20, r20, 1 #increment counter form 0 to 31
        bne r20, r21, process_word # reached end of word

    ret


.global _start
_start:

    movia r5, TEST_NUM   # store address of TEST_NUM to r5
    movia r10, LargestOnes 
    movia r11, LargestZeroes

    loop:
        ldw r4, 0(r5)     # access the first element in TEST_NUMS array
        beq r4, r0, after  # done with search 

        call ONES

        ldw r12, 0(r10)  # load the current value of LargestOnes 
        ldw r13, 0(r11)  # load the current value of LargestZeros 

        addi r5, r5, 4    # move to the next word in TEST_NUMS for next loop
        br compare_ones

    compare_ones:
        bgt r2, r12, greatest_ones  # break if current num ones (r2) > LargestOnes

    compare_zeros:
        bgt r3, r13, greatest_zeros  # break if current num zeros (r3) > LargestZeros
		
        br loop

    greatest_ones: 
        mov r14, r2 # update value of LargestOnes
        br compare_zeros

    greatest_zeros:
        mov r15, r3 # update value of LargestZeros
        br loop

    after: 
        br after

.data
TEST_NUM: .word 0x00000010, 0x0
LargestOnes: .word 0
LargestZeroes: .word 0

/*
TEST_NUM:  .word 0x4a01fead, 0xF677D671,0xDC9758D5,0xEBBD45D2,0x8059519D
            .word 0x76D8F0D2, 0xB98C9BB5, 0xD7EC3A9E, 0xD9BADC01, 0x89B377CD
            .word 0  # end of list 
*/