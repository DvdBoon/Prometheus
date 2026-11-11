/* Stand-in for the faststring.h the original gcc 2.95 build used.
   The driver links with -nostdlib, so strcpy must be inline here.
   Like faststring's strcpy, it returns the END of the copied string
   (the NUL), which the PDEBUG code chains on. */

#ifndef FASTSTRING_H
#define FASTSTRING_H

static inline char *fs_strcpy(char *d, const char *s)
{
  while ((*d = *s++) != 0) d++;
  return d;
}

#define strcpy(d, s) fs_strcpy((char *)(d), (const char *)(s))

#endif
