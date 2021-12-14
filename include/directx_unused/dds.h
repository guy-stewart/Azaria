//
//  dds.h
//  Azaria
//
//  Created by guy on 11/12/17.
//  Copyright © 2017 guy. All rights reserved.
//

#ifndef dds_h
#define dds_h

#ifndef INC_DDS_H
#define INC_DDS_H

/**************************************************************************\
 
 GTI-Framework - C++ library for realtime graphics rendering using OpenGL.
 Copyright (C) 2001-2008 Universitat Pompeu Fabra - Barcelona (Spain)
 
 This file is part of GTI-Framework.
 
 GTI-Framework is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 GTI-Framework is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with GTI-Framework.  If not, see <http://www.gnu.org/licenses/>.
 
 *****************************************************************************/

// typdefs for linux
#include "ptypes.h"
//#include "winerror.h"

#define DD_OK                                   0
#define DD_FALSE                                S_FALSE

// Operation successful
#define S_OK      0x00000000
// Operation aborted
#define E_ABORT     0x80004004
// General access denied error
#define E_ACCESSDENIED    0x80070005
// Unspecified failure
#define E_FAIL       0x80004005
// Handle that is not valid
#define E_HANDLE       0x80070006
// One or more arguments are not valid
#define E_INVALIDARG       0x80070057
// No such interface supported
#define E_NOINTERFACE       0x80004002
// Not implemented
#define E_NOTIMPL       0x80004001
// Failed to allocate necessary memory
#define E_OUTOFMEMORY       0x8007000E
// Pointer that is not valid
#define E_POINTER       0x80004003
// Unexpected failure
#define E_UNEXPECTED       0x8000FFFF


#ifndef DUMMYUNIONNAMEN
#if defined(__cplusplus) || !defined(NONAMELESSUNION)
#define DUMMYUNIONNAMEN(n)
#else
#define DUMMYUNIONNAMEN(n)      u##n
#endif
#endif



#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

/*
 * FOURCC codes for DX compressed-texture pixel formats
 */
#define FOURCC_DXT1  (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2  (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3  (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4  (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5  (MAKEFOURCC('D','X','T','5'))


/*
 * DDSCAPS2
 */
typedef struct _DDSCAPS2
{
    DWORD       dwCaps;         // capabilities of surface wanted
    DWORD       dwCaps2;
    DWORD       dwCaps3;
    union
    {
        DWORD       dwCaps4;
        DWORD       dwVolumeDepth;
    } DUMMYUNIONNAMEN(1);
} DDSCAPS2;



/*
 * DDPIXELFORMAT
 */
typedef struct _DDPIXELFORMAT
{
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


/*
 * DDSURFACEDESC2
 */
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
    union
    {
        DWORD           dwBackBufferCount;      // number of back buffers requested
        DWORD           dwDepth;                // the depth if this is a volume texture
    } DUMMYUNIONNAMEN(5);
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
    DDSCAPS2            ddsCaps;                // direct draw surface capabilities
    DWORD               dwTextureStage;         // stage in multitexture cascade
    
} DDSURFACEDESC2;

//dds flags
#define DDSD_CAPS 0x00000001
#define DDSD_HEIGHT 0x00000002
#define DDSD_WIDTH 0x00000004
#define DDSD_PITCH 0x00000008
#define DDSD_PIXELFORMAT 0x00001000
#define DDSD_MIPMAPCOUNT 0x00020000
#define DDSD_LINEARSIZE 0x00080000
#define DDSD_DEPTH 0x00800000

//dds pixel format flags
#define DDPF_ALPHAPIXELS 0x00000001            //The surface has alpha channel information in the pixel format.
#define DDPF_ALPHA 0x00000002                //The pixel format describes an alpha-only surface.
#define DDPF_FOURCC 0x00000004                //The FourCC code is valid.
#define DDPF_PALETTEINDEXED8 0x00000020        //Unsupported
#define DDPF_RGB 0x00000040                    //The RGB data in the pixel format structure is valid.
#define DDPF_LUMINANCE 0x00020000

#define DDPF_8BITS_MASK 0xFF000000 //mask

//dds caps flags
#define DDSCAPS_UNKNOWN_FLAG 0x00000002 //i dont know which means this bit but it matters
#define DDSCAPS_COMPLEX        0x00000008
#define DDSCAPS_MIPMAP        0x00400000
#define DDSCAPS_TEXTURE        0x00001000L

#define DDSCAPS2_CUBEMAP    0x00000200L
#define DDSCAPS2_VOLUME        0x00200000L

#define DDSCAPS2_CUBEMAP_POSITIVEX              0x00000400L
#define DDSCAPS2_CUBEMAP_NEGATIVEX              0x00000800L
#define DDSCAPS2_CUBEMAP_POSITIVEY              0x00001000L
#define DDSCAPS2_CUBEMAP_NEGATIVEY              0x00002000L
#define DDSCAPS2_CUBEMAP_POSITIVEZ              0x00004000L
#define DDSCAPS2_CUBEMAP_NEGATIVEZ              0x00008000L

/*
 * This macro may be used to specify all faces of a cube map at CreateSurface time
 */
#define DDSCAPS2_CUBEMAP_ALLFACES ( DDSCAPS2_CUBEMAP_POSITIVEX |\
DDSCAPS2_CUBEMAP_NEGATIVEX |\
DDSCAPS2_CUBEMAP_POSITIVEY |\
DDSCAPS2_CUBEMAP_NEGATIVEY |\
DDSCAPS2_CUBEMAP_POSITIVEZ |\
DDSCAPS2_CUBEMAP_NEGATIVEZ )




#endif //(INC_DDS_H)

#endif /* dds_h */
