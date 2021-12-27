
#pragma once

#include <stdint.h>

typedef struct tagPALETTEENTRY {
    uint8_t peRed;
    uint8_t peGreen;
    uint8_t peBlue;
    uint8_t peFlags;
} PALETTEENTRY, *LPPALETTEENTRY;

typedef struct {
    uint32_t        dwRiff;
    uint32_t           dwFileSize;
    uint32_t           dwPal;
    uint32_t           dwData;
    uint32_t           dwDataSize;
    uint16_t            palVersion;
    uint16_t            palNumEntries;
    PALETTEENTRY    ape[256];
} PALETTE;

typedef struct tagLOGPALETTE {
    uint16_t         palVersion;
    uint16_t         palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE;

typedef void * HPALETTE;
