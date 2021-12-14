//
//  directx.h
//  Azaria
//
//  Created by guy on 11/12/17.
//  Copyright © 2017 guy. All rights reserved.
//

#ifndef directx_h
#define directx_h

#define DD_OK                                   0
#define E_FAIL 0x80004005L
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

//
// For compilers that don't support nameless unions, do a
//
// #define NONAMELESSUNION
//
// before #include <ddraw.h>
//
#ifndef DUMMYUNIONNAMEN
#if defined(__cplusplus) || !defined(NONAMELESSUNION)
#define DUMMYUNIONNAMEN(n)
#else
#define DUMMYUNIONNAMEN(n)      u##n
#endif
#endif

 typedef struct _DDSCAPS {
 DWORD dwCaps;
 } DDSCAPS, *LPDDSCAPS;


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


/*
 * DDCOLORKEY
 */
typedef struct _DDCOLORKEY
{
    DWORD       dwColorSpaceLowValue;   // low boundary of color space that is to
    // be treated as Color Key, inclusive
    DWORD       dwColorSpaceHighValue;  // high boundary of color space that is
    // to be treated as Color Key, inclusive
} DDCOLORKEY;

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
/*
 * DDSURFACEDESC2
 */
/*
typedef struct _DDSURFACEDESC2
{
    DWORD               dwSize;                 // size of the DDSURFACEDESC structure
    DWORD               dwFlags;                // determines what fields are valid
    DWORD               dwHeight;               // height of surface to be created
    DWORD               dwWidth;                // width of input surface
    union
    {
        LONG            lPitch;                 // distance to start of next line (return value only)
        DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
    } DUMMYUNIONNAMEN(1);
    DWORD               dwBackBufferCount;      // number of back buffers requested
    union
    {
        DWORD           dwMipMapCount;          // number of mip-map levels requestde
        // dwZBufferBitDepth removed, use ddpfPixelFormat one instead
        DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
        DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
    } DUMMYUNIONNAMEN(2);
    DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
    DWORD               dwReserved;             // reserved
    LPVOID              lpSurface;              // pointer to the associated surface memory
    union
    {
        DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
        DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
    } DUMMYUNIONNAMEN(3);
    DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
    DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
    DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
    union
    {
        DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
        DWORD           dwFVF;                  // vertex format description of vertex buffers
    } DUMMYUNIONNAMEN(4);
   // DDSCAPS2            ddsCaps;                // direct draw surface capabilities
    DWORD               dwTextureStage;         // stage in multitexture cascade
} DDSURFACEDESC2, *LPDDSURFACEDESC2;
*/
typedef struct IDirectDrawSurface       FAR *LPDIRECTDRAWSURFACE;
//typedef struct IDirectDrawSurface2      FAR *LPDIRECTDRAWSURFACE2;
typedef struct IDirectDraw              FAR *LPDIRECTDRAW;
typedef struct IDirectDrawPalette               FAR *LPDIRECTDRAWPALETTE;
typedef struct IDirectDrawClipper               FAR *LPDIRECTDRAWCLIPPER;


typedef RECT * LPRECT;

class IDirectDrawSurface {
public:
    HRESULT Lock(
                 LPRECT lpDestRect,
                 LPDDSURFACEDESC lpDDSurfaceDesc,
                 DWORD dwFlags,
                 HANDLE hEvent
                 );
    HRESULT Unlock(
                   LPRECT lpRect
                   );
    HRESULT GetSurfaceDesc(
                           LPDDSURFACEDESC lpDDSurfaceDesc
                           )
    {
        assert(false);
    }
};


#endif /* directx_h */
