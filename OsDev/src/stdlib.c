#include "stdlib.h"
int memcmp (const void *str1, const void *str2, int count)
{
  const unsigned char *s1 = str1;
  const unsigned char *s2 = str2;
  while (count-- > 0)
    {
      if (*s1++ != *s2++)
          return s1[-1] < s2[-1] ? -1 : 1;
    }
  return 0;
}

char* itoa(int num, int base) {
  static char intbuf[33];
  uint32_t j = 0, isneg = 0, i;

  if (num == 0) {
    intbuf[0] = '0';
    intbuf[1] = 0;
    return intbuf;
  }

  if (base == 10 && num < 0) {
    isneg = 1;
    num = -num;
  }

  i = (uint32_t)num;

  while (i != 0) {
    int q = i / base;
    int r = i % base;
    // convert number to ascii character
    intbuf[j++] = (r) < 10 ? '0' + (r) : 'a' + (r)-10;
    i = q;
  }

  if (isneg) intbuf[j++] = '-';

  if (base == 16) {
    intbuf[j++] = 'x';
    intbuf[j++] = '0';
  } else if (base == 8) {
    intbuf[j++] = '0';
  } else if (base == 2) {
    intbuf[j++] = 'b';
    intbuf[j++] = '0';
  }

  intbuf[j] = '\0';
  j--;
  i = 0;
  // reverse buffer
  while (i < j) {
    int tmp = intbuf[i];
    intbuf[i] = intbuf[j];
    intbuf[j] = tmp;
    i++;
    j--;
  }

  return intbuf;
}

int atoi(char* num) {
  int res = 0, power = 0, digit, i;
  char* start = num;

  // Find the end
  while (*num >= '0' && *num <= '9') {
    num++;
  }

  num--;

  while (num != start) {
    digit = *num - '0';
    for (i = 0; i < power; i++) {
      digit *= 10;
    }
    res += digit;
    power++;
    num--;
  }

  return res;
}

void mcpy(void* dest, const void* src, int bytes) {
  char* d = dest;
  const char* s = src;
  while (bytes--) {
    *d++ = *s++;
  }
}
/*need to check if our implementation of mmzero work*/
void mzero(void* dest, int bytes) {
  char* d = dest;
  while (bytes--) {
    *d++ = 0;
  }
}
