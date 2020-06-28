#include "./simple_enum.h"

uchar color(uchar bg, uchar fg){
    return (bg << 4) | fg;
}
uchar color_bg(uchar color){
    return(color & 0xF0);
}
uchar color_fg(uchar color){
    return(color & 0xF);
}