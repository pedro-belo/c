#include "main.h"

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