#include "vga.h"
extern char* vga_init_p;

//通过 vga 往合适的位置输出HH:MM:SS,即显示时钟
void setWallClock(int HH, int MM, int SS) {
	char ch[8];
	ch[0] = HH / 10 + 0x30, ch[1] = HH % 10 + 0x30;
	ch[2] = ':';
	ch[3] = MM / 10 + 0x30, ch[4] = MM % 10 + 0x30;
	ch[5] = ':';
	ch[6] = SS / 10 + 0x30, ch[7] = SS % 10 + 0x30;

	int offset = VGA_COL * (VGA_ROW + 1) - 8;
	for (int i = 0;i < 8;i++) {
		*(vga_init_p + 2 * offset + 2 * i) = ch[i];
		*(vga_init_p + 2 * offset + 2 * i + 1) = 0x3F;
	}
}

//根据 vga 显存中的数值，返回时钟，并存到相应的指针指向位置中
void getWallClock(int* HH, int* MM, int* SS) {
	char ch[8];
	int offset = VGA_COL * (VGA_ROW + 1) - 8;
	for (int i = 0;i < 8;i++)
		ch[i] = *(vga_init_p + 2 * offset + 2 * i);

	*HH = 10 * (ch[0] - 0x30) + (ch[1] - 0x30);
	*MM = 10 * (ch[3] - 0x30) + (ch[4] - 0x30);
	*SS = 10 * (ch[6] - 0x30) + (ch[7] - 0x30);
}