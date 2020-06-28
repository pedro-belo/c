#ifndef __S_ENUM__

#define __S_ENUM__

typedef unsigned char uchar;

typedef enum{
    RED = 1,
    BLUE = 2,
    GREEN = 3
}Colors;

#include <stdlib.h>
#include <stdio.h>

uchar color(uchar bg, uchar fg);
uchar color_bg(uchar bg);
uchar color_fg(uchar fg);

#endif
