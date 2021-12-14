
#ifndef _rect_h
#define _rect_h
typedef struct {
	WORD    left;
	WORD    top;
	WORD    right;
	WORD    bottom;
} SRECT;

typedef struct {
    WORD    left;
    WORD    top;
    WORD    right;
    WORD    bottom;
} RECT, *LPRECT;

extern void SetRect(RECT * rect, int xLeft, int yTop, int xRight, int yBottom);

#define sPtInRect(r, p) ((p).x>(r)->left && (p).x<(r)->right && (p).y>(r)->top && (p).y<(r)->bottom)
//#define PtInRect(r, p) ((p).x>(r)->left && (p).x<(r)->right && (p).y>(r)->top && (p).y<(r)->bottom)
#endif
