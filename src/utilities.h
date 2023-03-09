#ifndef URCL_UTILITIES
#define URCL_UTILITIES

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static inline char* format_allocate_str(char* format, ...) {
  va_list vsnprintf_arg;
  va_list vsprintf_arg;

  va_start(vsnprintf_arg, format);
  va_copy(vsprintf_arg, vsnprintf_arg);

  int size = vsnprintf(NULL, 0, format, vsnprintf_arg);
  
  va_end(vsnprintf_arg);

  char* result = malloc(size);

  vsprintf(result, format, vsprintf_arg);

  return result;
}

#endif