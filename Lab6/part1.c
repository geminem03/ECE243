/* Write a C-language program that turns on all ten Red LEDs when button KEY0 is pressed and
released, and turns them off when KEY1 is pressed and released */

struct PIT_t {
      volatile unsigned int      DR;
      volatile unsigned int      DIR;
      volatile unsigned int      MASK;
      volatile unsigned int      EDGE;
      };


// The LED pit is at this base address
struct PIT_t      * const ledp = ((struct PIT_t *) 0xFF200000);
// The BUTTONS pit is at this base address
struct PIT_t      *const buttonp = ((struct PIT_t *) 0xFF200050);

main()
{
	LEDButtonToggle();
}


void LEDButtonToggle(void) {
   while (1) {
       // Check if button 0 was pressed and released
      if (buttonp->EDGE & 0x1) {
         // Turn on all LEDs
         ledp->DR = 0x3FF;
         buttonp->EDGE = 0x1; // Reset edge detection for button 0
      }
      // Check if button 1 was pressed and released
      else if (buttonp->EDGE & 0x2) {
         ledp->DR = 0x0; // Turn off all LEDs
         buttonp->EDGE = 0x2; // Reset edge detection for button 1
      }
   }
}
