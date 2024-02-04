/* tested feb4 de1-soc (think it's counting twice as fast?) */ 

.text
/* NIOS II assembly language program for displaying a binary counter on 10 LEDs.
   The counter increments every 0.25 seconds. It restarts from 0 when reaching 255.
   The counter starts/stops when any pushbutton KEY is pressed. */


.equ LED_BASE, 0xFF200000
.equ KEY_BASE, 0xFF200050
.equ TIMER0_BASE, 0xFF202000
.equ TIMER0_STATUS, 0
.equ TIMER0_CONTROL, 4
.equ TIMER0_PERIODL, 8
.equ TIMER0_PERIODH, 12
.equ TICKSPERSEC, 25000000 # 50 MHz / 2


.global _start
_start:
	movi r20, 0 # start_counter/stop state: 0 for stop, 1 for start_counter
	movi r17, 0 # Counter initialization
	movi r18, 255 # Counter maximum value (restart_counters from 0)
	
    # Initial check and branching based on start_counter/stop state
	call check_edge
	bne r20, r0, start_counter
	br stop
		
stop:
    # Check for start_counter/stop state and branch accordingly
	call check_edge
	bne r20, r0, start_counter
	br stop
	
start_counter:
    # Check for start_counter/stop state and branch accordingly
	call check_edge
	beq r20, r0, stop

	call delay250ms

    # incremenet counter and update_leds to LEDs
	addi r17, r17, 1 
	call update_leds

    # Check if counter reached maximum value, reset if true
	blt r17, r18, start_counter
	movi r17, 0

    call delay250ms # add extra delay for reset so it doesn't jump too fast

	br start_counter
	
delay250ms:
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

    quarter_sec_loop:
        ldwio r14, TIMER0_STATUS(r12)  # Check if the TO bit of the status register is 1
        andi r14, r14, 0x1
        beq r14, r0, quarter_sec_loop
        stwio r14, TIMER0_STATUS(r12)  # Clear the TO bit

    # Stop the counter before exiting
    addi r13, r0, 0x0
    stwio r13, TIMER0_CONTROL(r12)

    ret

check_edge:
    # Check if any pushbutton is pressed
    movia r8, KEY_BASE   # Store the address of KEY_BASE in r8
    ldwio r9, 12(r8)     # Load KEY_BASE edge into r9

    # If no button is pressed, return
    beq r9, r0, return

    # Toggle the start_counter/stop state
    nor r20, r20, r0

    # Check which button is pressed and update_leds LED accordingly
    andi r8, r9, 0xF      # Check all four bits representing keys
    bne r8, r0, key_pressed

key_pressed:
    # update_leds LEDs based on the pressed key
    movia r8, KEY_BASE   # Store the address of KEY_BASE in r8
    stwio r9, 12(r8)     # update_leds LEDs with the value of KEY_BASE
    ret

update_leds:
    movia r19, LED_BASE    # Set the address of IO to r81
    stwio r17, (r19)          # Store the value of r3 in IO
    ret 
	
return:
	ret