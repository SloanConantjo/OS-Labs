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
    char* tmp_chptr;

	const char* ptr;//指向fmt中当前字符
	int len;//buf长度
	//逐个扫描格式化字符串fmt中的每个字符，直到遇到结束标志'\0'
    for (len = 0, ptr = fmt;*ptr;ptr++, len++) {
		if (*ptr == '\\') {		//遇到'\'
			ptr++;	//查看下个字符
			switch (*ptr) {
				case 'n':	//'\n'
					*(buf + len) = '\n';
					break;
				case '\\':	//'\\'
					*(buf + len) = '\\';
					break;
				default:    //直接忽略'\'
                    ptr--;
                    len--;
            }
		}
		else if (*ptr == '%') {		//遇到'%'
			ptr++;	//查看下个字符
			switch (*ptr) {
				case '%':	//'%%'
					*(buf + len) = '%';
					break;
				case 'd':	//'%d'
                    len += itoa(va_arg(argptr, int), buf + len) - 1;
                    break;
                case 's':   //'%s'
                    tmp_chptr = va_arg(argptr, char*);
                    for (int i = 0;tmp_chptr[i];i++, len++)
                        *(buf + len) = tmp_chptr[i];
                    len--;
                    break;
                case 'c':   //'%c'
                    *(buf + len) = va_arg(argptr, int);
                    break;
                default:    //忽略'%'
					ptr--;
                    len--;
			}
		}
		else *(buf + len) = *ptr;		//可直接打印的字符
	}
	*(buf + len) = '\0';		//用'\0'表示字符串末尾
}
