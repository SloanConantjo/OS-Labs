#ifndef __VGA_H__
#define __VGA_H__

#define VGA_COL 80
#define VGA_ROW 24

#define VGA_BASE 0xB8000

void update_cursor(void);
short get_cursor_position(void);
void clear_screen(void);
void scrolling(void);
void append2screen(char* str, int color);

#endif