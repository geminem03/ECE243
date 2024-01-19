.global _start

_start:
    addi r8,r8, 1      ; /*Put 1 in r8, it is our counter*/
    movi r9, 31      ; /*Set the limit to 30*/
    movi r12, 0      ; /*Initialize sum to 0*/

       ; /**Infinite loop (program finished)**/
condition: 
    blt r8, r9, then ; /*Condition for checking if counter is less than 30*/
    br after         ; /*If not, branch to after*/

then:
    add r12, r12, r8; /* Add and store counter to the sum*/
    addi r8, r8, 1    ; /*Increment the counter*/
    br condition      ; /*Branch back to the beginning of the loop*/
    addi r8, r8, 1;

 after:
     br after;