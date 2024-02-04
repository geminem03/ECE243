/*  tested feb 4 on de1-soc board*/

.text
/* NIOS II assembly language program for displaying a binary counter on 10 LEDs.
   The counter increments every 0.25 seconds. It restarts from 0 when reaching 255.
   The counter starts/stops when any pushbutton KEY is pressed. */

.equ LED_BASE, 0xFF200000   # Base address for LEDs
.equ KEY_BASE, 0xFF200050   

.global _start
_start:
    movi r14, 0        # Start/stop state: 0 for stop, 1 for start
    movi r17, 0        # Counter initialization
    movi r18, 255     # Counter maximum value (restarts from 0)

    # Initial check and branching based on start/stop state
    call check_edge
    bne r14, r0, start_counter
    br stop

stop:
    # Check for start/stop state and branch accordingly
    call check_edge
    bne r14, r0, start_counter
    br stop

start_counter:
    # Check for start/stop state and branch accordingly
    call check_edge
    beq r14, r0, stop

    movi r20, 80
    loop_delay:
        call delay  # Call delay function for ones
        addi r20, r20, -1  # Decrement the counter
        bne r20, r0, loop_delay  # Branch to loop_delay_ones if the counter is not zero

    addi r17, r17, 1
    call update

    # Check if counter reached maximum value, reset if true
    blt r17, r18, start_counter
    movi r17, 0

    movi r20, 80
    loop_delay1:
        call delay  # Call delay function for ones
        addi r20, r20, -1  # Decrement the counter
        bne r20, r0, loop_delay1  # Branch to loop_delay_ones if the counter is not zero

    call update

    br start_counter

/*
    delay = (num iterations / clock frequency) = (100 000 / 50M) = 0.002 sec
    delay calls = (total delay / delay per call) = (0.25 / 0.002) = ~150 calls (r20)
*/

delay:
    # Delay function for approximately 0.25 seconds
    movia r8, 100000
    loop:
        subi r8, r8, 1
        bne r8, r0, loop
    ret

check_edge:
    # Check if any pushbutton is pressed
    movia r8, KEY_BASE   # Store the address of KEY_BASE in r8
    ldwio r9, 12(r8)     # Load KEY_BASE edge into r9

    # If no button is pressed, return
    beq r9, r0, return

    # Toggle the start/stop state
    nor r14, r14, r0

    # Check which button is pressed and update LED accordingly
    andi r8, r9, 0xF      # Check all four bits representing keys
    bne r8, r0, key_pressed

key_pressed:
    # Update LEDs based on the pressed key
    movia r8, KEY_BASE   # Store the address of KEY_BASE in r8
    stwio r9, 12(r8)     # Update LEDs with the value of KEY_BASE
    ret

update:
    # Update LEDs with the counter value
    movia r19, LED_BASE    # Set the address of LED_BASE to r19
    stwio r17, (r19)       # Store the counter value in LED_BASE
    ret

return: 
    ret