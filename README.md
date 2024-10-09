# ECE243 - Computer Organization & Assembly Language Programming

This second-year course focuses on fundamental aspects of computer architecture, covering topics such as central processing unit design, hardwired control, input-output mechanisms, interrupt handling, assembly language programming, main memory organization, cache structures, peripherals, interfacing techniques, and system design principles. The accompanying laboratory component includes hands-on experiments with logic systems and microprocessors, emphasizing design activities as a significant part of the practical work, culminating in an open-ended project.

## Lab 1: Introduction to Assembly Language Programming, CPULator, and the Monitor Program

Explore the fundamentals of Assembly Language programming, CPULator simulation, and the Monitor Program in Lab 1. Gain hands-on experience with the Nios II processor on the DE1-SoC Computer. Learn the essentials of program development, simulator utilization, and in-lab hardware interactions.

## Lab 2: Accessing Memory, Loops, Conditional Branches

Dive deeper into Assembly Language programming in Lab 2. Explore memory access, loop structures, and conditional branches. Enhance your skills in program development and gain a comprehensive understanding of real-time execution on the DE1-SoC board. Engage in hands-on interactions with the Monitor Program for a holistic learning experience.

## Lab 3: Logic Instructions, Subroutines and Memory Mapped Output

The goal of this lab is to cover the following concepts/skills: using the logic and shift instructions to access single
bits, understanding how to make a subroutine, and how transfer of control works, as well as parameter passing,
what memory-mapped output is, and an introduction to time in the form of software delay loops. This, together
with practice in basic assembly programming, accessing memory through loads and stores and those crucial de-
bugging skills that are the key pathway to becoming an engineer!

## Lab 4: Memory Mapped I/O, Polling and Timer

The goal of this lab is to explore the use of devices that provide input and output capabilities for a processor, and
to see how a processor connects to those inputs and outputs, through the Memory Mapped method. You’ll also
be introduced to a device that has a special purpose in computer systems, the Timer, which is used for precise
measurement and allocation of time within the computer

## Lab 5: Hex Displays and Interrupt-Driven Input/Output

Interrupt-driven I/O is a fundamental way that all processors synchronize with the outside world. The goal of this
lab is to understand the use of interrupts for the NIOS II processor, using assembly-language code, and to get some
practice with subroutines, modularity, and learning how to use the HEX displays on the DE1-SoC.

## Lab 6: Coding in C and Audio Input and Output
The goal of this lab is to become comfortable with using the C programming language to do “embedded system”
input/output, which you have been doing up to now using the Nios II Assembly Language. We will begin my
doing something very similar to previous labs, except in C. Then we’ll move on to a more complex input/output
device: sound input and output.

## Lab 7: Introduction to Graphics and Animation
The goal of this lab is to learn how to display images and perform animation, written in the C language for
the DE1-SoC Computer. Graphics can be displayed by connecting a VGA monitor to the video-out port on the
board. You can also use CPUlator to develop and debug graphics code - graphics that would normally appear on
a VGA display are instead rendered inside a sub-window, labeled VGA pixel buffer, within CPUlator. This
subwindow can be set to various sizes and it can also be “popped out” of the main browser window, if desired.

# Final Project - Billiard Blitz

Billiard Blitz is a dynamic two-player billiard game developed using C and deployed on the DE1-SoC FPGA platform. The game closely mimics real-life billiards, but with a player-specific twist: the objective for each player (Red or Green) is to pot all the balls of their assigned color. The game leverages several hardware and software components for a seamless interactive experience.

Hardware Features:
DE1-SoC Board: Utilized for running the game and interfacing with various inputs and outputs, managing game logic in real-time.
VGA Display: The visuals of the game are rendered on a VGA monitor, displaying the pool table, cue, balls, and player status, controlled via direct pixel manipulation as seen in draw_pixel_data(), draw_circles(), and plot_pixel() functions.
Mouse: The mouse is used to simulate real-time control over the cue ball, allowing players to drag and release the cue stick for precise shots.
Keyboard: Key inputs are handled for adjusting the angle of the pool stick and triggering different game states (e.g., moving from aiming to shooting as handled by read_keyboard() and changeState() functions).
Pushbuttons: Pushbuttons on the DE1-SoC board are mapped to handle game state resets and transitions. For example, pressing a specific button (key[2]) resets the game using the jmp_point: logic in the main loop.
Software Architecture:
Main Loop and Game States: The game is structured around a central state machine, which governs the flow of gameplay from aiming, shooting, waiting for the balls to stop moving, and updating the game state. This is handled in the while(true) loop in main(), with states transitioning based on player inputs and physics updates.

The state logic includes:

Aiming: Players adjust the pool stick's angle using the keyboard (setAngleAndPower()), with guide lines visually rendered for aiming (draw_guide_lines()).
Shooting: Once the player sets the power and angle, a shot is triggered via the shoot() function, applying velocity to the cue ball.
Waiting: The game monitors the movement of the balls, and once all movement ceases, the next player's turn is initiated (wait() function).
Physics Engine: The physics behind ball movements, collisions, and potting are simulated in the update_circle() function. It calculates velocity changes, checks for collisions between balls, and reverses direction when balls hit the table’s edges. Balls are "potted" when they enter designated areas on the table, making them invisible (circle->visible = false).

Game Reset and Scoring: At any point during the game, players can reset the game via the pushbutton logic embedded in PB_EDGECAPTURE. Scores are dynamically displayed on 7-segment displays using the display_score() function, and the game detects when all balls of a player's color are potted, showing end-game messages (draw_pixel_data(p1_wins) or p2_wins).

Visual Effects: The game ensures smooth graphical updates by leveraging double buffering with the VGA display (wait_for_vsync()), ensuring a flicker-free visual experience.

Key Technical Details:
Cue Stick Control: Players adjust the cue stick’s angle using the arrow keys, and the power is set with designated switches. The game calculates the resulting ball velocity based on angle and power inputs, as seen in setAngleAndPower().
Collision Detection and Response: The update_circle() function incorporates a simplified collision response between balls and the pool table’s walls, using a reflection-based approach when a ball hits the edge of the screen.
Buffering and Synchronization: Double buffering is used for smooth visual output, where the game switches between two pixel buffers to avoid visual tearing (wait_for_vsync() and pixel_buffer_start).
Overall, Billiard Blitz is a highly interactive game that showcases both software logic and hardware integration on the DE1-SoC platform, combining game design principles, physics simulation, and user input handling.

