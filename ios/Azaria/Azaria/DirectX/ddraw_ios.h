//
//  ddraw_ios.h
//  Azaria
//
//  Created by guy on 11/14/17.
//  Copyright © 2017 guy. All rights reserved.
//

#ifndef ddraw_ios_h
#define ddraw_ios_h

typedef struct _RGBA8 {
    size_t width;
    size_t height;
    size_t bytesPerRow;
    size_t bpp;
    unsigned char *bitmapData;
} RGBA8Image;

#if defined __cplusplus
extern "C" {
#endif
    
    RGBA8Image * getBackBuffer(void);
    void paintBuffer(RGBA8Image * surface);
    void ddraw_ios_init(void);
    
#if defined __cplusplus
};
#endif

#endif /* ddraw_ios_h */
