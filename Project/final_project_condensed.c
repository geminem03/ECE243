int main() {

  // reset point when key[2] is pressed
  jmp_point: 

  start_program();

  welcome_screen();

  reset_board();


  // reset all buttons once gameplay starts
  *PB_EDGECAPTURE = 0xF;

  // main loop
  while (true) {
    // keep checking if reset button (key[2] is pressed)
     if ((*PB_EDGECAPTURE & 0x4)){
      *PB_EDGECAPTURE = 0xF;
      goto jmp_point;
    }
    if(!cue_ball.visible){
      cue_ball_potted();
      // infinte loop
      while(true){
        // keep checking if reset button (key[2] is pressed)
        if ((*PB_EDGECAPTURE & 0x4)){
          *PB_EDGECAPTURE = 0xF;
          goto jmp_point;
        }
      }
    }
    
    // check state
    state = state % 4;
    switch (state) {
      case 1:
        ///set_power();
        break;
      case 2:
        shoot();
        break;
      case 3:
        wait();
        break;
      default:
        break;
    }

    changeState();

    // use key[P] to set aim and move to power
    if (pressedKey == 1 && state == 0) {
      state = 1; // move to shoot state
    }

    // use key[S] for shoot
    if(pressedKey == 2 && state == 1){
      state = 2; // move to wait state
      //is_p1 = !is_p1; // toggle current player
    }

    draw_pixel_data(table);
    for (int i = 0; i < NUM_CIRCLES; i++) {
      update_circle(&circles[i]);  // Update circle positions
    }

    setAngleAndPower();

    update_circle(&cue_ball);  // update's cue ball separately
    draw_circles();            // Draw circles
    draw_cue(stick_colour);
    display_score();

    wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
  }

}


void start_program(){
  pixel_ctrl_ptr = (int *)0xFF203020;
  *(pixel_ctrl_ptr + 1) =(int)&Buffer1;  // first store the address in the back buffer
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
}

void reset_board(){
  is_p1 = true; // reset p1 as starter

  initialize_break(); // this will reset balls & score

  // initialize cue ball
  cue_ball.x = ((SCREEN_WIDTH + TABLE_WIDTH) / 2) - 20;  // offset 20
  cue_ball.y = SCREEN_HEIGHT / 2;                        // center of table
  cue_ball.dx = 0;  // move with speed 1 in -x direction (left screen)
  cue_ball.dy = 0;
  cue_ball.visible = true;
  distance = 15;

  state = 0;
}

void welcome_screen(){
  // clear edge capture register
  *PB_EDGECAPTURE = 0xF;

  while (!(*PB_EDGECAPTURE & 0x8)) {
    draw_pixel_data(Start);
    wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
  }
}

void read_keyboard() {
	volatile int * PS2_ptr = (int *) 0xFF200100;
	int data = *PS2_ptr;
  if((data & 0xFF)){
    prevprev = prev;
    prev = pressedKey;
    pressedKey = data & 0xFF;
  }else{
    pressedKey = 0;
  }
}

void changeState(){
  read_keyboard();
  switch(pressedKey){
    case 0xE0:
      break;
    case 0xF0:
      break;
    case 0x4D:
      if(prev == 0xF0){
        pressedKey = 1;
      }
      break;
    case 0x1B:
      if(prev == 0xF0){
        pressedKey = 2;
      }
      break;
    default:
      break;
  }
  return;
}

void setAngleAndPower(){
  if(delay < 2){
    delay = 0;  
    switch(pressedKey){
      case 0x75:
        if(prev == 0xF0){
          return;
        }
        angleDeg++;
      
      break;
      case 0x72:
        if(prev == 0xF0){
        return;
        }
        angleDeg--;
      break;
      case 0x6B:
        if(prev == 0xF0 && power>0){
          power--;
        }
      break;
      case 0x74:
        if(prev == 0xF0){
          power++;
        }
      break;
      default:
        break;
    }
  }
  else{
    delay++;
  }
  angle = (angleDeg % 366) * 3.1415 / 180.0;
  power = power % 19;
  distance = (power / 19) * 35 + CIRCLE_RADIUS;
  return;
}
/* since flip from playe happens write after shot need to display opposite 
 end game message as player current player (as displayed on hex) */
void cue_ball_potted(){
  state = -1;
  // cue ball potted by player 2
  if (is_p1) { 
    draw_pixel_data(p1_wins);
    wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
  }
  // cue ball potted by player 1
  else{
    draw_pixel_data(p2_wins);
    wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
  }
}

void draw_pixel_data(const short int *pixel_data) {
  int x = 0;
  int y = 0;

  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    short int pixel_value = pixel_data[i];

    // Draw the pixel at the current position
    plot_pixel(x, y, pixel_value);

    // Move to the next pixel position
    x++;
    if (x >= SCREEN_WIDTH) {
      x = 0;
      y++;
      if (y >= SCREEN_HEIGHT) {
        break;  // Exit the loop if we've reached the end of the screen
      }
    }
  }
}

void draw_guide_lines(){
  int length = 10;
  double x = cue_ball.x;
  double y = cue_ball.y;
  double Dx = -length*cos(angle);
  double Dy = length*sin(angle);
  
  for(int k = 0; k < 50; k++){
    if (x <= (SCREEN_WIDTH - TABLE_WIDTH) / 2 || x >= (SCREEN_WIDTH + TABLE_WIDTH) / 2) {
      Dx = -Dx;
    }
    if (y  <= (SCREEN_HEIGHT - TABLE_HEIGHT) / 2 ||   y >= (SCREEN_HEIGHT + TABLE_HEIGHT) / 2) {
      Dy = -Dy;  // Reverse direction
    }
    draw_line(x + 2*Dx/3,y + 2*Dy/3,x + Dx,y + Dy,2,0x0000);
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
  is_p1 = !is_p1;
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
      //angle = (*SWITCHES % 366) * M_PI / 180.0;  // input angle [in rad] wrt conventional math rotation
      draw_guide_lines();
    }

    draw_line(cue_ball.x + distance * cos(angle),
              cue_ball.y - distance * sin(angle),
              cue_ball.x + (distance + length) * cos(angle),
              cue_ball.y - (distance + length) * sin(angle), width, color);
    draw_line(cue_ball.x + distance * cos(angle),
              cue_ball.y - distance * sin(angle),
              cue_ball.x + (distance + 15) * cos(angle),
              cue_ball.y - (distance + 15) * sin(angle), width, 0x0000);
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
    if(i % 2 == 0){
      circles[i].player = 1; // start with only p1
    }
    else{
      circles[i].player = 2; // start with only p1
    }

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
          double impulse = 2 * dot_product / (2.2);  // hard coded

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
    if(circles[i].visible && circles[i].player == 1){
      plot_circle(circles[i].x, circles[i].y, CIRCLE_RADIUS, p1_colour);  // blue
    }
    else if(circles[i].visible && circles[i].player == 2){
      plot_circle(circles[i].x, circles[i].y, CIRCLE_RADIUS, p2_colour);  // red
    }
  }
  // plot cue ball in starting position
  if(cue_ball.visible) plot_circle(cue_ball.x, cue_ball.y, CIRCLE_RADIUS, cball_colour);
}

// checks if circle has been potted or not
int count_p1_score(){
  int p1_score_counter = 0;
  for(int i = 0; i < NUM_CIRCLES; i++){
    if(circles[i].visible && circles[i].player == 1){
      p1_score_counter++;
    }
  }
  return p1_score_counter;
}

// checks if circle has been potted or not
int count_p2_score(){
  int p2_score_counter = 0;
  for(int i = 0; i < NUM_CIRCLES; i++){
    if(circles[i].visible && circles[i].player == 2){
      p2_score_counter++;
    }
  }
  return p2_score_counter;
}

// displays score to hex properly
void display_score() {

  int p1_current_score = count_p1_score();
  int p2_current_score = count_p2_score();
      
  // Define an array to map the hexadecimal values to the corresponding segments
  const int segments[] = {
        /* 0 */ 0x3F, /* 1 */ 0x06, /* 2 */ 0x5B, /* 3 */ 0x4F,
        /* 4 */ 0x66, /* 5 */ 0x6D, /* 6 */ 0x7D, /* 7 */ 0x07,
        /* 8 */ 0x7F, /* 9 */ 0x6F, /* A */ 0x77, /* B */ 0x7C,
        /* C */ 0x39, /* D */ 0x5E, /* E */ 0x79, /* F */ 0x71
  };

  // Display the mapped hexadecimal value on hex0
  *hex0 = segments[p1_current_score];
  // Display the mapped hexadecimal value on hex0
  *hex2 = segments[p2_current_score];
  if(is_p1){
    *hex4 = segments[1]; // display pl
  }
  else{
    *hex4 = segments[2]; // display p2
  }

  // display end game message for winner if all balls potted
  if(p1_current_score == 0){
    draw_pixel_data(p1_wins);
    wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
  }
  else if(p2_current_score == 0){
    draw_pixel_data(p2_wins);
    wait_for_vsync();  // Wait for vertical synchronization
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // Set pixel buffer start to back buffer
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