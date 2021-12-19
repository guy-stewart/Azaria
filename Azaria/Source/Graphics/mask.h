
#pragma once
#include "ptypes.h"
#include "xfile.h"
#include <cstring>
#include "one.h"

class MASKS;

//----------------------------------------------------------------------------
class MASK {
public:
    void GetBoundingRect(RECT *rect);
    BOOL PtInRect(POINT p);
    void Move(int x_delta, int y_delta);
    static MASKS * Create(PIXEL *bits, int f, int h, int w, int pitch, PIXEL ck);
    static MASK * Create(PIXEL *bits, int h, int w, int pitch, PIXEL ck);
    static int Write(const char *lpszPathName, MASK *vector);
    static int Read(const char *path, const char * lpszPathName, MASK **vector);

    static int Free(MASK **vector);

   public:
	int x;
	int y;
	int length;
};

class MASKS {
public:
    MASKS(int count);
    ~MASKS();
    MASK *mask(int index) {
        if (index >= count)
            return NULL;
        return masks[index];
        };
    void setMask(int index, MASK *mask) {
        if (index >= count) return;
        masks[index] = mask;
        };
private:
    int    count;
    MASK **masks;
};
