//
//  ddraw.cpp
//  Azaria
//
//  Created by guy on 11/14/17.
//  Copyright © 2017 guy. All rights reserved.
//

#include <stdio.h>
#include "ddraw.h"
#include "ddraw_ios.h"


class IosDirectDrawSurface : public IDirectDrawSurface {
    RGBA8Image * surface;
    DDSURFACEDESC surfacedesc;
public:
    IosDirectDrawSurface()
    {
        surface = getBackBuffer();
        surfacedesc.dwWidth = 640;
        surfacedesc.dwWidth = 480;
        surfacedesc.lPitch = 640*4;
        surfacedesc.lpSurface = surface->bitmapData;
    };
    ~IosDirectDrawSurface()
    {
        free(surface);
    };
    //  https://msdn.microsoft.com/en-us/library/ee490539.aspx
    virtual HRESULT GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc)
    {
        printf("IosDirectDrawSurface::GetSurfaceDesc()\n");
        assert(nullptr != lpDDSurfaceDesc);
        assert(sizeof(DDSURFACEDESC) == lpDDSurfaceDesc->dwSize);
        lpDDSurfaceDesc->dwWidth = 640;
        lpDDSurfaceDesc->dwHeight = 480;
        lpDDSurfaceDesc->lPitch = 640*4; // distance in bytes to the start of the next line
        return DD_OK;
    };
    virtual HRESULT Lock(LPRECT lpDestRect,
                         LPDDSURFACEDESC lpDDSurfaceDesc,
                         DWORD dwFlags,
                         HANDLE hEvent)
    {
        printf("IosDirectDrawSurface::Lock() --> %p\n", surface);
        lpDDSurfaceDesc->lpSurface = surface->bitmapData; // draw on this
        return DD_OK;
    };
    virtual HRESULT Flip(IDirectDrawSurface *lpDDSurfaceTargetOverride,
                         DWORD dwFlags)
    {
        printf("IosDirectDrawSurface::Flip()\n");
        paintBuffer(surface);
        return DD_OK;
    };
    virtual HRESULT Unlock(LPRECT lpRect)
    {
        printf("IosDirectDrawSurface::Unlock()\n");
        return DD_OK;
    };
};

class IosDirectDraw : public IDirectDraw {
public:
    HRESULT CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,
                          LPDIRECTDRAWSURFACE FAR* lplpDDSurface,
                          IUnknown FAR* pUnkOuter);
};

HRESULT IosDirectDraw::CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,
                                     LPDIRECTDRAWSURFACE FAR* lplpDDSurface,
                                     IUnknown FAR* pUnkOuter)
{
    printf("IosDirectDraw::CreateSurface()\n");
    *lplpDDSurface = new IosDirectDrawSurface();
    return DD_OK;
}

HRESULT WINAPI DirectDrawCreate(GUID         *lpGUID,
                                IDirectDraw  **lplpDD,
                                IUnknown     *pUnkOuter)
{
    printf("IOS DirectDrawCreate()\n");
    *lplpDD = new IosDirectDraw();
    return false;
}



