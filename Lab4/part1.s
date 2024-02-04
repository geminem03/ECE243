/*  tested feb 4 on de1-soc board*/


.text 
/* program that displays a binary number on the 10 LEDs under control of the four pushbuttons on the DE1-SoC board */

.equ LED_BASE, 0xFF200000
.equ KEY_BASE, 0xFF200050
.equ KEY0, 0
.equ KEY1, 1
.equ KEY2, 2
.equ KEY3, 3


.global _start
_start:
    movia r8, LED_BASE  # address of LED output
    movia r9, KEY_BASE  # address of KEY input

    movi r22, 16 # max value for LED display 
    movi r16, 1 # min value for LED display 


    loop: 
        ldw r10, 0(r9) # read the value of the KEY_BASE address

        andi r10, r10, 0xF # check value of current bit
        beq r10, r0, loop # if  r6 is zero then no buttons pressed so keep polling

        # Assuming r6 contains the value from the input register (e.g., KEY_BASE)

        # Check KEY0
        andi r11, r10, 1         # Set r11 to 1 if bit 0 (KEY0) is set, otherwise 0
        bne r11, r0, process_key0  # If r11 is not zero, branch to process_key0

        # Check KEY1
        andi r11, r10, 2         # Set r11 to 2 if bit 1 (KEY1) is set, otherwise 0
        bne r11, r0, process_key1  # If r11 is not zero, branch to process_key1

       
        # Check KEY2
        andi r11, r10, 4         # Set r11 to 4 if bit 2 (KEY2) is set, otherwise 0
        bne r11, r0, process_key2  # If r11 is not zero, branch to process_key2

        # Check KEY3
        andi r11, r10, 8         # Set r11 to 8 if bit 3 (KEY3) is set, otherwise 0
        bne r11, r0, process_key3  # If r11 is not zero, branch to process_key3
       

        br loop
    
process_key0:
    # Set LEDs to 1 in base 10 (0000000001 in binary)
    movi r12, 1

    stw r12, 0(r8) # store the value in r12 at LED base address (r8)
    call button_release

    br loop

process_key1: 
    # increment display counter (r12) to max of 15
    ldw r13, 0(r8) # load the current display number to r8
    addi r13, r13, 1 # increment r8 by 1 

    bge r13, r22, button_release # stop incrementing once reached 15 

    stw r13, 0(r8) # store the value in r14 at LED base address (r8)

    call button_release
    br loop
	
process_key2: 
    ldw r14, 0(r8)
    subi r14, r14, 1 # decrement display counter 

    blt r14, r16, button_release # stop decrementing if reached 1 

    stw r14, 0(r8) # store the value in r14 at LED base address (r8) 

    call button_release
    br loop

process_key3: 
    call button_release
    stw r0, 0(r8) # clear LED display by setting to zero 

    br loop

button_release:
    # Wait until the button is released
    ldw r15, 0(r9)
    bne r15, r0, button_release

    ret
