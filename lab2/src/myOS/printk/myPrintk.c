/*
 * 实现myPrint功能 先学习C语言的可变参数内容
 * 需要调用到格式化输出的function（vsprintf）
 */ 

#include <stdarg.h>

extern void append2screen(char *str,int color);

extern int vsprintf(char *buf, const char *fmt, va_list argptr);

//注意两个打印缓冲区均为全局变量
char kBuf[400];
int myPrintk(int color,const char *format, ...){
	int count;
	
	va_list argptr;	//char* argptr
	va_start(argptr,format);//初始化argptr指向第一个可变参数，即参数format后的第一个参数
	
	count=vsprintf(kBuf,format,argptr);	//格式化字符串
	
	append2screen(kBuf,color);	//VGA输出
	
	va_end(argptr);
	
	return count;
}

char uBuf[400];
int myPrintf(int color,const char *format, ...){
	int count;
	
	va_list argptr;
	va_start(argptr,format);//初始化argptr
	
	count=vsprintf(uBuf,format,argptr);
	
	append2screen(uBuf,color);//VGA输出
	
	va_end(argptr);
	
	return count;
}