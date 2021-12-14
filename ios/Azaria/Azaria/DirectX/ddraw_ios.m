//
//  ddraw_ios.m
//  Azaria
//
//  Created by guy on 11/14/17.
//  Copyright © 2017 guy. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "ddraw_ios.h"
#import "ImageHelper.h"

@interface ddraw_ios : NSObject {
    UIImage *source;
    RGBA8Image * rgba8;
}

+ (void)objective;
- (RGBA8Image*)getSurface;
@end

@implementation ddraw_ios : NSObject
- (id)init
{
    if( self = [super init] )
    {
        // Initialize your object here
        source = [UIImage imageNamed:@"GUI.JPG"];
        rgba8 = [ImageHelper convertUIImageToBitmapRGBA8:source];
        memset(rgba8->bitmapData, 0xff, rgba8->height * rgba8->width * 4);
    }
    return self;
}

+ (void)objective
{
}

- (RGBA8Image *)getSurface
{
    return rgba8;
}


@end

ddraw_ios *the_ios=nil;
void ddraw_ios_init()
{
    the_ios = [[ddraw_ios alloc] init];
}

RGBA8Image * getBackBuffer(void)
{
    RGBA8Image * surface;
    surface = [the_ios getSurface];
    [ddraw_ios objective];
    return surface;
}

