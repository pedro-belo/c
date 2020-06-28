#include "main.h"

typedef unsigned char uchar;

typedef enum{
    RED = 1,
    BLUE = 2,
    GREEN = 3
}Colors;

uchar color(uchar bg, uchar fg);
uchar color_bg(uchar bg);
uchar color_fg(uchar fg);

int main(void)
{
    Colors bg = RED;
    Colors fg = GREEN;

    uchar c = color(GREEN, RED);
    
    fprintf(stdout,"COLOR = %u\n", c);
    fprintf(stdout,"BG = %u\n", color_bg(c));
    fprintf(stdout,"FG = %u\n", color_fg(c));

    return (0);
}

uchar color(uchar bg, uchar fg){
    return (bg << 4) | fg;
}
uchar color_bg(uchar color){
    return(color & 0xF0);
}
uchar color_fg(uchar color){
    return(color & 0xF);
}