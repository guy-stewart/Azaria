//
//  ddraw.h
//  Azaria
//
//  Created by guy on 11/14/17.
//  Copyright © 2017 guy. All rights reserved.
//

#pragma once

#include "ptypes.h"
#include "rect.h"

#if defined __cplusplus
extern "C" {
#endif
    // IDirectDrawSurface
    //#include "directx.h"
#define DD_OK                                   0
#define E_FAIL 0x80004005L
#define DDFLIP_WAIT 0x001
    
#define DUMMYUNIONNAMEN(n)
    /*
     * Set to indicate that Lock should wait until it can obtain a valid memory
     * pointer before returning.  If this bit is set, Lock will never return
     * DDERR_WASSTILLDRAWING.
     */
#define DDLOCK_WAIT                             0x00000001L
    
    /*
     * dwHeight field is valid.
     */
#define DDSD_HEIGHT             0x00000002l
    
    /*
     * dwWidth field is valid.
     */
#define DDSD_WIDTH              0x00000004l
    typedef struct _DDCOLORKEY
    {
        DWORD       dwColorSpaceLowValue;   // low boundary of color space that is to
        // be treated as Color Key, inclusive
        DWORD       dwColorSpaceHighValue;  // high boundary of color space that is
        // to be treated as Color Key, inclusive
    } DDCOLORKEY;
    /*
     * DDPIXELFORMAT
     */
    typedef struct _DDPIXELFORMAT
    {
        DWORD   dwRGBBitCount;          // how many bits per pixel
        /*
         DWORD       dwSize;                 // size of structure
         DWORD       dwFlags;                // pixel format flags
         DWORD       dwFourCC;               // (FOURCC code)
         union
         {
         DWORD   dwRGBBitCount;          // how many bits per pixel
         DWORD   dwYUVBitCount;          // how many bits per pixel
         DWORD   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
         DWORD   dwAlphaBitDepth;        // how many bits for alpha channels
         DWORD   dwLuminanceBitCount;    // how many bits per pixel
         DWORD   dwBumpBitCount;         // how many bits per "buxel", total
         DWORD   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
         // format list and if DDPF_D3DFORMAT is set
         } DUMMYUNIONNAMEN(1);
         union
         {
         DWORD   dwRBitMask;             // mask for red bit
         DWORD   dwYBitMask;             // mask for Y bits
         DWORD   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
         DWORD   dwLuminanceBitMask;     // mask for luminance bits
         DWORD   dwBumpDuBitMask;        // mask for bump map U delta bits
         DWORD   dwOperations;           // DDPF_D3DFORMAT Operations
         } DUMMYUNIONNAMEN(2);
         union
         {
         DWORD   dwGBitMask;             // mask for green bits
         DWORD   dwUBitMask;             // mask for U bits
         DWORD   dwZBitMask;             // mask for Z bits
         DWORD   dwBumpDvBitMask;        // mask for bump map V delta bits
         struct
         {
         WORD    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
         WORD    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
         } MultiSampleCaps;
         
         } DUMMYUNIONNAMEN(3);
         union
         {
         DWORD   dwBBitMask;             // mask for blue bits
         DWORD   dwVBitMask;             // mask for V bits
         DWORD   dwStencilBitMask;       // mask for stencil bits
         DWORD   dwBumpLuminanceBitMask; // mask for luminance in bump map
         } DUMMYUNIONNAMEN(4);
         union
         {
         DWORD   dwRGBAlphaBitMask;      // mask for alpha channel
         DWORD   dwYUVAlphaBitMask;      // mask for alpha channel
         DWORD   dwLuminanceAlphaBitMask;// mask for alpha channel
         DWORD   dwRGBZBitMask;          // mask for Z channel
         DWORD   dwYUVZBitMask;          // mask for Z channel
         } DUMMYUNIONNAMEN(5);
         */
    } DDPIXELFORMAT;
    
    typedef struct _DDSCAPS {
        DWORD dwCaps;
    } DDSCAPS, *LPDDSCAPS;
    
    typedef struct _DDSURFACEDESC {
        DWORD         dwSize;
        DWORD         dwFlags;
        DWORD         dwHeight;
        DWORD         dwWidth;
        union {
            LONG  lPitch;
            DWORD dwLinearSize;
        } DUMMYUNIONNAMEN(1);
        DWORD         dwBackBufferCount;
        union {
            DWORD dwMipMapCount;
            DWORD dwZBufferBitDepth;
            DWORD dwRefreshRate;
        } DUMMYUNIONNAMEN(2);
        DWORD         dwAlphaBitDepth;
        DWORD         dwReserved;
        LPVOID        lpSurface;
        DDCOLORKEY    ddckCKDestOverlay;
        DDCOLORKEY    ddckCKDestBlt;
        DDCOLORKEY    ddckCKSrcOverlay;
        DDCOLORKEY    ddckCKSrcBlt;
        DDPIXELFORMAT ddpfPixelFormat;
        DDSCAPS       ddsCaps;
    } DDSURFACEDESC, *LPDDSURFACEDESC;
    
    class IDirectDrawSurface {
    public:
        //  https://msdn.microsoft.com/en-us/library/ee490539.aspx
        virtual HRESULT GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc
                                       )
        {return DD_OK;};
        virtual HRESULT Lock(LPRECT lpDestRect,
                             LPDDSURFACEDESC lpDDSurfaceDesc,
                             DWORD dwFlags,
                             HANDLE hEvent)
        {return DD_OK;};
        virtual HRESULT Flip(IDirectDrawSurface *lpDDSurfaceTargetOverride,
                             DWORD dwFlags)
        {return DD_OK;};
        virtual HRESULT Unlock(LPRECT lpRect)
        {return DD_OK;};
    };
    typedef IDirectDrawSurface * LPDIRECTDRAWSURFACE;
    class IDirectDrawPalette {
        //  https://msdn.microsoft.com/en-us/library/ee491664.aspx
    public:
        HRESULT SetEntries(DWORD          dwFlags,
                           DWORD          dwStartingEntry,
                           DWORD          dwCount,
                           PALETTEENTRY  *lpEntries) {return 0;};
    };
    class IDirectDraw {
    public:
        virtual HRESULT CreateSurface(
                              LPDDSURFACEDESC lpDDSurfaceDesc,
                              LPDIRECTDRAWSURFACE FAR* lplpDDSurface,
                              IUnknown FAR* pUnkOuter
                              );
    }  ;
    class IDirectDrawClipper {
        
    };
    
    HRESULT WINAPI DirectDrawCreate(GUID         *lpGUID,
                                    IDirectDraw  **lplpDD,
                                    IUnknown     *pUnkOuter);

#if defined __cplusplus
};
#endif
