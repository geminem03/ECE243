.equ HEX_BASE1, 0xff200020

.section .exceptions, "ax"
	addi sp, sp, -36
	stw r2, 0(sp)
	stw r4, 4(sp)
	stw r5, 8(sp)
	stw r6, 12(sp)
	stw r10, 16(sp)
	stw r12, 20(sp)
	stw r13, 24(sp)
	stw r3, 28(sp)
	stw ra, 32(sp)
	stw r7, 36(sp)
	
	# CHECK interrupt cause through ctl4
	# give priority to button
	# if both the button and the timer IRQs are on
	# we will handle the button firts, return 
	# then the timer IRQ will still be on causing another
	# call to the handler
	rdctl r5, ctl4
	andi r5, r5, 2
	bne r5, r0, was_button

was_button:
# ACK BUTTON CLEAR EDGE BIT 1
	movia r2, 0xff200050 # load address of the  buttons register into r2
	ldwio r4, 12(r2)  #loading ECR edge cap register

    andi r5, r4, 0b0001
    bne r5, r0, button_0

    andi r5, r4, 0b0010
    bne r5, r0, button_1

    andi r5, r4, 0b0100
    bne r5, r0, button_2

    andi r5, r4, 0b1000
    bne r5, r0, button_3

clear_hex:
 	movi r4, 0x10 # parameter to clear hex 
   
    call HEX_DISP
	br buttonack
	
	
button_0:
	# Load the address of the global variable into a register
    
	movia r3, RUN0
	ldw r6, 0(r3)

    xori r6, r6, 0x1
	andi r6, r6, 0x1

	# set 0 to hex 0
    movi r4, 0
    movi r5, 0

    stw r6, 0(r3)
	
	beq r6, r0, clear_hex

    call HEX_DISP
	br buttonack

button_1:
	movia r3, RUN1
	ldw r6, 0(r3)

    xori r6, r6, 0x1
	andi r6, r6, 0x1

    stw r6, 0(r3)
	
	movi r4, 1
    movi r5, 1
	
	beq r6, r0, clear_hex

	call HEX_DISP
	br buttonack

button_2:

	movia r3, RUN2
	ldw r6, 0(r3)

    xori r6, r6, 0x1
	andi r6, r6, 0x1

    stw r6, 0(r3)
	
	movi r4, 2
    movi r5, 2
	
	beq r6, r0, clear_hex

	call HEX_DISP
	br buttonack
	

button_3:
	movia r3, RUN3
	ldw r6, 0(r3)

    xori r6, r6, 0x1
	andi r6, r6, 0x1

    stw r6, 0(r3)
	
	movi r4, 3
    movi r5, 3
	
	beq r6, r0, clear_hex

	call HEX_DISP
	br buttonack

buttonack:

	call buttonsEdgeReset

ihepi:
	ldw r7, 36(sp)
	ldw ra, 32(sp)
	ldw r3, 28(sp)
	ldw r13, 24(sp)
	ldw r12, 20(sp)
	ldw r10, 16(sp)
	ldw r6, 12(sp)
	ldw r5, 8(sp)
	ldw r4, 4(sp)
	ldw r2, 0(sp)
	
	addi sp, sp, 36

	
	addi ea, ea, -4
	eret


.text
.global _start
_start:
	movia sp, 0x200000
	call buttonsEdgeReset
	call CONFIG_KEYS

LOOP:
    ldw     r10, 0(r9)          # global variable
    stw   r10, 0(r8)          # write to the LEDR lights
    br      LOOP

buttonsEdgeReset: # reset all edge bits
	movia r2, 0xff200050 #buttons
    movi r4, 0xf
	stwio r4, 12(r2)
	ret

CONFIG_KEYS:
	movia r2, 0xff200050 #buttons
	
	movi r4, 0xf   # allows buttons to interrupt
    stwio r4, 8(r2)
	
    #CTL3
    movi r5, 0x3   # button are connected 2nd bit and timer to 1st bit
    wrctl ctl3, r5 # enable ints for IRQ1/buttons
	  
    #CTL0
    movi r4, 0x1
    wrctl ctl0, r4 # enable ints globally (bit 0)

    ret

/*****************************
	part 1 hex code
******************************/

HEX_DISP:   
	movia    r10, BIT_CODES     	   # starting address of the bit codes
	andi     r6, r4, 0x10	   	   # get bit 4 of the input into r6
	beq      r6, r0, not_blank 
	mov      r2, r0
	br       DO_DISP
		
not_blank:  
	mov 	 r12, r4 	# manipulate r4 in r12 to avoid corruption
	
	andi     r12, r12, 0x0f	   	   # r12 is only 4-bit
    add      r12, r12, r10            # add the offset to the bit codes
    ldb      r2, 0(r12)             # index into the bit codes

#Display it on the target HEX display
DO_DISP:    
	mov r13, r5 	# manipulate r5 in r13 to avoid corruption
	
	movia    r10, HEX_BASE1         # load address
	movi     r6,  4
	blt      r13,r6, FIRST_SET      # hex4 and hex 5 are on 0xff200030
	sub      r13, r13, r6            # if hex4 or hex5, we need to adjust the shift
	addi     r10, r10, 0x0010        # we also need to adjust the address
	
FIRST_SET:
	slli     r13, r13, 3             # hex*8 shift is needed
	addi     r7, r0, 0xff          # create bit mask so other values are not corrupted
	sll      r7, r7, r13 
	addi     r12, r0, -1
	xor      r7, r7, r12  
    sll      r12, r2, r13            # shift the hex code we want to write
	ldwio    r13, 0(r10)             # read current value       
	and      r13, r13, r7            # and it with the mask to clear the target hex
	or       r13, r13, r12	           # or with the hex code
	stwio    r13, 0(r10)		       # store back
	
END: ret
			
BIT_CODES:  .byte     0b00111111, 0b00000110, 0b01011011, 0b01001111
			.byte     0b01100110, 0b01101101, 0b01111101, 0b00000111
			.byte     0b01111111, 0b01100111, 0b01110111, 0b01111100
			.byte     0b00111001, 0b01011110, 0b01111001, 0b01110001


.global  RUN                    # used by pushbutton KEYs
RUN0:    .word    0x0            # initial value to increment COUNT
RUN1:    .word    0x0            # initial value to increment COUNT
RUN2:    .word    0x0            # initial value to increment COUNT
RUN3:    .word    0x0            # initial value to increment COUNT