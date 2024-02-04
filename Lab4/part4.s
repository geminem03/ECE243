/* tested feb 4 on de1-soc (doubled value of TICKSPERSEC)*/

.text
/* 
In this part you are to write an assembly language program that implements a real-time binary clock. Display
the time on the 10 LEDs in the following format: SS:DD, where SS are seconds and DD are hundredths of a
second. The SS should be displayed on the high-order 3 bits of the red LEDS (i.e. LEDR9:5) and the DD should
be displayed on the low-order 7 bits 
*/


.equ LED_BASE, 0xFF200000
.equ KEY_BASE, 0xFF200050
.equ TIMER0_BASE, 0xFF202000
.equ TIMER0_STATUS, 0
.equ TIMER0_CONTROL, 4
.equ TIMER0_PERIODL, 8
.equ TIMER0_PERIODH, 12
.equ TICKSPERSEC, 1000000 # 50 MHz / 100 = 0.01 seconds

.global _start
_start:
	movi r20, 0 # start_counter/stop state: 0 for stop, 1 for start_counter

    movi r16, 0 # sec counter init 
	movi r17, 0 # mill Counter initialization

	movi r18, 99 # counter for milliseconds
    movi r15, 8 # counter for seconds
    # counter for seconds 
	
    # Initial check and branching based on start_counter/stop state
	call check_edge
	bne r20,r0, start_counter
    
	br stop

stop:
    # Check for start_counter/stop state and branch accordingly
	call check_edge
	bne r20, r0, start_counter
	br stop

start_counter:
    # Check for start/stop state and branch accordingly
	call check_edge
	beq r20,r0, stop

	call delay_millisec

	addi r17, r17, 1
	call update_milli

    # Check if counter reached maximum value, reset if true
	blt r17, r18, start_counter
	
	call delay_millisec
	
	#update seconds
	movi r17, 0
    addi r16, r16, 1
	
	call update_milli
   
    blt r16, r15, start_counter
	movi r16, 0
	
	call update_milli
	br start_counter

delay_millisec:
    movia r12, TIMER0_BASE

    # Stop the counter
    addi r13, r0, 0x0
    stwio r13, TIMER0_CONTROL(r12)

    # Clear the status register
    ldwio r14, TIMER0_STATUS(r12)
    stwio r14, TIMER0_STATUS(r12)

    # Set the period registers to 50M / 4 = 12.5M
    addi r13, r0, %lo(TICKSPERSEC) 
    stwio r13, TIMER0_PERIODL(r12)  # Load lower 16 bits
    addi r13, r0, %hi(TICKSPERSEC)
    stwio r13, TIMER0_PERIODH(r12)  # Load higher 16 bits

    # start_counter the counter
    addi r13, r0, 0x5  # 0x5 = 0101, so we write 1 to start_counter and clear CONT
    stwio r13, TIMER0_CONTROL(r12)

    millisec_loop:
        ldwio r14, TIMER0_STATUS(r12)  # Check if the TO bit of the status register is 1
        andi r14, r14, 0x1
        beq r14, r0, millisec_loop
        stwio r14, TIMER0_STATUS(r12)  # Clear the TO bit

    # Stop the counter before exiting
    addi r13, r0, 0x0
    stwio r13, TIMER0_CONTROL(r12)

    ret

check_edge:
	movia r8, KEY_BASE   # Store the address of IO in r8
    ldwio r9, 12(r8)         # Load IO edge into r9
	
	beq r9, r0, return
	nor r20, r20, r0
	
	# Check which button is pressed and update_milli LED accordingly
    andi r8, r9, 0xF      # Check all four bits representing keys
    bne r8, r0, key_pressed

key_pressed:
    # update_milli LEDs based on the pressed key
    movia r8, KEY_BASE   # Store the address of KEY_BASE in r8
    stwio r9, 12(r8)     # update LEDs with the value of KEY_BASE
    ret

update_milli:
    movia r19, LED_BASE    # Set the address of IO to r19
   	
	slli r16,r16,7
	or r22,r16,r17
	srli r16,r16,7
	stwio r22,(r19)
	
    ret 

return:
	ret
