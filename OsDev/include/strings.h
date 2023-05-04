#ifndef _STRINGS_H
#define _STRINGS_H
#include "stdlib.h"
#include "memory.h"
#include "printf.h"
#include "utils.h"
#include <stddef.h>

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t count);
int strcmp(const char *cs, const char *ct);
char *strchr(const char *s, int c);
char *strchrnul(const char *s, int c);
size_t strlen(const char *s);
void *memset(void *s, int c, size_t count);
#endif
