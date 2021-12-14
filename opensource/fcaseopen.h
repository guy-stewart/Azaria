
#pragma once

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern FILE *fcaseopen(char const *path, char const *mode);
extern int caseopen(char const *path, unsigned int flags, int mode);

extern void casechdir(char const *path);

#if defined(__cplusplus)
}
#endif
