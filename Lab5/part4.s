.section .exceptions, "ax"
    addi sp, sp, -20
    stw r2, 0(sp)
    stw r4, 4(sp)
    stw r5, 8(sp)
    stw r6, 12(sp)
    stw ra, 16(sp)

    # CHECK interrupt cause through ctl4
    # give priority to button
    # if both the button and the timer IRQs are on
    # we will handle the button firts, return 
    # then the timer IRQ will still be on causing another
    # call to the handler
    rdctl r5, ctl4
    andi r5, r5, 2
    bne r5, r0, was_button

was_timer:
    # Load the address of the global variable into a register
    movia r2, RUN
    ldw r5, 0(r2)

    # counter 
    movi r6, 0b1111111111 # full display 

    #increment LED's
    movia r2, 0xff200000
    ldwio r4, 0(r2)
    add r4, r4, r5  # This will leave only the first bit of r3 in r4

    bne r4, r6, skip_clear 
    mov r4, r0

skip_clear: 
    stwio r4, 0(r2)

timer_ack:
    #ACK Timer Interrupt
    movia r2, 0xff202000
    movi r4, 0
    stwio r4, 0(r2) #clear Status TO bit
    br ihepi

was_button:
    # Determine which button was pressed
    movia r5, 0xff200050
    ldwio r4, 12(r5) # Read the button edge register

    # Check if KEY0 was pressed
    andi r4, r4, 1
    beq r4, r0, check_key1

    # Toggle the RUN variable
    movia r5, RUN
    ldw r2, 0(r5)
    xori r2, r2, 1
    stw r2, 0(r5)
    br buttonack

check_key1:
    # Check if KEY1 was pressed
    movia r6, 0xff200050
    ldwio r4, 12(r6) # Read the button edge register
    srli r4, r4, 1
    andi r4, r4, 1
    beq r4, r0, check_key2

    # Half the period of incrementing COUNT
    movia r5, 0xff202000
	movi r4,0x8
	stwio r4,4(r5) # Stop the timer


    ldwio r4, 12(r5)  # Read the current Hperiod value
    srli r4, r4, 1    # DoHalfuble the period value
    stwio r4, 12(r5)  # Write the new Hperiod 
    
    ldwio r4, 8(r5)  # Read the current Lperiod value
    srli r4, r4, 1   # Half the period value
    beq r4,r0,setminimum
    stwio r4, 8(r5)  # Write the new Lperiod value
	
	# movia r4, 25000000
	# srli r4,r4,1
	
	# stwio r4, 8(r5)
	# srli r4,r4,16
	# stwio r4,12(r5)
	
	stwio r0, 0(r5) # clear TO
    movi r2, 0x7 # START | CONT | ITO
    stwio r2, 4(r5)
	
    br buttonack

    setminimum:
    addi r4,r4,1
    stwio r0, 12(r5)
    stwio r4, 8(r5)

    stwio r0, 0(r5) # clear TO
    movi r2, 0x7 # START | CONT | ITO
    stwio r2, 4(r5)
	
    br buttonack




check_key2:
    # Check if KEY2 was pressed
    movia r6, 0xff200050
    ldwio r4, 12(r6) # Read the button edge register

    
    srli r4, r4, 2
    andi r4, r4, 1
    beq r4, r0, buttonack 

    # Double the period of incrementing COUNT
    movia r5, 0xff202000
	movi r4,0x8
	stwio r4,4(r5) # Stop the timer

    ldwio r4,12(r5) # Load high pereiod
	andi r6,r4,0b1000000000000000
	bne r6,r0,continue
    slli r4,r4,1
    stwio r4,12(r5)
    ldwio r4,8(r5)
    slli r4,r4,1
    ldwio r4,8(r5)
	
    stwio r0, 0(r5) # clear TO
    movi r2, 0x7 # START | CONT | ITO
    stwio r2, 4(r5)
    
    br buttonack
	# movia r4, 25000000
	# slli r4,r4,1
	
	# stwio r4, 8(r5)
	# srli r4,r4,16
	# stwio r4,12(r5)
	
    
    continue:
	stwio r0, 0(r5) # clear TO
    movi r2, 0x7 # START | CONT | ITO
    stwio r2, 4(r5)

buttonack:
    call buttonsEdgeReset

ihepi:
    ldw ra, 16(sp)
    ldw r6, 12(sp)
    ldw r5, 8(sp)
    ldw r4, 4(sp)
    ldw r2, 0(sp)
    addi sp, sp, 20

    addi ea, ea, -4
    eret

.text
.global _start
_start:
    movia sp, 0x200000
    call CONFIG_TIMER
    call buttonsEdgeReset
    call CONFIG_KEYS
    call ledReset

LOOP:
    ldw     r10, 0(r9)          # global variable
    stw   r10, 0(r8)          # write to the LEDR lights
    br      LOOP

ledReset:
    movia r2, 0xff200000
    stwio r0, 0(r2)
    ret

buttonsEdgeReset: # reset all edge bits
    movia r2, 0xff200050 #buttons
    movi r4, 0xf
    stwio r4, 12(r2)
    ret

waitasec:
    movia r4, 0xff202000
    movia r5, 25000000
    movi r2, 0x8
    stwio r0, 0(r4) # clear TO
    stwio r2, 4(r4) # stop timer
    stwio r5, 8(r4) # periodlo
    srli r5, r5, 16
    stwio r5, 12(r4) # periodhi
    movi r2, 0x4
    stwio r2, 4(r4)

bwait:
    ldwio r2, 0(r4)
    andi r2, r2, 0x1 # check TO
    beq r2, r0, bwait

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

CONFIG_TIMER:
    #DEVICE SIDE
    movia r4, 0xff202000
    movia r5, 25000000
    movi r2, 0x8 # stop it
    stwio r0, 0(r4) # clear TO
    stwio r2, 4(r4) # stop timer
    stwio r5, 8(r4) # periodlo
    srli r5, r5, 16
    stwio r5, 12(r4) # periodhi

    stwio r0, 0(r4) # clear TO
    movi r2, 0x7 # START | CONT | ITO
    stwio r2, 4(r4)

    # CPU SIDE
    rdctl r4, ctl3
    ori r4, r4, 1
    wrctl ctl3, r4 # enable ints for IRQ0/timer
    wrctl ctl0, r4 # enable ints globally
    ret

.data
/* Global variables */
.global  COUNT
COUNT:  .word    0x0          # used by timer

.global  RUN                    # used by pushbutton KEYs
RUN:    .word    0x0            # initial value to increment COUNT

.end
