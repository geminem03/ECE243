#include <math.h>
#include <stdbool.h>
#include <stdlib.h>  // for rand()

int pixel_buffer_start;       // global variable
short int Buffer1[240][512];  // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SQUARE_SIZE 150
#define TABLE_WIDTH 260
#define TABLE_HEIGHT 180
#define NUM_CIRCLES 6
#define CIRCLE_RADIUS 10
#define SWITCHES ((volatile long *)0xFF200040)
#define PB_EDGECAPTURE ((volatile long *)0xFF20005C)

typedef struct {
  int x;
  int y;
  int dx;
  int dy;
  bool visible;
} Circle;

Circle circles[NUM_CIRCLES];
Circle cue_ball;
double angle = 0;
double power = 0;

void draw_square();
void initialize_break();
double squareRoot(double num);
double frictionCounter = 0;
double power;
int delay;
int distance = 15;
int state = 0;
/*States
   -1: GAME OVER LOSER
    0: Setting angle;
    1: Setting power;
    2: Shoot!
    3: wait for balls to stop...
*/

void plot_circle(int x0, int y0, int radius, short int color);
void initialize_circles();
void update_circle(Circle *circle);
void draw_circles();
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_line(int x0, int y0, int x1, int y1, int width, short int line_color);
void draw_cue(int color);
void set_power();
void shoot();
void wait();
void draw_guide_lines();

int main() {
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
  *(pixel_ctrl_ptr + 1) =
      (int)&Buffer1;  // first store the address in the back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
  wait_for_vsync();
  /* initialize a pointer to the pixel buffer, used by drawing functions */
  pixel_buffer_start = *pixel_ctrl_ptr;
  clear_screen();

  // pixel_buffer_start points to the pixel buffer
  /* set back pixel buffer to Buffer 2 */
  *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
  pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // we draw on the back buffer
  clear_screen();  // pixel_buffer_start points to the pixel buffer

  initialize_break();

  // initialize cue ball
  cue_ball.x = ((SCREEN_WIDTH + TABLE_WIDTH) / 2) - 20;  // offset 20
  cue_ball.y = SCREEN_HEIGHT / 2;                        // center of table
  cue_ball.dx = 0;  // move with speed 1 in -x direction (left screen)
  cue_ball.visible = true;

  while (true) {
    if(!cue_ball.visible){
      state = -1;
      clear_screen();
      wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start =
        *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
      break;
    }
    state = state % 4;
    if (*PB_EDGECAPTURE & 0x1 && state != 3) {
      state++;
      *PB_EDGECAPTURE = 1;
    }
    if (state == 1) set_power();
    if (state == 2) shoot();
    if (state == 3){
      wait();
    }
    clear_screen();
    draw_square();  // Draw square
    for (int i = 0; i < NUM_CIRCLES; i++) {
      update_circle(&circles[i]);  // Update circle positions
    }

    update_circle(&cue_ball);  // update's cue ball seperatley
    draw_circles();            // Draw circles
    draw_cue(0xFF00);
    // if first key is pressed

    wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start =
        *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
  }
}

void draw_guide_lines(){
  int length = 10;
  int x = cue_ball.x;
  int y = cue_ball.y;
  int Dx = -length*cos(angle);
  int Dy = length*sin(angle);
  
  for(int k = 0; k < 50; k++){
    draw_line(x + length*cos(angle)/2,y - length*sin(angle)/2,x + length*cos(angle),y + length*sin(angle),2,0x0000);
    if (x <= (SCREEN_WIDTH - TABLE_WIDTH) / 2 || x >= (SCREEN_WIDTH + TABLE_WIDTH) / 2) {
      Dx = -Dx;
    }
    if (y  <= (SCREEN_HEIGHT - TABLE_HEIGHT) / 2 ||   y >= (SCREEN_HEIGHT + TABLE_HEIGHT) / 2) {
      Dy = -Dy;  // Reverse direction
    }
    x+=Dx;
    y+=Dy;
  }
}
void wait(){
  int sum;
    sum = 0;
    for(int i = 0; i < NUM_CIRCLES; i++){
      sum += circles[i].dx + circles[i].dy;
    }
    if(!sum && !cue_ball.dx && !cue_ball.dy)delay++;
    
  if (delay > 10) {
    delay = 0;
    distance = CIRCLE_RADIUS;
    state++;
  }
}

void shoot() {
  cue_ball.dx = - power * cos(angle);
  cue_ball.dy = power * sin(angle);
  state++;
}

void set_power() {
  power = *SWITCHES % 19;
  distance = (power / 19) * 35 + CIRCLE_RADIUS;
}

void draw_cue(int color) {
  if (state < 3) {
    int length = 125;
    int width = 5;
    if (state < 1) {
      angle = (*SWITCHES % 366) * M_PI / 180.0;  // input angle [in rad] wrt conventional math rotation
      draw_guide_lines();
    }

    draw_line(cue_ball.x + distance * cos(angle),
              cue_ball.y - distance * sin(angle),
              cue_ball.x + (distance + length) * cos(angle),
              cue_ball.y - (distance + length) * sin(angle), width, color);
  }
}

void draw_line(int x0, int y0, int x1, int y1, int width,
               short int line_color) {
  int temp;
  int ystep;
  bool is_steep = abs(y1 - y0) > abs(x1 - x0);

  if (is_steep) {
    temp = x0;
    x0 = y0;
    y0 = temp;

    temp = x1;
    x1 = y1;
    y1 = temp;
  }

  if (x0 > x1) {
    temp = x0;
    x0 = x1;
    x1 = temp;

    temp = y0;
    y0 = y1;
    y1 = temp;
  }

  int dx = x1 - x0;
  int dy = abs(y1 - y0);
  int error = -(dx / 2);
  int y = y0;
  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (int x = x0; x <= x1; x++) {
    if (is_steep) {
      for (int i = -width / 2; i <= width / 2; i++) {
        plot_pixel(y + i, x, line_color);
      }
    } else {
      for (int i = -width / 2; i <= width / 2; i++) {
        plot_pixel(x, y + i, line_color);
      }
    }

    error = error + dy;
    if (error > 0) {
      y += ystep;
      error = error - dx;
    }
  }
}

void draw_square() {
  for (int y = (SCREEN_HEIGHT - TABLE_HEIGHT) / 2;
       y < (SCREEN_HEIGHT + TABLE_HEIGHT) / 2; y++) {
    for (int x = (SCREEN_WIDTH - TABLE_WIDTH) / 2;
         x < (SCREEN_WIDTH + TABLE_WIDTH) / 2; x++) {
      plot_pixel(x, y, 0xFFFF);  // White color
    }
  }

  plot_circle((SCREEN_WIDTH-TABLE_WIDTH)/2+CIRCLE_RADIUS,(SCREEN_HEIGHT-TABLE_HEIGHT)/2+CIRCLE_RADIUS,CIRCLE_RADIUS,0x07E0);
  plot_circle((SCREEN_WIDTH-TABLE_WIDTH)/2+CIRCLE_RADIUS,(SCREEN_HEIGHT+TABLE_HEIGHT)/2-CIRCLE_RADIUS,CIRCLE_RADIUS,0x07E0);
  plot_circle((SCREEN_WIDTH+TABLE_WIDTH)/2-CIRCLE_RADIUS,(SCREEN_HEIGHT-TABLE_HEIGHT)/2+CIRCLE_RADIUS,CIRCLE_RADIUS,0x07E0);
  plot_circle((SCREEN_WIDTH+TABLE_WIDTH)/2-CIRCLE_RADIUS,(SCREEN_HEIGHT+TABLE_HEIGHT)/2-CIRCLE_RADIUS,CIRCLE_RADIUS,0x07E0);

}

void plot_circle(int x0, int y0, int radius, short int color) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    // Draw two horizontal lines from (x0 - x, y0 + y) to (x0 + x, y0 + y)
    for (int i = x0 - x; i <= x0 + x; i++) {
      plot_pixel(i, y0 + y, color);
      plot_pixel(i, y0 - y, color);
    }

    // Draw two horizontal lines from (x0 - y, y0 + x) to (x0 + y, y0 + x)
    for (int i = x0 - y; i <= x0 + y; i++) {
      plot_pixel(i, y0 + x, color);
      plot_pixel(i, y0 - x, color);
    }

    y += 1;

    // Calculate the next pixel position
    if (err <= 0) {
      err += 2 * y + 1;
    }
    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

void initialize_circles() {
  for (int i = 0; i < NUM_CIRCLES; i++) {
    circles[i].x = (rand() % (SQUARE_SIZE - 2 * CIRCLE_RADIUS)) +
                   (SCREEN_WIDTH - SQUARE_SIZE) / 2 + CIRCLE_RADIUS;
    circles[i].y = (rand() % (SQUARE_SIZE - 2 * CIRCLE_RADIUS)) +
                   (SCREEN_HEIGHT - SQUARE_SIZE) / 2 + CIRCLE_RADIUS;
    circles[i].dx = (rand() % 11) - 5;  // Random velocity between -1 and 1
    circles[i].dy = (rand() % 11) - 5;
  }
}

// set up ball starting position
void initialize_break() {
  int limit = 1;
  int count = 0;
  int x = (SCREEN_WIDTH - TABLE_WIDTH) / 2 + 60;  // Leftmost circle
  int y = (SCREEN_HEIGHT) / 2;

  for (int i = 0; i < NUM_CIRCLES; i++) {
    circles[i].x = x;
    circles[i].y = y;
    circles[i].dx = 0;
    circles[i].dy = 0;
    circles[i].visible = true;

    count++;
    y = y + 2 * CIRCLE_RADIUS;

    if (count == limit) {
      count = 0;
      limit++;
      x = x - 1.87 * CIRCLE_RADIUS;  // Adjust x position for the next row
      y = (SCREEN_HEIGHT) / 2 - (limit - 1) * CIRCLE_RADIUS;
    }
  }
}

double squareRoot(double num) {
  if (num < 0) {
    // Handling negative numbers (imaginary roots)
    return -1.0;
  }

  double x = num;
  double y = 1;
  double epsilon = 0.000001;  // Precision

  while (x - y > epsilon) {
    x = (x + y) / 2;
    y = num / x;
  }

  return x;
}

void update_circle(Circle *circle) {
  // Check for collision with square edges
  if (circle->x - CIRCLE_RADIUS <= (SCREEN_WIDTH - TABLE_WIDTH) / 2 ||
      circle->x + CIRCLE_RADIUS >= (SCREEN_WIDTH + TABLE_WIDTH) / 2) {
    circle->dx = -circle->dx;  // Reverse direction
  }
  if (circle->y - CIRCLE_RADIUS <= (SCREEN_HEIGHT - TABLE_HEIGHT) / 2 ||
      circle->y + CIRCLE_RADIUS >= (SCREEN_HEIGHT + TABLE_HEIGHT) / 2) {
    circle->dy = -circle->dy;  // Reverse direction
  }

  if (circle->dx || circle->dy) {
    frictionCounter++;
    if (frictionCounter == 20) {
      frictionCounter = 0;
      if (circle->dx > 0) {
        circle->dx--;
      } else if(circle->dx != 0){
        circle->dx++;
      }
      if (circle->dy > 0) {
        circle->dy--;
      } else if(circle->dy != 0){
        circle->dy++;
      }
    }
  }
  circle->y += circle->dy;
  circle->x += circle->dx;

  // Check for collision with other circles
  for (int i = 0; i < NUM_CIRCLES; i++) {
    if (&circles[i] != circle) {  // Skip self
      Circle *other_circle = &circles[i];
      double dx = circle->x - other_circle->x;
      double dy = circle->y - other_circle->y;
      double distance = squareRoot(dx * dx + dy * dy);

      if (distance < 2 * CIRCLE_RADIUS) {  // Circles overlap
        // Calculate the normalized collision vector
        double nx = dx / distance;
        double ny = dy / distance;

        // Calculate the relative velocity
        double dvx = circle->dx - other_circle->dx;
        double dvy = circle->dy - other_circle->dy;

        // Calculate the dot product of relative velocity and collision vector
        double dot_product = dvx * nx + dvy * ny;

        // If they are moving towards each other
        if (dot_product < 0) {
          // Calculate the impulse scalar
          double impulse = 2 * dot_product / (2);  // hard coded

          // Apply the impulse to both circles
          circle->dx -= impulse * nx;
          circle->dy -= impulse * ny;
          other_circle->dx += impulse * nx;
          other_circle->dy += impulse * ny;
        }
      }
    }
  }
  
  //check for pots
  bool topleft = circle->x < (2*CIRCLE_RADIUS+(SCREEN_WIDTH-TABLE_WIDTH)/2) && circle->y < (2*CIRCLE_RADIUS+(SCREEN_HEIGHT-TABLE_HEIGHT)/2);
  bool topright = circle->x > ((SCREEN_WIDTH+TABLE_WIDTH)/2 - 2*CIRCLE_RADIUS) && circle->y < (2*CIRCLE_RADIUS+(SCREEN_HEIGHT-TABLE_HEIGHT)/2);
  bool bottomleft =  circle->x < (2*CIRCLE_RADIUS+(SCREEN_WIDTH-TABLE_WIDTH)/2) && circle->y > ((SCREEN_HEIGHT+TABLE_HEIGHT)/2 - 2*CIRCLE_RADIUS);
  bool bottomright = circle->x > ((SCREEN_WIDTH+TABLE_WIDTH)/2 - 2*CIRCLE_RADIUS) && circle->y > ((SCREEN_HEIGHT + TABLE_HEIGHT)/2 - 2*CIRCLE_RADIUS);
  
  if(topleft || topright || bottomleft || bottomright){
    circle->visible = false;
  }

}

void draw_circles() {
  for (int i = 0; i < NUM_CIRCLES; i++) {
    if(circles[i].visible)
    plot_circle(circles[i].x, circles[i].y, CIRCLE_RADIUS,
                0xF800);  // Red color
  }
  // plot cue ball in starting position
  if(cue_ball.visible) plot_circle(cue_ball.x, cue_ball.y, CIRCLE_RADIUS, 0x6969);
}

void clear_screen() {
  int x, y;
  for (y = 0; y < SCREEN_HEIGHT; y++) {
    for (x = 0; x < SCREEN_WIDTH; x++) {
      plot_pixel(x, y, 0x0000);  // Black color
    }
  }
}

void plot_pixel(int x, int y, short int line_color) {
  short int *one_pixel_address;
  one_pixel_address = (short int *)(pixel_buffer_start + (y << 10) + (x << 1));
  *one_pixel_address = line_color;
}

void wait_for_vsync() {
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
  *(pixel_ctrl_ptr) = 0x1;  // Start synchronization
  while (*(volatile int *)(pixel_ctrl_ptr + 3) & 0x01) {
  };  // Wait for the synchronization to finish
}