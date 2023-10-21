/*与 UART 相关的输入输出*/
#include "io.h"
#define uart_base 0x3F8

//从UART端口读出一个字节(字符)
unsigned char uart_get_char(void)
{
	while (!(inb(uart_base+5)&1));
	return inb(uart_base);
}

//往UART端口发送一个字节(字符)
void uart_put_char(unsigned char c)
{
    if (c == '\n') {
        outb(uart_base, '\r');
        outb(uart_base, '\n');
        return;
    }
    outb(uart_base, c);
}

//往UART端口发送若干字节(字符)
void uart_put_chars(char* str)
{
	char *p;
    for (p = str; *p; ++p) {
        uart_put_char(*p);
    }
}
