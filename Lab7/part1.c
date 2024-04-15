int pixel_buffer_start; // global variable
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void plot_pixel(int x, int y, short int line_color);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen();
    draw_line(0, 0, 150, 150, 0x001F);   // this line is blue
    draw_line(150, 150, 319, 0, 0x07E0); // this line is green
    draw_line(0, 239, 319, 239, 0xF800); // this line is red
    draw_line(319, 0, 0, 239, 0xF81F);   // this line is a pink color
}

// code not shown for clear_screen() and draw_line() subroutines
void clear_screen() {
    int x, y;
    for (y = 0; y < 240; y++) {
        for (x = 0; x < 320; x++) {
            plot_pixel(x, y, 0x0000); // Black color
        }
    }
}


void draw_line(int x0, int y0, int x1, int y1, short int line_color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int dx2 = dx << 1; // dx * 2
    int dy2 = dy << 1; // dy * 2
    int error;

    // Determine the direction to increment x and y
    int ix = (dx > 0) ? 1 : -1;
    int iy = (dy > 0) ? 1 : -1;

    // Adjust for negative slopes
    if (dy < 0) dy = -dy;
    if (dx < 0) dx = -dx;

    // Initialize error term
    if (dx > dy)
        error = dy2 - dx;
    else
        error = dx2 - dy;

    // Start point
    int x = x0;
    int y = y0;

    // Plot initial pixel
    plot_pixel(x, y, line_color);

    // Plot subsequent pixels along the line
    if (dx > dy) {
        // Line is more horizontal
        for (int i = 0; i < dx; i++) {
            if (error >= 0) {
                y += iy;
                error -= dx2;
            }
            x += ix;
            error += dy2;
            plot_pixel(x, y, line_color);
        }
    } else {
        // Line is more vertical
        for (int i = 0; i < dy; i++) {
            if (error >= 0) {
                x += ix;
                error -= dy2;
            }
            y += iy;
            error += dx2;
            plot_pixel(x, y, line_color);
        }
    }
}


void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;

        one_pixel_address = (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));

        *one_pixel_address = line_color;
}

