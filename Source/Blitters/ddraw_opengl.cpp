
#include "display.h"
#include "ddraw.h"
#include "recover.h"
#include <stddef.h>
#include <stdlib.h>
#include "palette.h"
#include <cstring>
#include <GL/freeglut.h>

GLubyte back_buffer[640*480*4];
GLuint rgb_buffer[640*480];

typedef struct _RGBA8 {
    size_t width;
    size_t height;
    size_t bytesPerRow;
    size_t bpp;
    unsigned char *bitmapData;
} RGBA8Image;

RGBA8Image * getBackBuffer(void)
{
    RGBA8Image * surface = new RGBA8Image;
    surface->width = 640;
    surface->height = 480;
    surface->bytesPerRow = 640;
    surface->bpp = 1;
    surface->bitmapData = (unsigned char*)back_buffer;
    return surface;
}


typedef struct tagRGBV {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peAlpha;
} RGBV;


static void dumpPalette(PALETTE *a)
{
	int i;

	Msg("  Red Green  Blue");
	for (i=0;i<256;i++)
	{
		Msg("%3d: %5d %5d %5d", i, a->ape[i].peRed, a->ape[i].peGreen, a->ape[i].peBlue);
	}		
}

void mapbuffer(GLubyte *a, uint32_t *b) {
/*
    for (int i=0; i<640*480; i++) {
        void * p = &palPalette.ape[a[i]];
        b[i] = *(uint32_t*)(p);
    }
*/
    RGBV rgbv;

    PALETTEENTRY *m_palette = display.GetPalette();

    for (int r=0; r<480; r++) {
        for (int c=0; c<640; c++) {
            /*
            if (a[(r*640)+c] > 122) {
                dumpPalette(&palPalette);
                Msg("break;");
                break;
            }
            */
            PALETTEENTRY * p = &m_palette[a[(r*640)+c]];
            rgbv.peRed = p->peRed;
            rgbv.peGreen = p->peGreen;
            rgbv.peBlue = p->peBlue;
            rgbv.peAlpha = 0;

            b[((479-r)*640)+c] = *(uint32_t*)(&rgbv);
        }
    }
    // memset(a, 0, 640*480);
}

void paintBuffer(RGBA8Image * _surface)
{
    glClear(GL_COLOR_BUFFER_BIT);


    glRasterPos2i (0, 0);
    mapbuffer(_surface->bitmapData, rgb_buffer);
    glPixelZoom( 2, 2 );
    glDrawPixels(640, 480,  GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, rgb_buffer);

    // glFlush();
}

class OpenglDirectDrawSurface : public IDirectDrawSurface {
    RGBA8Image * surface;
    DDSURFACEDESC surfacedesc;
public:
    OpenglDirectDrawSurface()
    {
        surface = getBackBuffer();

        surfacedesc.dwWidth = 640;
        surfacedesc.dwWidth = 480;
        surfacedesc.lPitch = 640;
        surfacedesc.lpSurface = surface->bitmapData;
    };
    ~OpenglDirectDrawSurface()
    {
        // TODO: free(surface);
    };
    //  https://msdn.microsoft.com/en-us/library/ee490539.aspx
    virtual HRESULT GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc)
    {
        assert(nullptr != lpDDSurfaceDesc);
        assert(sizeof(DDSURFACEDESC) == lpDDSurfaceDesc->dwSize);
        lpDDSurfaceDesc->dwWidth = 640;
        lpDDSurfaceDesc->dwHeight = 480;
        lpDDSurfaceDesc->lPitch = 640; // distance in bytes to the start of the next line
        return DD_OK;
    };
    virtual HRESULT Lock(LPRECT lpDestRect,
                         LPDDSURFACEDESC lpDDSurfaceDesc,
                         DWORD dwFlags,
                         HANDLE hEvent)
    {
        lpDDSurfaceDesc->lpSurface = surface->bitmapData; // draw on this
        return DD_OK;
    };
    virtual HRESULT Flip(IDirectDrawSurface *lpDDSurfaceTargetOverride,
                         DWORD dwFlags)
    {
        paintBuffer(surface);
        return DD_OK;
    };
    virtual HRESULT Unlock(LPRECT lpRect)
    {
        return DD_OK;
    };
};

class OpenglDirectDraw : public IDirectDraw {
    public:
        HRESULT CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,
                              LPDIRECTDRAWSURFACE FAR* lplpDDSurface,
                              IUnknown FAR* pUnkOuter)
        {
            *lplpDDSurface = new OpenglDirectDrawSurface();
            return DD_OK;
        };
};



HRESULT WINAPI DirectDrawCreate(GUID         *lpGUID,
                                IDirectDraw  **lplpDD,
                                IUnknown     *pUnkOuter)
{
    *lplpDD = new OpenglDirectDraw();
    return DD_OK;
}

