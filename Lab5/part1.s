/*    Subroutine to display a four-bit quantity as a hex digits (from 0 to F) 
      on one of the six HEX 7-segment displays on the DE1_SoC.
*
 *    Parameters: the low-order 4 bits of register r4 contain the digit to be displayed
		  if bit 4 of r4 is a one, then the display should be blanked
 *    		  the low order 3 bits of r5 say which HEX display number 0-5 to put the digit on
 *    Returns: r2 = bit patterm that is written to HEX display
 */

.equ HEX_BASE1, 0xff200020
.equ HEX_BASE2, 0xff200030

.global _start
_start: 
	movi r9, 0x5 # max value of hex displays 
	movi r10, 0x10 # min value for clear 
	
	movi r4, 0x10 # set r4 to 0x10 to clear the hex display 
	movi r5, 0x0
	
	blt r4, r10, skip_clear
	
	mov r5, r0
	br clear_loop
	

clear_loop:
    call HEX_DISP   # Call HEX_DISP subroutine to clear the display
    addi r5, r5, 0x1  # Increment r5 to move to the next HEX display

    ble r5, r9, clear_loop # Branch back to clear_loop if r5 is less than 6

    br END          # End the program
	
skip_clear: 
	call HEX_DISP

HEX_DISP:   
	movia    r8, BIT_CODES     	   # starting address of the bit codes
	andi     r6, r4, 0x10	   	   # get bit 4 of the input into r6
	beq      r6, r0, not_blank 
	mov      r2, r0
	br       DO_DISP
		
not_blank:  
	mov 	 r12, r4 	# manipulate r4 in r12 to avoid corruption
	
	andi     r12, r12, 0x0f	   	   # r12 is only 4-bit
    add      r12, r12, r8            # add the offset to the bit codes
    ldb      r2, 0(r12)             # index into the bit codes

#Display it on the target HEX display
DO_DISP:    
	mov r13, r5 	# manipulate r5 in r13 to avoid corruption
	
	movia    r8, HEX_BASE1         # load address
	movi     r6,  4
	blt      r13,r6, FIRST_SET      # hex4 and hex 5 are on 0xff200030
	sub      r13, r13, r6            # if hex4 or hex5, we need to adjust the shift
	addi     r8, r8, 0x0010        # we also need to adjust the address
	
FIRST_SET:
	slli     r13, r13, 3             # hex*8 shift is needed
	addi     r7, r0, 0xff          # create bit mask so other values are not corrupted
	sll      r7, r7, r13 
	addi     r12, r0, -1
	xor      r7, r7, r12  
    sll      r12, r2, r13            # shift the hex code we want to write
	ldwio    r13, 0(r8)             # read current value       
	and      r13, r13, r7            # and it with the mask to clear the target hex
	or       r13, r13, r12	           # or with the hex code
	stwio    r13, 0(r8)		       # store back
	
END: ret
			
BIT_CODES:  .byte     0b00111111, 0b00000110, 0b01011011, 0b01001111
			.byte     0b01100110, 0b01101101, 0b01111101, 0b00000111
			.byte     0b01111111, 0b01100111, 0b01110111, 0b01111100
			.byte     0b00111001, 0b01011110, 0b01111001, 0b01110001

            .end
			
