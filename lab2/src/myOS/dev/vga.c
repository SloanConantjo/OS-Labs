/*
 * 本文件实现vga的相关功能，清屏和屏幕输出
 * 获取或回写光标：与端口交互
 * 往VGA写：往内存指定地址写；不支持控制字符；屏幕位置与内存地址严格对应
 */
extern void outb (unsigned short int port_to, unsigned char value);
extern unsigned char inb(unsigned short int port_from);
//VGA字符界面规格：25行80列
//VGA显存初始地址为0xB8000

short cur_line=0;
short cur_column=0;//当前光标位置
char* vga_init_p = (char*)0xB8000;


//通过当前行值cur_cline与列值cur_column回写光标
void update_cursor(void) {
	//计算偏移值，并得到高低位字节
	short offset = 80 * cur_line + cur_column;
	unsigned char lowByte = offset & 0x00FF;
	unsigned char highByte = offset >> 8;

	//与端口交互，分别发送偏移值的高低位字节
	outb(0x3D4, 0x0F);
	outb(0x3D5, lowByte);
	outb(0x3D4, 0x0E);
	outb(0x3D5, highByte);
}

//获得当前光标，计算出cur_line和cur_column的值
short get_cursor_position(void) {
	//与端口交互，分别获取偏移值的高低位字节
	unsigned char lowByte, highByte;
	outb(0x3D4, 0x0F);
	lowByte = inb(0x3D5);
	outb(0x3D4, 0x0E);
	highByte = inb(0x3D5);

	//由高低位字节计算偏移值，更新行列值
	short offset = lowByte + highByte * 256;
	cur_line = offset / 80;
	cur_column = offset % 80;

	return 0x00;
}

//清除整个屏幕，并将光标置于开始位置(0,0)
void clear_screen(void) {
	get_cursor_position();
	short offset = 80 * cur_line + cur_column;
	for (int i = 0;i < offset;i++) {
		//小端编址：[attr,char]中属性在高地址，字符在低地址
		*(vga_init_p + 2 * i) = 0x00;		//'\0'
		*(vga_init_p + 2 * i + 1) = 0x07;	//黑底白字
	}

	//重置光标位置
	cur_column = cur_line = 0;
	update_cursor();
}

//往上滚屏一行
void scrolling(void) {
	//显示字符的平移：数据[attr,char]在内存中的迁移
	for (int i = 0;i <= 23;i++) {
		for (int j = 0;j < 80;j++) {
			short offset = 80 * i + j;
			*(vga_init_p + 2 * offset) = *(vga_init_p + 2 * (offset + 80));
			*(vga_init_p + 2 * offset + 1) = *(vga_init_p + 2 * (offset + 80) + 1);
		}
	}
	//25行(最后一行)清空
	for (short offset = 80 * 24;offset < 80 * 25;offset++) {
		*(vga_init_p + 2 * offset) = 0x00;		//'0x0'
		*(vga_init_p + 2 * offset + 1) = 0x07;
	}
}

//按指定属性输出字符串str到VGA屏幕
void append2screen(char* str, int color) {
	char attr = color & 0x000000FF;		//获取一字节属性参数

	//从当前光标位置逐个往后填写字符，直到字符串末尾('\0x0')
	get_cursor_position();
	short offset = 80 * cur_line + cur_column;
	for (int i = 0;str[i] != '\0';i++) {
		if (str[i] == '\n')	//控制字符:直接更新光标偏移值到指定位置，无需写入
			offset = (offset / 80 + 1) * 80;
		else {	//可显示字符
			*(vga_init_p + 2 * offset) = *(str + i);
			*(vga_init_p + 2 * offset + 1) = attr;
			offset++;
		}
		//下一个输出的位置超出屏幕
		if (offset >= 80 * 25) {
			scrolling();		//向上滚屏一行
			offset = 80 * 24;	//更新偏移值指向最后一行开头
		}
	}

	//更新光标位置
	cur_line = offset / 80;
	cur_column = offset % 80;
	update_cursor();
}