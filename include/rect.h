
#pragma once

#include <stdint.h>

typedef struct {
	int    left;
	int    top;
	int    right;
	int    bottom;
} SRECT;

typedef struct {
    int    left;
    int    top;
    int    right;
    int    bottom;
} RECT, *LPRECT;

extern void SetRect(RECT * rect, int xLeft, int yTop, int xRight, int yBottom);

#define sPtInRect(r, p) ((p).x>(r)->left && (p).x<(r)->right && (p).y>(r)->top && (p).y<(r)->bottom)
