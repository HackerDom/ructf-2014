#include "io.h"

#include <stdio.h>
#include <stdarg.h>

void Write(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    fflush(stdout);
    va_end(args);
}

void WriteLn(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    printf("\r\n");
    fflush(stdout);
    va_end(args);
}
