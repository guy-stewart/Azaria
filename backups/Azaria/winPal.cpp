
#include "includes.h"
#include "math.h"

#include "display.h"

extern PALETTE					palPalette;

#ifdef _MSC_VER // Windows
extern HWND                    hWndMain;

void oneRealizePalette(PALETTEENTRY *src)
{
	static HPALETTE hPal;
	int i;

struct { // lgpl 
    WORD         palVersion; 
    WORD         palNumEntries; 
    PALETTEENTRY palPalEntry[256]; 
} lPal; 

	lPal.palVersion = 0x300;
	lPal.palNumEntries = 256;
	memcpy(lPal.palPalEntry, src, sizeof(PALETTEENTRY) *256);

	hPal = CreatePalette((LOGPALETTE*)&lPal);

	if (hPal)
	{
		HDC hdc = GetDC(hWndMain);
		if (hdc)
		{
			SelectPalette(hdc, hPal, TRUE);
			i = RealizePalette(hdc);
			ReleaseDC(hWndMain, hdc);
			InvalidateRect(NULL, NULL, TRUE);
		}
		//###		DeleteObject(hPal);
	}
}


int oneFindClosestRGB(PALETTEENTRY *tgt, PALETTEENTRY *src)
{
	int i;
	double dist;
	double min = 9999999999.9;
	int color = 0;
	long r,g,b;

	// Look for an exact match ...
	for (i=0; i<256; i++)
	{
		if ((tgt[i].peRed == src->peRed)
		&&  (tgt[i].peGreen == src->peGreen)
		&&  (tgt[i].peBlue == src->peBlue))
		{
			return i;
		}

	}

	// look for an approx match ...
	for (i=0; i<256; i++)
	{
		r = (tgt[i].peRed - src->peRed);
		g = (tgt[i].peGreen - src->peGreen);
		b = (tgt[i].peBlue - src->peBlue);
		dist = (double)(r*r + g*g + b*b);
		dist = sqrt(dist);
		if (fabs(dist) < min)
		{
			min = fabs(dist);
			color = i;
		}
	}
	return color;
}

void oneCreatePalMapper(int *map, PALETTEENTRY *tgt, PALETTEENTRY *src)
{
	int i;
	int changed = 0;
	for (i=0;i<256;i++)
	{
		map[i] = oneFindClosestRGB(tgt, &src[i]);
		if (map[i] != i)
			changed++;
	}
}

void twoCreatePalMapper(int *map, PALETTEENTRY *tgt, PALETTEENTRY *src)
{
	int i;
	HPALETTE hPal;

struct { // lgpl 
    WORD         palVersion; 
    WORD         palNumEntries; 
    PALETTEENTRY palPalEntry[256]; 
} lPal; 

	lPal.palVersion = 0x300;
	lPal.palNumEntries = 256;
	memcpy(lPal.palPalEntry, src, sizeof(PALETTEENTRY) *256);
	
	hPal = CreatePalette((LOGPALETTE*)&lPal);
	if (hPal)
	{
		for (i=0;i<256;i++)
			map[i] = GetNearestPaletteIndex(hPal, 
						RGB(src->peRed, src->peGreen, src->peBlue));
		DeleteObject(hPal);
	}
}

void OFP(ONE *one)
{
	HDC hdc;
	HRESULT ddrval;
	PALETTEENTRY ddEntries[256];
	PALETTEENTRY dcEntries[256];
	int amap[256];
	int bmap[256];
	int i;
	unsigned char *bits;
	LPDIRECTDRAWPALETTE     lpDDPalette = display.GetPalette();

	if (hdc = GetDC(NULL))
	{
		i = GetSystemPaletteEntries(hdc, 0, 256, dcEntries);
		ddrval = lpDDPalette->GetEntries(0, 0, 256, ddEntries);

		i = memcmp(palPalette.ape, ddEntries, sizeof(ddEntries));

		twoCreatePalMapper(bmap, dcEntries, palPalette.ape );
		oneCreatePalMapper(amap, dcEntries, palPalette.ape );

		i = memcmp(amap, bmap, sizeof(bmap));
		ReleaseDC(NULL, hdc);
	}

	bits = one->getBits();
	for (i=0;i<(int)one->getSize();i++)
		bits[i] = amap[bits[i]];

	i = memcmp(ddEntries, dcEntries, 256*sizeof(PALETTEENTRY));

	memcpy(palPalette.ape, ddEntries, sizeof(ddEntries));
}

void TFP(ONE *one)
{
	HDC hdc;
	PALETTEENTRY dcEntries[256];
	int i, y;
	unsigned char *bits;

	if (hdc = GetDC(NULL))
	{
		GetSystemPaletteEntries(hdc, 0, 256, dcEntries);
		ReleaseDC(NULL, hdc);
	}

	bits = one->getBits();
	for (i=0;i<256*10;i++)
		for (y=0;y<one->GetHeight();y++)
			bits[i+y*one->GetPitch()] = i/10;

}

oneFixPal(ONE *one)
{
	oneRealizePalette(palPalette.ape);

	OFP(one);

	return 0;
}
#endif

