/*
 * 识别格式化字符串的核心代码写在本文件中
 * 可以从网上移植代码
 */ 
 
#include <stdarg.h> 

//整数转为对应字符串；返回字符串长度
int itoa(int val, char* val_str) {
	int i, len;
	if (val > 0) {	//正整数
		for (i = val, len = 0;i > 0;i = i / 10, len++);
		for (i = len - 1;i >= 0;i--, val = val / 10)
			val_str[i] = val % 10 + 0x30;
		return len;
	}
	else if (val == 0) { val_str[0] = '0'; return 1; }	//0
	else {	//负整数
		val_str[0] = '-';
		return itoa(-val, val_str + 1) + 1;	
	}
}

//识别格式化字符串
int vsprintf(char* buf, const char* fmt, va_list argptr) {
	int count = 0;	//可变参数计数器

	const char* ptr;
	int i;
	//逐个扫描格式化字符串fmt中的每个字符，直到遇到结束标志'\0'
	for (i = 0, ptr = fmt;*ptr;ptr++, i++) {
		if (*ptr == '\\') {		//遇到'\'
			ptr++;	//查看下个字符
			switch (*ptr) {
				case 'n':	//'\n'
					*(buf + i) = '\n';
					break;
				case '\\':	//'\\'
					*(buf + i) = '\\';
					break;
				default:
					ptr--;
					break;
			}
		}
		else if (*ptr == '%') {		//遇到'%'
			ptr++;	//查看下个字符
			switch (*ptr) {
				case '%':	//'%%'
					*(buf + i) = '%';
					break;
				case 'd':	//'%d'
					i += itoa(va_arg(argptr, int), buf + i) - 1;
					count++;
					break;
				default:
					ptr--;
					break;
			}
		}
		else *(buf + i) = *ptr;		//可直接打印的字符
	}
	*(buf + i) = '\0';		//用'\0'表示字符串末尾
}
