#ifndef __VSPRINTF_H__
#define __VSPRINTF_H__

#include <stdarg.h>

int vsprintf(char* dst, const char* format, va_list args);

#endif