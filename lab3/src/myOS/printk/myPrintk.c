/*字符串格式化输出:同时向VGA和UART串口进行输出*/

#include "uart.h"
#include "vga.h"
#include "vsprintf.h"
#include <stdarg.h>


char kBuf[400];
int myPrintk(int color, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int count = vsprintf(kBuf, format, args);
    va_end(args);
    append2screen(kBuf, color); //VGA输出
    uart_put_chars(kBuf);   //UART输出
    return count;
}

char uBuf[400];
int myPrintf(int color, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int count = vsprintf(uBuf, format, args);
    va_end(args);
    append2screen(uBuf, color); //VGA输出
    uart_put_chars(uBuf);   //UART输出
    return count;
}