#include <stdbool.h>
#include <stdlib.h>  // for rand()

#define NumBox 15
int pixel_buffer_start;       // global variable
short int Buffer1[240][512];  // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BOX_SIZE 5

void copy_array(int source[], int destination[], int size);
void clear_screen();
void draw_boxes(int x, int y, int color);
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void draw_boxes_and_lines(int box_x[], int box_y[], int color);
void update_box_locations(int box_x[], int box_y[], int direction_x[],
                          int direction_y[], int box_x_next[],
                          int box_y_next[]);
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);

int main(void) {
  // Function prototypes
  int direction_x[NumBox], direction_y[NumBox];
  int box_x[NumBox], box_y[NumBox];
  int box_x_next[NumBox], box_y_next[NumBox];
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

  // Set initial positions and directions of boxes
  for (int i = 0; i < NumBox; ++i) {
    box_x[i] = rand() % (SCREEN_WIDTH -
                         BOX_SIZE);  // Random x-coordinate within the screen
    box_y[i] = rand() % (SCREEN_HEIGHT -
                         BOX_SIZE);  // Random y-coordinate within the screen
    direction_x[i] = (rand() % 2 == 0)
                         ? 1
                         : -1;  // Random initial x-direction (left or right)
    direction_y[i] =
        (rand() % 2 == 0) ? 1 : -1;  // Random initial y-direction (up or down)
  }

  // declare other variables(not shown)
  // initialize location and direction of rectangles(not shown)
  /* set front pixel buffer to Buffer 1 */
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

  // Draw initial image
  draw_boxes_and_lines(box_x, box_y, 0xFFFF);
  wait_for_vsync();
  // Swap front and back buffers on VGA vertical sync
  pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // New back buffer

  while (1) {
    // Update the locations of boxes
    update_box_locations(box_x, box_y, direction_x, direction_y, box_x_next,
                         box_y_next);
    draw_boxes_and_lines(box_x_next, box_y_next, 0xFFFF);

    wait_for_vsync();
    // Swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // New back buffer

    draw_boxes_and_lines(box_x, box_y, 0x0000);
    // copy new cords to drawing array
    copy_array(box_x_next, box_x, NumBox);
    copy_array(box_y_next, box_y, NumBox);
  }

  return 0;
}

void copy_array(int source[], int destination[], int size) {
  for (int i = 0; i < size; ++i) {
    destination[i] = source[i];
  }
}

void clear_screen() {
  int x, y;
  for (y = 0; y < SCREEN_HEIGHT; y++) {
    for (x = 0; x < SCREEN_WIDTH; x++) {
      plot_pixel(x, y, 0x0000);  // Black color
    }
  }
}

void draw_line(int x0, int y0, int x1, int y1, short int line_color) {
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
      plot_pixel(y, x, line_color);
    } else {
      plot_pixel(x, y, line_color);
    }

    error = error + dy;
    if (error > 0) {
      y += ystep;
      error = error - dx;
    }
  }
}

void draw_box(int x, int y, int color) {
  // Draw boxes of width _ centered around the current box position
  int width = BOX_SIZE;
  for (int column = x - width / 2; column <= x + width / 2; column++) {
    for (int row = y - width / 2; row <= y + width / 2; row++) {
      plot_pixel(column, row, color);
    }
  }
}

void draw_boxes_and_lines(int box_x[], int box_y[], int color) {
  for (int i = 0; i < NumBox; ++i) {
    // plot i-th box
    draw_box(box_x[i], box_y[i], color);  // Draw boxes

    if (i < NumBox - 1) {
      // Draw lines connecting boxes
      draw_line(box_x[i], box_y[i], box_x[i + 1], box_y[i + 1], color);
    }
  }
}

void update_box_locations(int box_x[], int box_y[], int direction_x[],
                          int direction_y[], int box_x_next[],
                          int box_y_next[]) {
  for (int i = 0; i < NumBox; ++i) {
    // Check if box hits an edge, filp the direction in that axis
    if (box_x[i] < 0 + BOX_SIZE / 2 ||
        box_x[i] > SCREEN_WIDTH - BOX_SIZE / 2 - 1)
      direction_x[i] = -direction_x[i];
    if (box_y[i] < 0 + BOX_SIZE / 2 ||
        box_y[i] > SCREEN_HEIGHT - BOX_SIZE / 2 - 1)
      direction_y[i] = -direction_y[i];

    box_x_next[i] = box_x[i] + direction_x[i];
    box_y_next[i] = box_y[i] + direction_y[i];
  }
}

void wait_for_vsync() {
  volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
  *(pixel_ctrl_ptr) = 0x1;  // Start synchronization
  while (*(volatile int *)(pixel_ctrl_ptr + 3) & 0x01) {
  };  // Wait for the synchronization to finish
}

void plot_pixel(int x, int y, short int line_color) {
  short int *one_pixel_address;
  one_pixel_address = (short int *)(pixel_buffer_start + (y << 10) + (x << 1));
  *one_pixel_address = line_color;
}