
#pragma once

#include <stdint.h>

typedef struct tagSIZE {
    int32_t cx;
    int32_t cy;
} SIZE, *PSIZE;

typedef struct {
    uint16_t x;
    uint16_t y;
} POINT;
typedef POINT * LPPOINT;

