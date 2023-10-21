/* 
 * 与 UART 相关的输出(直接使用端口进行I/O交互)
 * 调用inb和outb函数，实现下面的uart的三个函数
 */

extern unsigned char inb(unsigned short int port_from);
extern void outb(unsigned short int port_to, unsigned char value);

#define uart_base 0x3F8		//UART端口号

//往UART端口发送一个字节(字符)
void uart_put_char(unsigned char c) {
	outb(uart_base, c);
}

//从UART端口读出一个字节(字符)
unsigned char uart_get_char(void) {
	return inb(uart_base);
}

//往UART端口发送若干字节(字符)
void uart_put_chars(char *str){ 
	while (*str != '\0') {
		uart_put_char(*str);
		str++;
	}
}