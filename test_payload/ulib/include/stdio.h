#ifndef INCLUDE_STDIO_H_
#define INCLUDE_STDIO_H_

#include <stdarg.h>
#include <stdattr.h>

int ATTR_UFREEZONE_TEXT printf(const char *fmt, ...);
int ATTR_UFREEZONE_TEXT vprintf(const char *fmt, va_list va);

#endif
